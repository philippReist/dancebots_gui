#!/bin/sh
#
# Automatically creates an APK shortcut for temi's Launcher OS
#
# Usage
#   ./shortcut.sh <apk> <shortcut-name>
#

# constants
# should match CFBundleExecutable string in Info.plist
APP_NAME="Dancebots Editor"

# display usage instructions
usage()
{
  echo ""
  echo "usage: deploy.sh <build-dir> <qt-bin-dir> <qml-dir>"
  echo ""
  echo "Deploy app for macOS"
  echo ""
  echo "positional arguments:"
  echo ""
  echo "  build-dir             cmake build directory"
  echo "  qt-bin-dir            QT bin directory"
  echo "  qml-dir               QML directory"
  echo ""
  exit
}

# arguments
# check for build directory
if [ -z "$1" ]; then
  echo "Missing build directory"
  usage
else
  BUILD_DIR=$1
fi

# check for QT bin directory
if [ -z "$2" ]; then
  echo "Missing QT bin directory (absolute path)"
  usage
else
  QT_BIN_DIR=$2
fi

# check for QML directory
if [ -z "$3" ]; then
  echo "Missing QML directory (absolute path)"
  usage
else
  QML_DIR=$3
fi


# create app and resources directory
mkdir -vp "${BUILD_DIR}/${APP_NAME}.app/Contents/MacOS"
mkdir -vp "${BUILD_DIR}/${APP_NAME}.app/Contents/Resources"

# copy executable, Info.plist, and Icon file
cp -vf "${BUILD_DIR}/gui/${APP_NAME}" "${BUILD_DIR}/${APP_NAME}.app/Contents/MacOS/"
cp -vf ./applogo.icns "${BUILD_DIR}/${APP_NAME}.app/Contents/Resources/"
cp -vf ./Info.plist "${BUILD_DIR}/${APP_NAME}.app/Contents"

# copy the appropriate frameworks to the app
cd "${BUILD_DIR}"
"${QT_BIN_DIR}/macdeployqt" "./${APP_NAME}.app" -qmldir="${QML_DIR}" -dmg
