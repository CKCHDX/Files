#!/bin/bash
# configure_oscyra.sh
# Configure an installed Debian 13 minimal system into "Oscyra".
# Run as root.

set -u

OSCYRA_PKG_LIST="./oscyra.txt"
USERNAME="oscyra"

log() { echo -e "\n[+] $*"; }
warn() { echo -e "\n[!] $*" >&2; }

if [ "$(id -u)" -ne 0 ]; then
  warn "This script must be run as root."
  exit 1
fi

if [ ! -f "$OSCYRA_PKG_LIST" ]; then
  warn "Cannot find $OSCYRA_PKG_LIST in current directory."
  exit 1
fi

# Read package list
mapfile -t PACKS < <(grep -vE '^\s*$|^\s*#' "$OSCYRA_PKG_LIST" | tr -d '\r')
log "Packages to process: ${#PACKS[@]}"

export DEBIAN_FRONTEND=noninteractive

# Update & install essentials
log "Updating system..."
apt-get update -y && apt-get upgrade -y

log "Installing essentials..."
apt-get install -y sudo dbus udev policykit-1 locales systemd-sysv

# Create user
if ! id "$USERNAME" >/dev/null 2>&1; then
  log "Creating user $USERNAME"
  useradd -m -s /bin/bash "$USERNAME"
  echo "$USERNAME:$USERNAME" | chpasswd
  usermod -aG sudo,adm "$USERNAME"
else
  log "User $USERNAME already exists"
fi

# Install all packages in oscyra.txt
log "Installing packages from oscyra.txt..."
for pkg in "${PACKS[@]}"; do
  log "Installing: $pkg"
  apt-get install -y --no-install-recommends "$pkg" || warn "Failed: $pkg"
done

# Ensure SDDM installed
log "Installing SDDM..."
apt-get install -y sddm

# Enable graphical login
systemctl enable sddm.service
systemctl set-default graphical.target

# Configure autologin
log "Configuring SDDM autologin for $USERNAME..."
mkdir -p /etc/sddm.conf.d
cat > /etc/sddm.conf.d/10-autologin.conf <<EOT
[Autologin]
User=$USERNAME
Session=oscyra.desktop
EOT

# Oscyra session script
log "Creating Oscyra session..."
cat > /usr/bin/oscyra-session.sh <<'EOT'
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
chmod +x /usr/bin/oscyra-session.sh

# Oscyra desktop entry
mkdir -p /usr/share/xsessions
cat > /usr/share/xsessions/oscyra.desktop <<'EOT'
[Desktop Entry]
Name=Oscyra
Comment=Oscyra Session
Exec=/usr/bin/oscyra-session.sh
Type=Application
EOT

# Provide placeholder /usr/bin/Oscyra
if [ ! -f /usr/bin/Oscyra ]; then
  log "Creating placeholder /usr/bin/Oscyra"
  cat > /usr/bin/Oscyra <<'EOT'
#!/bin/bash
echo "Oscyra placeholder..."
exec /usr/bin/konsole || exec /bin/bash
EOT
  chmod +x /usr/bin/Oscyra
fi

# Cleanup
log "Cleaning apt cache..."
apt-get clean
rm -rf /var/lib/apt/lists/*

log "Configuration complete! Reboot to start Oscyra."
