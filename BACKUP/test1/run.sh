#!/bin/bash
# create_oscyra_distro.sh
# Create an "Oscyra" Debian-based install from a live environment.
# Run as root. Destroys the target disk you choose.

set -u

OSCYRA_PKG_LIST="./oscyra.txt"
DOWNLOAD_DIR="/tmp/oscyra_downloads"
TARGET_MOUNT="/mnt/oscyra_target"
CHROOT="$TARGET_MOUNT"
DEBOOTSTRAP_SUITE="bookworm"   # adjust if you want stable/testing/your-preference
ARCH="amd64"

# The oscyra-session.desktop content (will be installed into target)
/usr_local_oscyra_desktop="[Desktop Entry]
Name=Oscyra Session
Comment=Custom Oscyra Session Launcher
Exec=/usr/bin/oscyra-session.sh
TryExec=/usr/bin/oscyra-session.sh
Type=Application
"

# The oscyra-session.sh content (will be installed into target)
read -r -d '' OSCYRA_SESSION_SH <<'EOF'
#!/bin/bash
Xorg :0 &

# Wait a few seconds for Xorg to initialize
sleep 3

# Start kwin_x11 standalone
kwin_x11 &

# Optionally start dbus (if your apps need it)
if ! pgrep -x dbus-daemon > /dev/null; then
  eval $(dbus-launch --sh-syntax)
fi

# Start your custom login GUI
/usr/bin/Oscyra

wait
EOF

# helper
log() { echo -e "\n[+] $*"; }
warn() { echo -e "\n[!] $*" >&2; }

if [ "$(id -u)" -ne 0 ]; then
  warn "This script must be run as root."
  exit 1
fi

if [ ! -f "$OSCYRA_PKG_LIST" ]; then
  warn "Cannot find $OSCYRA_PKG_LIST in the current directory. Put your oscyra.txt here and retry."
  exit 1
fi

# Read package list into an array (ignore empty lines and comments)
mapfile -t PACKS < <(grep -vE '^\s*$|^\s*#' "$OSCYRA_PKG_LIST" | tr -d '\r')

log "Packages to process: ${#PACKS[@]} packages."

# Ensure apt is updated (you said you already ran apt update; still do it)
log "Running apt-get update..."
apt-get update -y || warn "apt-get update returned non-zero, continuing."

# Ensure debootstrap exists
if ! command -v debootstrap >/dev/null 2>&1; then
  log "Installing debootstrap..."
  apt-get install -y debootstrap || { warn "Failed to install debootstrap. You need it to continue. Exiting."; exit 1; }
fi

# Create download directory
mkdir -p "$DOWNLOAD_DIR"

log "Downloading packages (best-effort). Output saved in $DOWNLOAD_DIR."
for pkg in "${PACKS[@]}"; do
  if [ -z "$pkg" ]; then continue; fi
  log "Downloading: $pkg"
  apt-get download "$pkg" -y -o Dir::Cache::archives="$DOWNLOAD_DIR" >/dev/null 2>&1 || warn "Download failed for $pkg (ignored)."
done

log "Package download pass finished."

# Ask target device
# --- Choose target device ---
echo
read -p "Enter the target device (install disk, e.g. /dev/sda) : " TARGET_DEV
if [ -z "$TARGET_DEV" ] || [ ! -b "$TARGET_DEV" ]; then
  warn "Device not valid. Exiting."
  exit 1
fi

# Confirm wipe
read -p "ALL data on $TARGET_DEV will be erased. Type YES to continue: " CONFIRM
[ "$CONFIRM" != "YES" ] && { warn "Aborted by user."; exit 1; }

# Detect EFI automatically
if [ -d /sys/firmware/efi ]; then
  UEFI=true
else
  UEFI=false
fi
log "Boot mode detected: $([ "$UEFI" = true ] && echo UEFI || echo BIOS)."

# --- Partition disk ---
log "Creating new partition table on $TARGET_DEV..."
umount "${TARGET_DEV}"* 2>/dev/null || true

parted -s "$TARGET_DEV" mklabel gpt || { warn "Could not create GPT label on $TARGET_DEV"; exit 1; }

if [ "$UEFI" = true ]; then
  log "Creating EFI + root partitions"
  parted -s "$TARGET_DEV" mkpart ESP fat32 1MiB 513MiB
  parted -s "$TARGET_DEV" set 1 esp on
  parted -s "$TARGET_DEV" mkpart primary ext4 513MiB 100%
  EFI_PART="${TARGET_DEV}1"
  ROOT_PART="${TARGET_DEV}2"
