#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>

#include "application.h"
#include "videodata.h"
#include "refloggenerator.h"
#include "smartvideodata.h"
#include "analyzer.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("AWESOME");
    QCoreApplication::setApplicationName("fdt");

    qmlRegisterType<Analyzer>("awesome.app", 1, 0, "Analyzer");
    qmlRegisterType<VideoData>("awesome.app", 1, 0, "VideoData");
    qmlRegisterType<Application>("awesome.app", 1, 0, "Application");
    qmlRegisterType<SmartVideoData>("awesome.app", 1, 0, "SmartVideoData");
    qmlRegisterType<RefLogGenerator>("awesome.app", 1, 0, "RefLogGenerator");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/demonstration/main.qml")));

    return app.exec();
}
