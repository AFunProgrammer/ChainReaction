#!/bin/bash
# Usage: ./makeicns.sh source_image.png [OutputName]

set -e

if [ -z "$1" ]; then
    echo "Usage: $0 source_image.png [OutputName]"
    exit 1
fi

SRC="$1"
BASENAME=$(basename "$SRC")
NAME="${2:-${BASENAME%.*}}"
ICONSET="${NAME}.iconset"

# Clean old builds
rm -rf "$ICONSET" "${NAME}.icns"
mkdir "$ICONSET"

# Function to run ffmpeg safely
make_icon() {
    ffmpeg -y -i "$SRC" -vf "scale=$1:$2" -frames:v 1 -update 1 "$3"
}

# Generate required sizes
make_icon 16   16   "$ICONSET/icon_16x16.png"
make_icon 32   32   "$ICONSET/icon_16x16@2x.png"
make_icon 32   32   "$ICONSET/icon_32x32.png"
make_icon 64   64   "$ICONSET/icon_32x32@2x.png"
make_icon 128 128   "$ICONSET/icon_128x128.png"
make_icon 256 256   "$ICONSET/icon_128x128@2x.png"
make_icon 256 256   "$ICONSET/icon_256x256.png"
make_icon 512 512   "$ICONSET/icon_256x256@2x.png"
make_icon 512 512   "$ICONSET/icon_512x512.png"
make_icon 1024 1024 "$ICONSET/icon_512x512@2x.png"

# Build .icns file
iconutil -c icns "$ICONSET"

echo "✅ Created ${NAME}.icns"