else
  log "Creating single root partition"
  parted -s "$TARGET_DEV" mkpart primary ext4 1MiB 100%
  ROOT_PART="${TARGET_DEV}1"
fi

# Wait a bit for kernel to see partitions
sleep 1
log "Formatting partitions..."
if [ "$UEFI" = true ]; then
  mkfs.vfat -F32 "$EFI_PART" >/dev/null 2>&1 || { warn "mkfs.vfat failed on $EFI_PART"; exit 1; }
fi
mkfs.ext4 -F "$ROOT_PART" >/dev/null 2>&1 || { warn "mkfs.ext4 failed on $ROOT_PART"; exit 1; }

log "Mounting target root at $TARGET_MOUNT"
mkdir -p "$TARGET_MOUNT"
mount "$ROOT_PART" "$TARGET_MOUNT" || { warn "mount failed"; exit 1; }

if [ "$UEFI" = true ]; then
  mkdir -p "$TARGET_MOUNT/boot/efi"
  mount "$EFI_PART" "$TARGET_MOUNT/boot/efi" || { warn "mount efi failed"; exit 1; }
fi

log "Running debootstrap ($DEBOOTSTRAP_SUITE) to install base system..."
debootstrap --arch="$ARCH" "$DEBOOTSTRAP_SUITE" "$CHROOT" http://deb.debian.org/debian/ || warn "debootstrap returned non-zero, continuing (might still work)."

# Prepare chroot
log "Preparing chroot environment (/proc, /sys, /dev, resolv.conf)."
mount --bind /dev "$CHROOT/dev"
mount --bind /dev/pts "$CHROOT/dev/pts"
mount -t proc /proc "$CHROOT/proc"
mount -t sysfs /sys "$CHROOT/sys"
cp /etc/resolv.conf "$CHROOT/etc/resolv.conf"

# Copy apt caches (optional)
if [ -d "$DOWNLOAD_DIR" ]; then
  mkdir -p "$CHROOT/var/cache/apt/archives"
  cp -v "$DOWNLOAD_DIR"/"*.deb" "$CHROOT/var/cache/apt/archives/" 2>/dev/null || true
fi

# Create a small helper script to run inside chroot
CHROOT_SCRIPT="/tmp/oscyra_chroot_setup.sh"
cat > "$CHROOT_SCRIPT" <<'CHROOT'
#!/bin/bash
set -u

# minimal apt config
export DEBIAN_FRONTEND=noninteractive
apt-get update -y || true

# create locale, hostname & necessary packages
apt-get install -y --no-install-recommends locales systemd-sysv || true

# create user 'oscyra' (no password by default)
useradd -m -s /bin/bash oscyra || true
echo "oscyra:oscyra" | chpasswd || true
usermod -aG sudo,adm oscyra || true

# ensure systemd journal & required services
apt-get install -y sudo dbus udev policykit-1 cron cronolog || true

# Install sddm (will be enabled later)
# Note: package installs below come from the external script's PACKS variable
CHROOT_END_MARKER
CHROOT

# copy it into chroot
cp "$CHROOT_SCRIPT" "$CHROOT/tmp/oscyra_chroot_setup.sh"
chmod +x "$CHROOT/tmp/oscyra_chroot_setup.sh"

log "Entering chroot to perform package installs. This may take a while."

# Execute the initial chroot script
chroot "$CHROOT" /tmp/oscyra_chroot_setup.sh || warn "Initial chroot actions had non-zero exit status."

# Now install each package from the user's list inside chroot, with best-effort.
log "Installing packages inside chroot (best-effort; failures are ignored)."
for pkg in "${PACKS[@]}"; do
  [ -z "$pkg" ] && continue
  log "chroot apt-get install -y $pkg"
  chroot "$CHROOT" /bin/bash -c "export DEBIAN_FRONTEND=noninteractive; apt-get install -y --no-install-recommends $pkg" >/dev/null 2>&1 || warn "Install failed in chroot for $pkg (ignored)."
done

# Enable sddm, create session, copy files and configure autologin and X session
log "Configuring system for SDDM, creating session files, user home config..."

# Create session file and session script inside chroot
log "Deploying oscyra-session.sh and .desktop into target."
cat > "$CHROOT/usr/bin/oscyra-session.sh" <<'EOT'
#!/bin/bash
Xorg :0 &

sleep 3
kwin_x11 &

