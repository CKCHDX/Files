#!/bin/bash
# oscyra-setup.sh
# Run as root

# Update repositories
xbps-install -Syu

# Install system essentials
xbps-install -y \
  systemd bash sudo coreutils login dbus udev policykit-1 procps psmisc cron \
  xorg xserver-xorg xserver-xorg-video-all xserver-xorg-input-all xinit x11-utils \
  plasma-workspace plasma-desktop plasma-integration khotkeys kmenuedit kwayland kio kwin \
  kded5 kglobalaccel kde-cli-tools libplasma libkworkspace libtaskmanager libkscreenlocker \
  sddm \
  qt5-devel qttools5-dev qttools5-dev-tools qt5-webengine libqt5widgets5 libqt5webenginewidgets5 \
  libqt5dbus5 libqt5gui5 libqt5core5a libqt5network5 libqt5svg5 \
  libkf5configcore5 libkf5widgetsaddons5 libkf5coreaddons5 libkf5guiaddons5 libkf5iconthemes5 \
  libkf5itemviews5 libkf5service-bin libkf5dbusaddons5 \
  networkmanager plasma-nm upower udisks2 kio-extras solid \
  base-devel cmake make gcc g++ \
  fonts-dejavu fonts-noto fonts-noto-color-emoji breeze breeze-gtk-theme breeze-icon-theme \
  oxygen-icon-theme gtk-engines gtk-engines-murrine kde-config-gtk-style qt5-style-kvantum \
  dolphin konsole spectacle kate ark okular

# Enable system services
ln -s /etc/sv/dbus /var/service/
ln -s /etc/sv/udev /var/service/
ln -s /etc/sv/NetworkManager /var/service/
ln -s /etc/sv/cron /var/service/
ln -s /etc/sv/sddm /var/service/

# Set SDDM as default display manager
ln -s /etc/sv/sddm /var/service/

# Create custom Oscyra session for SDDM
OSCYRA_DESKTOP=/usr/share/xsessions/oscyra.desktop
cat <<EOF > $OSCYRA_DESKTOP
[Desktop Entry]
Name=Oscyra
Comment=Custom Oscyra GUI
Exec=/usr/bin/oscyra-login
Type=Application
EOF

# Make Oscyra login script executable
cat <<'EOF' > /usr/bin/oscyra-login
#!/bin/bash

# Start Xorg (if not already running)
if ! pgrep -x Xorg > /dev/null; then
  Xorg :0 &
  sleep 3
fi

# Start kwin_x11 window manager
kwin_x11 &

# Start dbus if not running
if ! pgrep -x dbus-daemon > /dev/null; then
  eval $(dbus-launch --sh-syntax)
fi

# Start your custom GUI
/usr/bin/oscyra-login-app
EOF

chmod +x /usr/bin/oscyra-login

echo "Setup complete. You can now select 'Oscyra' in SDDM and login to start your GUI."
