#!/bin/bash
# RUN-ME.sh — Debian Minimal → Dynamic OS conversion script
# Must be run as root

set -e
set -x

REQUIRED_FILES=("Oscyra" "oscyra-session.sh" "oscyra-installer" "oscyra-login")
PACKAGES_LIST="oscyra.txt"
PLYMOUTH_THEME_NAME="oscyra"
PLYMOUTH_SRC_DIR="./plymouth"
PLYMOUTH_DST_DIR="/usr/share/plymouth/themes/$PLYMOUTH_THEME_NAME"
XWRAPPER_CONFIG="/etc/X11/Xwrapper.config"

# --- Step 0: Root check ---
if [ "$EUID" -ne 0 ]; then
  echo "Please run as root"
  exit 1
fi

# --- Step 1: Ensure network ---
echo ">>> Checking network..."
if ! ping -c1 debian.org &>/dev/null; then
  echo "No internet connection. Please connect to a network before running."
  exit 1
fi

# --- Step 2: Ensure guest user exists ---
if ! id -u guest >/dev/null 2>&1; then
  echo ">>> Creating guest user..."
  adduser --disabled-password --gecos "" guest
fi

# --- Step 3: Install packages from oscyra.txt ---
if [[ -f "$PACKAGES_LIST" ]]; then
  echo ">>> Installing packages from $PACKAGES_LIST..."
  while IFS= read -r pkg; do
    [[ -z "$pkg" || "$pkg" == \#* ]] && continue
    apt-get install -y "$pkg" || echo "Package $pkg failed, skipping..."
  done < "$PACKAGES_LIST"
else
  echo ">>> No $PACKAGES_LIST found, skipping package install."
fi

# --- Step 4: Install Oscyra files ---
echo ">>> Installing Oscyra files..."
for file in "${REQUIRED_FILES[@]}"; do
    if [[ ! -f "./$file" ]]; then
        echo "Missing required file: $file"
        exit 1
    fi
    install -m 755 "$file" /usr/bin/
    chmod +x /usr/bin/"$file"
done

# --- Step 5: Create systemd service ---
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

# --- Step 6: Enable autologin ---
echo ">>> Setting up autologin for guest..."
mkdir -p /etc/systemd/system/getty@tty1.service.d
cat > /etc/systemd/system/getty@tty1.service.d/override.conf << 'EOF'
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin guest --noclear %I $TERM
EOF

# --- Step 7: Configure Xorg permissions ---
echo ">>> Configuring Xorg permissions..."
cat > "$XWRAPPER_CONFIG" << 'EOF'
allowed_users=anybody
needs_root_rights=yes
EOF

# --- Step 8: Set Plymouth theme (optional) ---
if command -v plymouth-set-default-theme >/dev/null; then
  echo ">>> Installing Plymouth theme..."
  if [ -d "$PLYMOUTH_SRC_DIR" ]; then
    rm -rf "$PLYMOUTH_DST_DIR"
    cp -r "$PLYMOUTH_SRC_DIR" "$PLYMOUTH_DST_DIR"
    plymouth-set-default-theme "$PLYMOUTH_THEME_NAME"
    update-initramfs -u -k all
  else
    echo ">>> No plymouth theme directory found, skipping..."
  fi
else
  echo ">>> Plymouth not installed, skipping theme setup..."
fi

# --- Step 9: Enable Oscyra service ---
systemctl daemon-reload
systemctl enable oscyra-dm.service
systemctl set-default graphical.target

# --- Step 10: Ask to export system ---
read -p "Do you want to export the current system to an ISO/IMG before reboot? (y/N): " EXPORT_CHOICE
if [[ "$EXPORT_CHOICE" =~ ^[Yy]$ ]]; then
    echo ">>> Installing live-build tools..."
    apt-get install -y live-build
    echo ">>> Building ISO image..."
    mkdir -p /mnt/dynamicos-export
    lb config
    lb build
    mv *.iso /mnt/dynamicos-export/DynamicOS-$(date +%F).iso
    echo "ISO saved to /mnt/dynamicos-export/"
fi

echo ">>> Conversion complete! Reboot to start Dynamic OS."
