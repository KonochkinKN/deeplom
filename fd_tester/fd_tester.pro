QT += qml quick

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

SOURCES += main.cpp \
    manager/manager.cpp \
    searcher/searcher.cpp

HEADERS += \
    manager/manager.h \
    searcher/searcher.h

RESOURCES += qml.qrc
