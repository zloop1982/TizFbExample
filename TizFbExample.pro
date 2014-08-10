# TizFbExample.pro
# Copyright (C) Christian Schabesberger 2013 <chris.schabesberger@gmail.com>
#
# TizFbExample is free software: you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# TizFbExample is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License along
# with this program. If not, see <http://www.gnu.org/licenses/>.

include(tizFacebook/tizFacebook.pri)

QT       += core gui

QT += widgets

TARGET = TizFbExample
TEMPLATE = app

FORMS    += mainWindow.ui

CONFIG += mobility
MOBILITY = 

HEADERS += \
    src/mainWindow.hpp


SOURCES += \
    src/mainWindow.cpp \
    src/main.cpp

ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android

OTHER_FILES += \
    android/AndroidManifest.xml \
    android/res/values/strings.xml \
    android/src/org/schabi/tiz/TizActivityFb.java
