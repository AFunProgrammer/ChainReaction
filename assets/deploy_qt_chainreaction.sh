#!/bin/bash

# === CONFIGURATION ===
APP_BINARY="./chainreaction"
DEPLOY_DIR="./chainreaction_deploy"
LIB_DIR="$DEPLOY_DIR/usr/lib/chainreaction/lib"
PLUGIN_DIR="$DEPLOY_DIR/usr/lib/chainreaction/plugins/platforms"
BIN_INSTALL_DIR="$DEPLOY_DIR/usr/lib/chainreaction"
LAUNCHER_DIR="$DEPLOY_DIR/usr/bin"
DOC_DIR="$DEPLOY_DIR/usr/share/doc/chainreaction"
DESKTOP_DIR="$DEPLOY_DIR/usr/share/applications"
ICON_DIR="$DEPLOY_DIR/usr/share/icons/hicolor/128x128/apps"
CONTROL_FILE="$DEPLOY_DIR/DEBIAN/control"
APP_NAME="ChainReaction"
VERSION="1.0"
MAINTAINER="Inspiring Family <inspiringfamily.games@gmail.com>"

echo "🔧 Starting deployment for: $APP_BINARY"

# === 0. Create folder structure ===
mkdir -p "$LIB_DIR" "$PLUGIN_DIR" "$BIN_INSTALL_DIR" "$LAUNCHER_DIR" "$DOC_DIR" "$DESKTOP_DIR" "$ICON_DIR" "$DEPLOY_DIR/DEBIAN"

# === 1. Copy non-system shared libraries ===
echo "📦 Copying non-system shared libraries..."
ldd "$APP_BINARY" | awk '{print $3}' | while read -r lib; do
    if [[ -f "$lib" && "$lib" != /lib/* && "$lib" != /usr/lib/* ]]; then
        echo "    ➜ $lib"
        cp -u "$lib" "$LIB_DIR/"
    fi
done

# === 1b. Copy Qt platform plugin ===
QT_PLUGIN_SRC="$HOME/Qt/6.5.3/gcc_64/plugins/platforms/libqxcb.so"
if [[ -f "$QT_PLUGIN_SRC" ]]; then
    cp "$QT_PLUGIN_SRC" "$PLUGIN_DIR/"
else
    echo "⚠️  Missing libqxcb.so at $QT_PLUGIN_SRC"
fi

# === 1c. Copy Qt platform integration lib ===
QT_XCB_LIB="$HOME/Qt/6.5.3/gcc_64/lib/libQt6XcbQpa.so.6"
if [[ -f "$QT_XCB_LIB" ]]; then
    cp "$QT_XCB_LIB" "$LIB_DIR/"
else
    echo "⚠️  Missing libQt6XcbQpa.so.6"
fi

# === 1d. Copy ICU libraries used by Qt ===
for lib in libicudata.so.56.1 libicuuc.so.56.1 libicui18n.so.56.1; do
    src="$HOME/Qt/6.5.3/gcc_64/lib/$lib"
    if [[ -f "$src" ]]; then
        cp "$src" "$LIB_DIR/"
    fi
done

# === 1f. Copy xcbglintegration plugin ===
echo "📁 Copying xcbglintegration plugin..."
XCBGL_PLUGIN_SRC="$HOME/Qt/6.5.3/gcc_64/plugins/xcbglintegrations/libqxcb-glx-integration.so"
XCBGL_PLUGIN_DEST="$DEPLOY_DIR/usr/lib/chainreaction/plugins/xcbglintegrations"
mkdir -p "$XCBGL_PLUGIN_DEST"
if [[ -f "$XCBGL_PLUGIN_SRC" ]]; then
    cp "$XCBGL_PLUGIN_SRC" "$XCBGL_PLUGIN_DEST/"
    echo "    ➜ libqxcb-glx-integration.so copied"
else
    echo "⚠️  Missing libqxcb-glx-integration.so at $XCBGL_PLUGIN_SRC"
fi

# === 1g. Copy Wayland platform plugins ===
echo "📁 Copying Wayland platform plugins..."
WAYLAND_PLUGIN_SRC="$HOME/Qt/6.5.3/gcc_64/plugins/platforms"
WAYLAND_PLUGIN_DEST="$DEPLOY_DIR/usr/lib/chainreaction/plugins/platforms"

for plugin in libqwayland-egl.so libqwayland-generic.so; do
    if [[ -f "$WAYLAND_PLUGIN_SRC/$plugin" ]]; then
        cp "$WAYLAND_PLUGIN_SRC/$plugin" "$WAYLAND_PLUGIN_DEST/"
        echo "    ➜ $plugin copied"
    else
        echo "⚠️  Missing $plugin — check your Qt installation"
    fi
done


# === 2. Patch binary RPATH ===
echo "🧩 Patching binary RPATH to include plugin paths..."
patchelf --set-rpath '$ORIGIN/lib:$ORIGIN/plugins/platforms:$ORIGIN/plugins/xcbglintegrations' "$APP_BINARY"


# === 3. Copy binary ===
cp "$APP_BINARY" "$BIN_INSTALL_DIR/chainreaction.bin"

# === 4. Generate launcher script ===
echo "🚀 Creating launcher script..."
cat > "$LAUNCHER_DIR/chainreaction" <<EOF
#!/bin/bash
export LD_LIBRARY_PATH="\$(dirname "\$0")/../lib/chainreaction/lib"
export QT_QPA_PLATFORM_PLUGIN_PATH="\$(dirname "\$0")/../lib/chainreaction/plugins/platforms"
exec /usr/lib/chainreaction/chainreaction.bin "\$@"
EOF
chmod +x "$LAUNCHER_DIR/chainreaction"

# === 5. Create .desktop file ===
echo "🖥️  Adding .desktop file..."
cat > "$DESKTOP_DIR/chainreaction.desktop" <<EOF
[Desktop Entry]
Name=ChainReaction
Exec=/usr/bin/chainreaction
Icon=chainreaction
Type=Application
Categories=Game;
EOF

# === 6. Install icon (optional) ===
if [[ -f "./chainreaction.png" ]]; then
    cp ./chainreaction.png "$ICON_DIR/chainreaction.png"
fi

# === 7. Copy Qt license files ===
QT_LICENSE_DIR=$(find "$HOME/Qt" -type d -name "Src" | head -n 1)
if [[ -n "$QT_LICENSE_DIR" ]]; then
    cp "$QT_LICENSE_DIR"/LICENSE* "$DOC_DIR/" 2>/dev/null
fi

# === 8. Generate control file ===
echo "📝 Generating control file..."
cat > "$CONTROL_FILE" <<EOF
Package: $APP_NAME
Version: $VERSION
Architecture: amd64
Maintainer: $MAINTAINER
Depends: libc6, libx11-6, libxcb1, libgl1, libgtk-3-0
Section: games
Priority: optional
Description: Qt 6 Application: ChainReaction
 A simple Qt-based ChainReaction game packaged for Debian.
EOF

# === 9. Build .deb ===
echo "📦 To build the .deb package, run:"
echo "    dpkg-deb --build $DEPLOY_DIR ${APP_NAME}_${VERSION}_amd64.deb"

