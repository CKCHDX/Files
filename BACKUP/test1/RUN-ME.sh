#!/bin/bash
# Convert KDE Neon -> Dynamic-OS
# Recovery Mode Script (No GUI Required)
set -e
set -x  # Enable debug output

# Step 0: Prerequisites check
echo ">>> Checking required files..."
REQUIRED_FILES=("Oscyra" "oscyra-session.sh" "oscyra-installer" "oscyra-start" "oscyra-session.desktop" "oscyra-login")
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
After=network.target systemd-user-sessions.service

[Service]
User=guest
Environment=DISPLAY=:0
ExecStart=/usr/bin/oscyra-session.sh
Restart=always

[Install]
WantedBy=graphical.target
EOF

# Step 2: Move Oscyra files to /usr/bin
echo ">>> Copying Oscyra files to /usr/bin/"
for file in "${REQUIRED_FILES[@]}"; do
    install -m 755 "$file" /usr/bin/
done

# Step 3: Disable and stop other DMs (SDDM)
echo ">>> Disabling and stopping SDDM"
systemctl disable sddm || true
systemctl stop sddm || true

# Step 4: Enable custom DM service
echo ">>> Enabling Oscyra custom display manager"
systemctl daemon-reexec
systemctl daemon-reload
systemctl enable oscyra-dm.service

# Step 5: Autologin configuration for tty1
echo ">>> Configuring autologin for guest on tty1"
mkdir -p /etc/systemd/system/getty@tty1.service.d
cat > /etc/systemd/system/getty@tty1.service.d/override.conf << 'EOF'
[Service]
ExecStart=
ExecStart=-/sbin/agetty --autologin guest --noclear %I $TERM
EOF

# Step 6: Show current status and logs
echo ">>> Checking status of Oscyra service"
systemctl status oscyra-dm.service || true

echo ">>> Showing journal logs for Oscyra DM (latest boot)"
journalctl -u oscyra-dm.service -b || true

echo ">>> Conversion to Dynamic-OS complete. Reboot when ready."

