#-------------------------------------------------
#
# Project created by QtCreator 2018-09-10T09:34:14
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Factory_test
TEMPLATE = app


SOURCES += main.cpp\
    core/func/BrightTest.cpp \
    core/func/CamaraTest.cpp \
    core/func/EdidTest.cpp \
    core/func/FanTest.cpp \
    core/func/FuncBase.cpp \
    core/func/FuncTest.cpp \
    core/func/HddTest.cpp \
    core/func/MemTest.cpp \
    core/func/NetTest.cpp \
    core/func/NextProcess.cpp \
    core/func/SoundTest.cpp \
    core/func/StressTest.cpp \
    core/func/UploadMes.cpp \
    core/func/UsbTest.cpp \
    core/func/WifiTest.cpp \
    core/ui/UiHandle.cpp \
    utils/fac_log.cpp \
    utils/fac_utils.cpp \
    core/ui/DisplayTestWindow.cpp \
    core/ui/MainTestWindow.cpp \
    core/ui/StressTestWindow.cpp \
    core/ui/messageform.cpp \
    core/control/Control.cpp \
    core/ui/VideoTestThread.cpp \
    core/ui/ImageTestThread.cpp

HEADERS  += \
    inc/Control.h \
    inc/EdidTest.h \
    inc/fac_log.h \
    inc/fac_utils.h \
    inc/FuncBase.h \
    inc/FuncTest.h \
    inc/NetTest.h \
    inc/SoundTest.h \
    inc/ui.h \
    inc/UploadMes.h \
    inc/WifiTest.h \
    inc/UiHandle.h \
    inc/messageform.h \
    inc/Control.h \
    inc/VideoTestThread.h \
    inc/ImageTestThread.h

INCLUDEPATH += /usr/local/include/

LIBS += -L/usr/local/include \
        -lavcodec \
        -lavdevice \
        -lavfilter \
        -lavformat \
        -lavutil \
        -lswresample \
        -lswscale

LIBS += -lftp -ludev -lasound
