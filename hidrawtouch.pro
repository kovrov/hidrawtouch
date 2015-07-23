TARGET = hidrawtouchplugin

PLUGIN_TYPE = generic
PLUGIN_EXTENDS = -
PLUGIN_CLASS_NAME = HidRawTouchPlugin
load(qt_plugin)

QT += core-private gui-private platformsupport-private

CONFIG += C++11

SOURCES += $$files(src/*.cpp)

HEADERS += $$files(src/*.h)

OTHER_FILES += \
    hidrawtouch.json
