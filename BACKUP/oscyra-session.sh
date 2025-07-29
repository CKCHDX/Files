#!/bin/bash
FLAG="/etc/oscyra_installed"

export KDE_SKIP_INITIAL_PLASMASHELL=1
/usr/bin/startplasma-x11 &

sleep 4
killall plasmashell &

if [ ! -f "$FLAG" ]; then
    kdialog --msgbox "The installer needs root access. Default root password is: toor"
    su -c "/usr/bin/oscyra-installer" root
else
    /usr/bin/oscyra-start.sh
fi
