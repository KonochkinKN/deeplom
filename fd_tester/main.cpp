#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QCommandLineParser>
#include <QIcon>
#include <QDebug>

#include "qmlmanager.h"
#include "videodata.h"
#include "refloggenerator.h"
#include "smartvideodata.h"
#include "analyzer.h"
#include "storage.h"
#include "comparevideodata.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setApplicationName("fdt");
    QCoreApplication::setOrganizationName("AWESOME");
    QCoreApplication::setOrganizationDomain("awesome.kostyan@gmail.com");
    QCoreApplication::setApplicationVersion(QString::number(__version));
    app.setWindowIcon(QIcon(":/design/icon.png"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addVersionOption();
    parser.setApplicationDescription(Manager::instance()->consoleMission());
    parser.process(app);

    qmlRegisterType<Storage>("awesome.app.fdt", 1, 0, "Storage");
    qmlRegisterType<Analyzer>("awesome.app.fdt", 1, 0, "Analyzer");
    qmlRegisterType<VideoData>("awesome.app.fdt", 1, 0, "VideoData");
    qmlRegisterType<QmlManager>("awesome.app.fdt", 1, 0, "QmlManager");
    qmlRegisterType<SmartVideoData>("awesome.app.fdt", 1, 0, "SmartVideoData");
    qmlRegisterType<RefLogGenerator>("awesome.app.fdt", 1, 0, "RefLogGenerator");
    qmlRegisterType<CompareVideoData>("awesome.app.fdt", 1, 0, "CompareVideoData");

    QQmlApplicationEngine engine;
    engine.load(QUrl(QLatin1String("qrc:/demonstration/main.qml")));

    return app.exec();
}
