#!/bin/bash
# Utility script for flashing the target image on an SD card.

SD_IMAGE=buildroot/output/images/sdcard.img

if [ ! -e $SD_IMAGE ]
then
    echo "No sdcard.img found"
    exit 1
fi

# Print removable block devices for the user
echo "Removable block devices:"
mapfile -t DEVICES < <(
    lsblk -dpno NAME,SIZE,MODEL,TRAN |
    grep -E 'usb|mmc' |
    sed '/^$/d'               # drop empty lines
)

if [[ ${#DEVICES[@]} -eq 0 ]]; then
    echo "No removable USB/MMC block devices detected."
    exit 1
fi

# Print with an index
for i in "${!DEVICES[@]}"; do
    printf "  [%2d] %s\n" "$((i+1))" "${DEVICES[$i]}"
done

echo "============================================"
read -rp "Enter the number of the device you want to flash: " NUM
if ! [[ "$NUM" =~ ^[0-9]+$ ]]; then
    echo "Invalid input – not a number."
    exit 1
fi
INDEX=$((NUM-1))
if (( INDEX < 0 || INDEX >= ${#DEVICES[@]} )); then
    echo "Number out of range."
    exit 1
fi

# The first column of the line is the device path (e.g. /dev/sdb)
DEV=$(awk '{print $1}' <<<"${DEVICES[$INDEX]}")
if [[ ! -b "$DEV" ]]; then
    echo "Error: $DEV is not a block device."
    exit 1
fi

SIZE=$(sudo blockdev --getsize64 "$DEV")
printf "\nTarget device: %s\nSize: %.2f GiB\n" "$DEV" "$(bc <<<"$SIZE/1073741824")"
read -rp "WARNING: THIS WILL ERASE EVERYTHING ON $DEV. Continue? [y/N]: " CONFIRM
if [[ "$CONFIRM" != "y" ]]; then
    echo "Aborted."
    exit 1
fi

echo "Flashing $SD_IMAGE to $DEV ..."
sudo dd if="$SD_IMAGE" of="$DEV" bs=4M conv=fsync status=progress

sync
echo "Done! You can now safely eject the card."