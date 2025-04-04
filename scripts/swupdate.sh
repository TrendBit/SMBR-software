#!/bin/bash

# Check for required arguments
if [ $# -ne 2 ]; then
    echo "Usage: $0 <path-to-image.swu> <remote-ip>"
    exit 1
fi

# Arguments
SWU_FILE="$1"         # First argument: Path to SWU file
REMOTE_HOST="$2"      # Second argument: Remote device IP/hostname
REMOTE_USER="root"    # Change if needed
REMOTE_PATH="/tmp"    # Where to copy the SWU file on the device
SWUPDATE_CMD="cd $REMOTE_PATH && rpi_ab_update perform && rpi_ab_reboot "  # Adjust if needed

# Check if the SWU file exists
if [ ! -f "$SWU_FILE" ]; then
    echo "Error: SWU file '$SWU_FILE' not found!"
    exit 1
fi

echo "Copying $SWU_FILE to $REMOTE_HOST..."
scp "$SWU_FILE" "${REMOTE_USER}@${REMOTE_HOST}:$REMOTE_PATH/rpi-ab-update-image-bsp24xx.swu"
if [ $? -ne 0 ]; then
    echo "Error: Failed to copy $SWU_FILE to $REMOTE_HOST"
    exit 1
fi

echo "Running swupdate on $REMOTE_HOST..."
ssh "${REMOTE_USER}@${REMOTE_HOST}" "$SWUPDATE_CMD"
if [ $? -ne 0 ]; then
    echo "Error: swupdate failed!"
    exit 1
fi

echo "Update completed successfully!"
exit 0
