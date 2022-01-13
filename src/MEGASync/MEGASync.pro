#-------------------------------------------------
#
# Project created by QtCreator 2013-10-17T12:41:38
#
#-------------------------------------------------

win32:THIRDPARTY_VCPKG_BASE_PATH = C:/Users/build/MEGA/build-MEGAsync/3rdParty_MSVC2017_20200529
win32:contains(QMAKE_TARGET.arch, x86_64):VCPKG_TRIPLET = x64-windows-mega
win32:!contains(QMAKE_TARGET.arch, x86_64):VCPKG_TRIPLET = x86-windows-mega

macx {
    isEmpty(THIRDPARTY_VCPKG_BASE_PATH){
        THIRDPARTY_VCPKG_BASE_PATH = $$PWD/../../../3rdParty
    }
    VCPKG_TRIPLET = x64-osx-mega
}

unix:!macx:THIRDPARTY_VCPKG_BASE_PATH = $$PWD/../../../3rdParty
unix:!macx:VCPKG_TRIPLET = x64-linux

message("THIRDPARTY_VCPKG_BASE_PATH: $$THIRDPARTY_VCPKG_BASE_PATH")
message("VCPKG_TRIPLET: $$VCPKG_TRIPLET")


DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

win32:!contains(QMAKE_TARGET.arch, x86_64):DEFINES += PDFIUM_DELAY_LOAD_DLL=1

debug_and_release {
    CONFIG -= debug_and_release
    CONFIG += debug_and_release
}
CONFIG(debug, debug|release) {
    CONFIG -= debug release
    CONFIG += debug
}
CONFIG(release, debug|release) {
    CONFIG -= debug release
    CONFIG += release
}

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4) {
    QT += widgets
    BUILD_ARCH=$${QT_ARCH}
} else {
    BUILD_ARCH=$${QMAKE_HOST.arch}
}

unix:!macx {
    QT += svg x11extras
    TARGET = megasync

#    Uncomment the following if "make install" doesn't copy megasync in /usr/bin directory
#    isEmpty(PREFIX) {
#        PREFIX = /usr
#    }
#    target.path = $$PREFIX/bin
#    INSTALLS += target
}
else {
    TARGET = MEGAsync
}

TEMPLATE = app

#DEFINES += LOG_TO_LOGGER
#DEFINES += LOG_TO_FILE
#DEFINES += LOG_TO_STDOUT
DEFINES += ENABLE_LOG_PERFORMANCE

debug {
    CONFIG += console
    DEFINES += CREATE_COMPATIBLE_MINIDUMPS
    DEFINES += LOG_TO_STDOUT
#   DEFINES += LOG_TO_FILE
    DEFINES += LOG_TO_LOGGER
}

CONFIG += USE_LIBUV
CONFIG += USE_MEGAAPI
CONFIG += USE_MEDIAINFO
CONFIG += USE_LIBRAW
CONFIG += USE_FFMPEG

macx {
}
else:win32 {
DEFINES += NOMINMAX
}
else:contains(BUILD_ARCH, x86_64) { #Notice this might not work for clang!
}

unix:!macx {
        exists(/usr/include/ffmpeg-mega)|exists(mega/bindings/qt/3rdparty/include/ffmpeg)|packagesExist(ffmpeg)|packagesExist(libavcodec) {
            CONFIG += USE_FFMPEG
        }
}
else {
    CONFIG += USE_FFMPEG
}

# Drive notifications (for SDK)
CONFIG += USE_DRIVE_NOTIFICATIONS

include(gui/gui.pri)
include(mega/bindings/qt/sdk.pri)
include(control/control.pri)
include(model/model.pri)
include(platform/platform.pri)
include(google_breakpad/google_breakpad.pri)
include(qtlockedfile/qtlockedfile.pri)

unix:!macx {
    GCC_VERSION = $$system("g++ -dumpversion")
    lessThan(GCC_VERSION, 5) {
        LIBS -= -lstdc++fs
	QMAKE_CFLAGS += -std=c99
    }
}

DEPENDPATH += $$PWD
INCLUDEPATH += $$PWD

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII

!CONFIG(building_tests) {
    SOURCES += $$PWD/main.cpp
}