if ! pgrep -x dbus-daemon > /dev/null; then
  eval $(dbus-launch --sh-syntax)
fi

/usr/bin/Oscyra

wait
EOT
chmod +x "$CHROOT/usr/bin/oscyra-session.sh" || true

mkdir -p "$CHROOT/usr/share/xsessions"
cat > "$CHROOT/usr/share/xsessions/oscyra.desktop" <<'EOT'
[Desktop Entry]
Name=Oscyra
Comment=Oscyra Session
Exec=/usr/bin/oscyra-session.sh
Type=Application
EOT

# Ensure SDDM is installed (if not, try to install)
chroot "$CHROOT" /bin/bash -c "export DEBIAN_FRONTEND=noninteractive; apt-get install -y sddm" >/dev/null 2>&1 || warn "sddm install failed or returned non-zero."

# enable sddm & graphical target
chroot "$CHROOT" /bin/bash -c "systemctl enable sddm.service || true; systemctl set-default graphical.target || true"

# configure sddm autologin
mkdir -p "$CHROOT/etc/sddm.conf.d"
cat > "$CHROOT/etc/sddm.conf.d/10-autologin.conf" <<'EOT'
[Autologin]
User=oscyra
Session=oscyra.desktop
EOT

# ensure /usr/bin/Oscyra exists — if user didn't provide it, create a placeholder
if [ ! -f "$CHROOT/usr/bin/Oscyra" ]; then
  cat > "$CHROOT/usr/bin/Oscyra" <<'EOT'
#!/bin/bash
# Placeholder: replace this with your real /usr/bin/Oscyra binary or script.
echo "Oscyra placeholder starting..."
# Launch a terminal to show something
exec /usr/bin/konsole || exec /bin/bash
EOT
  chmod +x "$CHROOT/usr/bin/Oscyra"
fi

# Install grub
log "Installing GRUB bootloader (best-effort)."
if [ -d /sys/firmware/efi ] || [ "$UEFI" = true ]; then
  chroot "$CHROOT" /bin/bash -c "export DEBIAN_FRONTEND=noninteractive; apt-get install -y grub-efi-amd64 shim-signed" >/dev/null 2>&1 || warn "grub-efi install failed."
  # Make sure efivars is mounted inside chroot
  if [ ! -d "$CHROOT/sys/firmware/efi" ]; then
    mkdir -p "$CHROOT/sys/firmware/efi"
    mount -t efivarfs efivarfs "$CHROOT/sys/firmware/efi/efivars" 2>/dev/null || true

  fi
  chroot "$CHROOT" /bin/bash -c "grub-install --target=x86_64-efi --efi-directory=/boot/efi --bootloader-id=Oscyra --recheck; update-grub"
else
  chroot "$CHROOT" /bin/bash -c "export DEBIAN_FRONTEND=noninteractive; apt-get install -y grub-pc" >/dev/null 2>&1 || warn "grub-pc install failed."
  chroot "$CHROOT" /bin/bash -c "grub-install --target=i386-pc --recheck $TARGET_DEV; update-grub"
fi

# Clean up apt lists to reduce image size
chroot "$CHROOT" /bin/bash -c "apt-get clean || true; rm -rf /var/lib/apt/lists/*" >/dev/null 2>&1 || true

log "Chroot configuration complete."
# Force all writes to disk
sync
sleep 2

# Exit chroot and unmount
log "Cleaning up and unmounting chroot binds."
umount -l "$CHROOT/dev/pts" 2>/dev/null || true
umount -l "$CHROOT/dev" 2>/dev/null || true
umount -l "$CHROOT/proc" 2>/dev/null || true
umount -l "$CHROOT/sys" 2>/dev/null || true

# leave target mounted so the user can inspect before reboot; if you want automatic reboot, uncomment:
# log "Rebooting now..."
# sync; reboot

log "Done. The new system is installed on $TARGET_DEV and mounted at $TARGET_MOUNT."
log "When you boot it (in VirtualBox remove the live image and boot from the new disk), it should start sddm and auto-login the 'oscyra' user into the Oscyra session."
log "Notes:"
echo " - You may replace /usr/bin/Oscyra inside the installed system with your real binary before first boot:"
echo "     cp /path/to/your/Oscyra $TARGET_MOUNT/usr/bin/Oscyra && chmod +x $TARGET_MOUNT/usr/bin/Oscyra"
echo " - If anything fails during package installs, inspect logs: chroot apt logs at $TARGET_MOUNT/var/log/apt/."

exit 0
