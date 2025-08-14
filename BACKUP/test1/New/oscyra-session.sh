#!/bin/bash
export DISPLAY=:0
export XAUTHORITY=/home/guest/.Xauthority

# Start Xorg on tty1
Xorg :0 vt1 &

# Wait for Xorg to be ready
for i in {1..10}; do
    if xdpyinfo -display :0 >/dev/null 2>&1; then
        break
    fi
    sleep 1
done

# Start DBus session
if ! pgrep -x dbus-daemon > /dev/null; then
    eval $(dbus-launch --exit-with-session --sh-syntax)
fi

# Start KWin
kwin_x11 &

# Launch installer if not installed
if [[ ! -f /var/lib/dynamicos-installed ]]; then
    /usr/bin/oscyra-installer
else
    /usr/bin/Oscyra
fi

wait