SOURCES += $$PWD/MegaApplication.cpp \
    $$PWD/DesktopNotifications.cpp \
    $$PWD/RemovedSharesNotificator.cpp \
    $$PWD/TransferQuota.cpp \
    $$PWD/UserAlertTimedClustering.cpp \
    $$PWD/ScaleFactorManager.cpp

HEADERS += $$PWD/MegaApplication.h \
    $$PWD/DesktopNotifications.h \
    $$PWD/RemovedSharesNotificator.h \
    $$PWD/TransferQuota.h \
    $$PWD/UserAlertTimedClustering.h \
    $$PWD/ScaleFactorManager.h

TRANSLATIONS = \
    gui/translations/MEGASyncStrings_ar.ts \
    gui/translations/MEGASyncStrings_de.ts \
    gui/translations/MEGASyncStrings_en.ts \
    gui/translations/MEGASyncStrings_es.ts \
    gui/translations/MEGASyncStrings_fr.ts \
    gui/translations/MEGASyncStrings_id.ts \
    gui/translations/MEGASyncStrings_it.ts \
    gui/translations/MEGASyncStrings_ja.ts \
    gui/translations/MEGASyncStrings_ko.ts \
    gui/translations/MEGASyncStrings_nl.ts \
    gui/translations/MEGASyncStrings_pl.ts \
    gui/translations/MEGASyncStrings_pt.ts \
    gui/translations/MEGASyncStrings_ro.ts \
    gui/translations/MEGASyncStrings_ru.ts \
    gui/translations/MEGASyncStrings_th.ts \
    gui/translations/MEGASyncStrings_vi.ts \
    gui/translations/MEGASyncStrings_zh_CN.ts \
    gui/translations/MEGASyncStrings_zh_TW.ts

CODECFORTR = UTF8

win32 {
    greaterThan(QT_MAJOR_VERSION, 4) {
        greaterThan(QT_MINOR_VERSION, 1) {
            QT += winextras
        }
    }

    RC_FILE = icon.rc
    QMAKE_LFLAGS += /LARGEADDRESSAWARE
    QMAKE_LFLAGS_WINDOWS += /SUBSYSTEM:WINDOWS,6.01
    QMAKE_LFLAGS_CONSOLE += /SUBSYSTEM:CONSOLE,6.01
    DEFINES += PSAPI_VERSION=1
}


macx {
    QMAKE_CXXFLAGS += -DCRYPTOPP_DISABLE_ASM -D_DARWIN_C_SOURCE
    MAC_ICONS_RESOURCES.files += $$PWD/folder.icns
    MAC_ICONS_RESOURCES.files += $$PWD/folder_yosemite.icns
    MAC_ICONS_RESOURCES.files += $$PWD/appicon32.tiff
    MAC_ICONS_RESOURCES.path = Contents/Resources
    QMAKE_BUNDLE_DATA += MAC_ICONS_RESOURCES
    ICON = app.icns

    QMAKE_INFO_PLIST = Info_MEGA.plist

    QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.12
    QMAKE_CXXFLAGS += -fvisibility=hidden -fvisibility-inlines-hidden
    QMAKE_LFLAGS += -F /System/Library/Frameworks/Security.framework/
    QMAKE_LFLAGS += -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk
}


CONFIG(FULLREQUIREMENTS) {
DEFINES += REQUIRE_HAVE_FFMPEG
DEFINES += REQUIRE_HAVE_LIBUV
DEFINES += REQUIRE_HAVE_LIBRAW
DEFINES += REQUIRE_USE_MEDIAINFO

macx {
}
else:win32 {
}
else:contains(BUILD_ARCH, x86_64) { #Notice this might not work for clang!
}

#DEFINES += REQUIRE_ENABLE_CHAT
#DEFINES += REQUIRE_ENABLE_BACKUPS
#DEFINES += REQUIRE_ENABLE_WEBRTC
#DEFINES += REQUIRE_ENABLE_EVT_TLS
#DEFINES += REQUIRE_USE_PCRE
}

CONFIG(debug) {
    OUTPATH=debug
}
CONFIG(release) {
    OUTPATH=release
}

win32 {
    QMAKE_POST_LINK = $$quote(mt.exe -nologo -manifest $$shell_path($$PWD/../../contrib/cmake/MEGAsync.exe.manifest) -outputresource:$$shell_path($${OUTPATH}/$${TARGET}.exe);1$$escape_expand(\n\t))
}
