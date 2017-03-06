QT += qml quick multimedia widgets opengl testlib gui

DEFINES += "__print=\'qDebug()<<__FILE__<<__LINE__<<Q_FUNC_INFO\'" \
    QT_DEPRECATED_WARNINGS

CONFIG += c++11

LIBS += -ldl \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_features2d \
        -lopencv_calib3d \
        -lopencv_flann \
        -lopencv_nonfree

INCLUDEPATH += \
    manager \
    searcher \
    video \
    logs \
    effeciency \
    demonstration \
    demonstration/windows \
    demonstration/views

SOURCES += main.cpp \
    manager/manager.cpp \
    searcher/searcher.cpp \
    video/videodata.cpp \
    logs/logger.cpp \
    effeciency/analyzer.cpp \
    application.cpp \
    logs/refloggenerator.cpp \
    video/smartvideodata.cpp

HEADERS += \
    manager/manager.h \
    searcher/searcher.h \
    video/videodata.h \
    logs/logger.h \
    logs/logheader.h \
    effeciency/analyzer.h \
    application.h \
    logs/refloggenerator.h \
    video/smartvideodata.h

RESOURCES += qml.qrc

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/release/ -lpulse
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/debug/ -lpulse
else:unix: LIBS += -L$$PWD/../../../../../usr/lib/x86_64-linux-gnu/ -lpulse

INCLUDEPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu
DEPENDPATH += $$PWD/../../../../../usr/lib/x86_64-linux-gnu

DISTFILES += \
    demonstration/windows/MainWindow.qml \
    demonstration/main.qml \
    demonstration/components/VideoComponent.qml \
    demonstration/components/VideoControlBar.qml \
    demonstration/views/RefLogGeneratorView.qml \
    demonstration/windows/RefLogGeneratorWindow.qml
