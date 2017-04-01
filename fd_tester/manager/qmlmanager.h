#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QStringList>
#include <QQmlEngine>
#include <QJSEngine>

#include "manager.h"

class QmlManager : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(QmlManager)

public:
    explicit QmlManager(QObject *parent = 0);
    virtual ~QmlManager();

    Q_INVOKABLE void aboutQt();
    Q_INVOKABLE QString mission();
    Q_INVOKABLE QStringList logs();
    Q_INVOKABLE QString logFilesPath();
    Q_INVOKABLE QStringList algorithms();
    Q_INVOKABLE void logsPathToClipBoard();

    Q_INVOKABLE QString cleanLogs();
    Q_INVOKABLE QString cleanInvalidLogs();

    Q_INVOKABLE QStringList refLogsByVideo(QString video);
    Q_INVOKABLE QStringList testLogsByVideo(QString video);

    enum Algs
    {
        None    = alg::None,
        ORB     = alg::ORB,
        ORB2    = alg::ORB2,
        SIFT    = alg::SIFT,
        SURF    = alg::SURF,
        BRISK   = alg::BRISK
    };

    Q_ENUMS(Algs)

    static QObject *qmlInstance(QQmlEngine *engine, QJSEngine *scriptEngine)
    {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new QmlManager();
    }

private:
    Manager* pManager;

};

#endif // QMLMANAGER_H
