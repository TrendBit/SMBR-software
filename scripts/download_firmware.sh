#!/bin/bash

# Define the firmware directory
firmware_dir="./SMBR.Services/smbr-firmware-update/firmware"

# Parse command line arguments
fetch_firmware=true


rm -rf "$firmware_dir"
# Ensure the firmware directory exists
mkdir -p "$firmware_dir"


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

# Unzip the archive into the firmware directory
unzip -o "$firmware_dir/$archive_name" -d "$firmware_dir"
echo "Firmware unzipped to $firmware_dir"
rm "$firmware_dir/$archive_name"
