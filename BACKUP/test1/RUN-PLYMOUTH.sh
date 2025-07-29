#!/bin/bash
set -e
set -x

THEME_NAME="oscyra"               # Your new theme name
THEME_SRC_DIR="./plymouth"        # Folder containing your custom plymouth theme files (pngs, .plymouth, script)
THEME_DST_DIR="/usr/share/plymouth/themes/$THEME_NAME"

# Step 1: Check if source theme directory exists
if [ ! -d "$THEME_SRC_DIR" ]; then
    echo "Error: Plymouth theme source directory '$THEME_SRC_DIR' not found."
    exit 1
fi

# Step 2: Backup current Plymouth theme (optional)
echo "Backing up current Plymouth theme..."
CURRENT_THEME=$(plymouth-set-default-theme)
BACKUP_DIR="/usr/share/plymouth/themes/${CURRENT_THEME}_backup_$(date +%F-%T)"
if [ -d "/usr/share/plymouth/themes/$CURRENT_THEME" ]; then
    sudo cp -r "/usr/share/plymouth/themes/$CURRENT_THEME" "$BACKUP_DIR"
    echo "Backup saved to $BACKUP_DIR"
fi

# Step 3: Remove old theme if exists, then copy your custom theme
echo "Installing new Plymouth theme '$THEME_NAME'..."
sudo rm -rf "$THEME_DST_DIR"
sudo cp -r "$THEME_SRC_DIR" "$THEME_DST_DIR"

# Step 4: Set Plymouth theme to your new theme
echo "Setting Plymouth theme to '$THEME_NAME'..."
sudo plymouth-set-default-theme "$THEME_NAME"

# Step 5: Update initramfs so new theme takes effect
echo "Updating initramfs..."
sudo update-initramfs -u

# Step 6: Confirm current Plymouth theme
echo "Current Plymouth theme:"
plymouth-set-default-theme

# Step 7: Done
echo "Plymouth theme changed to '$THEME_NAME'. Reboot to see changes."
