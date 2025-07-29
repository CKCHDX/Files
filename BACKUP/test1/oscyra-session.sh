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
/usr/bin/oscyra-login

wait
