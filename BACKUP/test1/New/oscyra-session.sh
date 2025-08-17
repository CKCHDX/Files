#!/bin/bash
# Oscyra Session Script — Debian Minimal

export DISPLAY=:0
export XAUTHORITY=/home/guest/.Xauthority

# --- Step 1: Start Xorg on tty1 ---
if ! pgrep -x Xorg >/dev/null; then
    Xorg :0 vt1 & 
fi

# --- Step 2: Wait for Xorg to be ready ---
for i in {1..10}; do
    if xdpyinfo -display :0 >/dev/null 2>&1; then
        break
    fi
    sleep 1
done

# --- Step 3: Start D-Bus session (if not running) ---
if ! pgrep -x dbus-daemon >/dev/null; then
    eval "$(dbus-launch --exit-with-session --sh-syntax)"
fi

# --- Step 4: Start window manager (KWin) ---
if command -v kwin_x11 >/dev/null; then
    kwin_x11 &
else
    echo "Warning: KWin not installed, using basic X session."
    xterm &
fi

# --- Step 5: Run installer or main program ---
if [[ ! -f /var/lib/dynamicos-installed ]]; then
    /usr/bin/oscyra-installer
    touch /var/lib/dynamicos-installed
else
    /usr/bin/Oscyra
fi

wait
