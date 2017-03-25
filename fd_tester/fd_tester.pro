QT += qml quick multimedia widgets opengl testlib gui

DEFINES += "__print=\'qDebug()<<__FILE__<<__LINE__<<Q_FUNC_INFO\'" \
    QT_DEPRECATED_WARNINGS \
    "__version=1"

CONFIG += c++11 exception

LIBS += -ldl \
        -lopencv_core \
        -lopencv_highgui \
        -lopencv_imgproc \
        -lopencv_features2d \
        -lopencv_calib3d \
        -lopencv_flann \
        -lopencv_nonfree

ICON = hicolor/32x32/pisos.png

INCLUDEPATH += \
    manager \
    video \
    logs \
    demonstration \
    demonstration/windows \
    demonstration/views \
    algorithms

SOURCES += main.cpp \
    manager/manager.cpp \
    video/videodata.cpp \
    logs/logger.cpp \
    logs/refloggenerator.cpp \
    video/smartvideodata.cpp \
    manager/application.cpp \
    algorithms/analyzer.cpp \
    algorithms/searcher.cpp \
    manager/storage.cpp

HEADERS += \
    manager/manager.h \
    video/videodata.h \
    logs/logger.h \
    logs/logheader.h \
    logs/refloggenerator.h \
    video/smartvideodata.h \
    manager/application.h \
    algorithms/analyzer.h \
    algorithms/searcher.h \
    manager/storage.h

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
    demonstration/windows/RefLogGeneratorWindow.qml \
    demonstration/components/StrobeDrawComponent.qml \
    demonstration/views/AlgorithmRunnerView.qml \
    demonstration/windows/AlgorithmRunnerWindow.qml \
    demonstration/components/SmartVideoComponent.qml \
    demonstration/components/FileSelectionComponent.qml
