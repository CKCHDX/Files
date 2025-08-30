#!/bin/bash
# install_packages.sh
# Reads package names from oscyra.txt and installs them via apt

while read -r pkg; do
    # Skip empty lines or lines starting with #
    [[ -z "$pkg" || "$pkg" =~ ^# ]] && continue

    echo "Installing $pkg..."
    if ! sudo apt install -y "$pkg"; then
        echo "Skipping $pkg (failed)"
    fi
done < oscyra.txt
