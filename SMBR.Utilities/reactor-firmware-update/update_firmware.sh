#!/bin/bash

usage() {
    echo "Usage: $0 [-l local_firmware_path] [-f]"
    echo "  Without arguments it uses existing firmware from firmware directory"
    echo "  -l : Path to local firmware zip file"
    echo "  -f : Fetch latest firmware from GitHub"
    echo "  -h : Show this help"
    exit 1
}

# Define the firmware directory
firmware_dir="./firmware"

# Parse command line arguments
local_firmware=""
install_existing=false
fetch_firmware=false
while getopts "l:fh" opt; do
    case $opt in
        l) local_firmware="$OPTARG" ;;
        f) fetch_firmware=true ;;
        h) usage ;;
        ?) usage ;;
    esac
done

# Ensure at least one option is specified
if [ -z "$local_firmware" ] && [ "$fetch_firmware" = false ]; then
    install_existing=true
    echo "Install existing firmware"
fi

# Ensure the firmware directory exists
mkdir -p "$firmware_dir"


if [ "$install_existing" = false ]; then
    if [ -n "$local_firmware" ]; then
        if [ ! -f "$local_firmware" ]; then
            echo "Error: Local firmware file not found: $local_firmware"
            exit 1
        fi
        echo "Using local firmware: $local_firmware"
        cp "$local_firmware" "$firmware_dir/$(basename "$local_firmware")"
        archive_name="$(basename "$local_firmware")"
    elif [ "$fetch_firmware" = true ]; then
        # Fetch the latest release URL
        url=$(curl -s https://api.github.com/repos/TrendBit/SMBR-firmware/releases/latest | grep "browser_download_url.*\.zip" | cut -d '"' -f 4)

        if [ -n "$url" ]; then
            echo "Downloading from: $url"
            # Download the archive
            archive_name="$(basename "$url")"
            curl -L -o "$firmware_dir/$archive_name" "$url"
        else
            echo "Error: Could not fetch the download URL."
            exit 1
        fi
    fi

    # Unzip the archive into the firmware directory
    unzip -o "$firmware_dir/$archive_name" -d "$firmware_dir"
    echo "Firmware unzipped to $firmware_dir"
fi

# Stop the services
echo "Stopping services..."
systemctl stop reactor-api-server
systemctl stop reactor-core-module

# Install the firmware
echo "Installing firmware..."
cd $firmware_dir
python3 update_all_modules.py --yes -d binaries

# Start the services
echo "Starting services..."
systemctl start reactor-core-module
systemctl start reactor-api-server

