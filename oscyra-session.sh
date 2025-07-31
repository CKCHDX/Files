#!/bin/bash
# oscyra-session.sh — Run by systemd as guest user on tty1

set -euo pipefail

export DISPLAY=:0
export XDG_SESSION_TYPE=x11
export XDG_RUNTIME_DIR="/run/user/$(id -u)"

# Start X server on VT1
echo ">>> Starting X server on VT1..."
/usr/bin/Xorg :0 vt1 -nolisten tcp &
X_PID=$!

# Wait for X server to come up (max 10 seconds)
for i in {1..10}; do
    if xdpyinfo -display :0 >/dev/null 2>&1; then
        break
    fi
    echo ">>> Waiting for Xorg..."
    sleep 1
done

if ! xdpyinfo -display :0 >/dev/null 2>&1; then
    echo ">>> ERROR: Xorg failed to start on :0"
    kill "$X_PID"
    exit 1
fi

echo ">>> Starting kwin..."
kwin_x11 --replace &

# Start D-Bus if not already running
if ! pgrep -x dbus-daemon >/dev/null; then
    echo ">>> Launching D-Bus..."
    eval "$(dbus-launch --sh-syntax)"
fi

# Start your Oscyra login UI
echo ">>> Starting Oscyra Login UI..."
/usr/bin/Oscyra &

# Wait on X to exit
wait "$X_PID"
