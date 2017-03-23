#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QCommandLineParser>

#include "application.h"
#include "videodata.h"
#include "refloggenerator.h"
#include "smartvideodata.h"
#include "analyzer.h"
#include "storage.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("fdt");
    QCoreApplication::setOrganizationName("AWESOME");
    QCoreApplication::setOrganizationDomain("AWESOME.KOSTYAN@Ya.com");
    QCoreApplication::setApplicationVersion(QString::number(__version));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(Manager::instance()->mission());
    parser.process(app);

    qmlRegisterType<Storage>("awesome.app.fdt", 1, 0, "Storage");
    qmlRegisterType<Analyzer>("awesome.app.fdt", 1, 0, "Analyzer");
    qmlRegisterType<VideoData>("awesome.app.fdt", 1, 0, "VideoData");
    qmlRegisterType<Application>("awesome.app.fdt", 1, 0, "Application");
    qmlRegisterType<SmartVideoData>("awesome.app.fdt", 1, 0, "SmartVideoData");
    qmlRegisterType<RefLogGenerator>("awesome.app.fdt", 1, 0, "RefLogGenerator");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/demonstration/main.qml")));

    return app.exec();
}
