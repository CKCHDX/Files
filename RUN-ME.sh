#!/bin/bash
# RUN-ME.sh — KDE Neon → Dynamic-OS conversion script
# Requires: root privileges (run via sudo or as root)

set -e
set -x

# Constants
REQUIRED_FILES=("Oscyra" "oscyra-session.sh" "oscyra-installer" "oscyra-start" "oscyra-session.desktop" "oscyra-login")
PLYMOUTH_THEME_NAME="oscyra"
PLYMOUTH_SRC_DIR="./plymouth"
PLYMOUTH_DST_DIR="/usr/share/plymouth/themes/$PLYMOUTH_THEME_NAME"
XWRAPPER_CONFIG="/etc/X11/Xwrapper.config"

# Check root
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root"
  exit 1
fi

# Step 0: Validate files
echo ">>> Checking for required files..."
for file in "${REQUIRED_FILES[@]}"; do
    if [[ ! -f "./$file" ]]; then
        echo "Missing required file: $file"
        exit 1
    fi
done

# Step 1: Create systemd service
echo ">>> Creating systemd service: /etc/systemd/system/oscyra-dm.service"
cat > /etc/systemd/system/oscyra-dm.service << 'EOF'
[Unit]
Description=Oscyra Custom Display Manager
After=getty@tty1.service
Requires=getty@tty1.service

[Service]
Type=simple
User=guest
Environment=DISPLAY=:0
TTYPath=/dev/tty1
StandardInput=tty
StandardOutput=journal
ExecStart=/usr/bin/oscyra-session.sh
Restart=always

[Install]
WantedBy=graphical.target
EOF

# Step 2: Install Oscyra files to /usr/bin/
echo ">>> Installing Oscyra files..."
for file in "${REQUIRED_FILES[@]}"; do
    install -m 755 "$file" /usr/bin/
done

# Step 3: Disable and stop SDDM
echo ">>> Disabling SDDM (if present)..."
systemctl disable sddm || true
systemctl stop sddm || true

# Step 4: Enable Oscyra custom DM
echo ">>> Enabling Oscyra DM service..."
systemctl daemon-reexec
systemctl daemon-reload
systemctl enable oscyra-dm.service

# Step 5: Configure autologin for guest on tty1
echo ">>> Setting up tty1 autologin for 'guest'..."
mkdir -p /etc/systemd/system/getty@tty1.service.d
cat > /etc/systemd/system/getty@tty1.service.d/override.conf << 'EOF'
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin guest --noclear %I $TERM
EOF

# Step 6: Set graphical target as default
echo ">>> Setting default systemd target to graphical..."
systemctl set-default graphical.target

# Step 7: Allow guest to run Xorg without root
echo ">>> Configuring X wrapper permissions for non-root Xorg..."
cat > "$XWRAPPER_CONFIG" << 'EOF'
allowed_users=anybody
needs_root_rights=yes
EOF

# Step 8: Install Plymouth theme
echo ">>> Installing Plymouth theme..."
if [ ! -d "$PLYMOUTH_SRC_DIR" ]; then
    echo "Error: Plymouth theme source directory '$PLYMOUTH_SRC_DIR' not found."
    exit 1
fi

CURRENT_THEME=$(plymouth-set-default-theme || echo "unknown")
BACKUP_DIR="/usr/share/plymouth/themes/${CURRENT_THEME}_backup_$(date +%F-%T)"

if [ -d "/usr/share/plymouth/themes/$CURRENT_THEME" ]; then
    echo ">>> Backing up current Plymouth theme to: $BACKUP_DIR"
    cp -r "/usr/share/plymouth/themes/$CURRENT_THEME" "$BACKUP_DIR"
fi

rm -rf "$PLYMOUTH_DST_DIR"
cp -r "$PLYMOUTH_SRC_DIR" "$PLYMOUTH_DST_DIR"
plymouth-set-default-theme "$PLYMOUTH_THEME_NAME"
update-initramfs -u -k all

# Step 9: Final status checks
echo ">>> Final status checks:"
systemctl status oscyra-dm.service || true
journalctl -u oscyra-dm.service -b || true

echo
echo ">>> Conversion complete!"
echo ">>> Please reboot your system to enter Dynamic-OS."
