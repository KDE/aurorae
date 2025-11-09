#!/usr/bin/bash

# TODO Plasma 7: Drop aurorae.knsrc, use knsrc file provided by KWin.

if [ -f "@KDE_INSTALL_FULL_LIBEXECDIR@/kwin-applywindowdecoration" ]; then
    "@KDE_INSTALL_FULL_LIBEXECDIR@/kwin-applywindowdecoration" "$@"
elif [ -f "@KDE_INSTALL_FULL_LIBEXECDIR@/kwin-applywindowdecoration-x11" ]; then
    "@KDE_INSTALL_FULL_LIBEXECDIR@/kwin-applywindowdecoration-x11" "$@"
else
    echo "Neither kwin_x11 nor kwin_wayland is installed"
    exit 1
fi
