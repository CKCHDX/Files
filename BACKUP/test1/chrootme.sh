#!/bin/bash
# chroot_config_oscyra.sh
# Configure a fresh Debian 13 chroot into "Oscyra".
# Must be run as root inside the chroot.

set -euo pipefail

OSCYRA_PKG_LIST="./oscyra.txt"
USERNAME="oscyra"

log() { echo -e "\n[+] $*"; }
warn() { echo -e "\n[!] $*" >&2; }

if [ "$(id -u)" -ne 0 ]; then
    warn "This script must be run as root inside the chroot."
    exit 1
fi

if [ ! -f "$OSCYRA_PKG_LIST" ]; then
    warn "Cannot find $OSCYRA_PKG_LIST in current directory."
    exit 1
fi

# Prepare environment for noninteractive apt
export DEBIAN_FRONTEND=noninteractive
export APT_LISTCHANGES_FRONTEND=none

# Read package list
mapfile -t PACKS < <(grep -vE '^\s*$|^\s*#' "$OSCYRA_PKG_LIST" | tr -d '\r')
log "Packages to process: ${#PACKS[@]}"

log "Updating package lists..."
apt-get update -y
apt-get upgrade -y

log "Installing essential packages..."
apt-get install -y sudo dbus udev policykit-1 locales systemd-sysv

# Create user
if ! id "$USERNAME" >/dev/null 2>&1; then
    log "Creating user $USERNAME..."
    useradd -m -s /bin/bash "$USERNAME"
    echo "$USERNAME:$USERNAME" | chpasswd
    usermod -aG sudo,adm "$USERNAME"
else
    log "User $USERNAME already exists"
fi

# Install all packages from oscyra.txt
log "Installing packages from oscyra.txt..."
for pkg in "${PACKS[@]}"; do
    log "Installing: $pkg"
    apt-get install -y --no-install-recommends "$pkg" || warn "Failed to install $pkg"
done

# Ensure SDDM installed and enable graphical target
log "Installing SDDM and enabling graphical target..."
apt-get install -y sddm
systemctl enable sddm.service
systemctl set-default graphical.target

# Configure SDDM autologin
log "Configuring SDDM autologin for $USERNAME..."
mkdir -p /etc/sddm.conf.d
cat > /etc/sddm.conf.d/10-autologin.conf <<EOT
[Autologin]
User=$USERNAME
Session=oscyra.desktop
EOT

# Create Oscyra session script
log "Creating Oscyra session script..."
cat > /usr/bin/oscyra-session.sh <<'EOT'
#!/bin/bash
# Start Xorg and KDE Plasma session for Oscyra

Xorg :0 &

sleep 3
kwin_x11 &

if ! pgrep -x dbus-daemon > /dev/null; then
    eval $(dbus-launch --sh-syntax)
fi

/usr/bin/Oscyra
wait
EOT
chmod +x /usr/bin/oscyra-session.sh

# Create Oscyra desktop entry
log "Creating Oscyra desktop entry..."
mkdir -p /usr/share/xsessions
cat > /usr/share/xsessions/oscyra.desktop <<'EOT'
[Desktop Entry]
Name=Oscyra
Comment=Oscyra Session
Exec=/usr/bin/oscyra-session.sh
Type=Application
EOT

# Placeholder for /usr/bin/Oscyra
if [ ! -f /usr/bin/Oscyra ]; then
    log "Creating placeholder /usr/bin/Oscyra..."
    cat > /usr/bin/Oscyra <<'EOT'
#!/bin/bash
echo "Oscyra placeholder GUI starting..."
exec /usr/bin/konsole || exec /bin/bash
EOT
    chmod +x /usr/bin/Oscyra
fi

# Clean up apt cache
log "Cleaning apt cache..."
apt-get clean
rm -rf /var/lib/apt/lists/*

log "Oscyra chroot configuration complete!"
