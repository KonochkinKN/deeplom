#ifndef QMLMANAGER_H
#define QMLMANAGER_H

#include <QObject>
#include <QStringList>

#include "manager.h"

class QmlManager : public QObject
{
    Q_OBJECT

public:
    explicit QmlManager(QObject *parent = 0);
    virtual ~QmlManager();

    Q_INVOKABLE void aboutQt();
    Q_INVOKABLE QString mission();
    Q_INVOKABLE QStringList logs();
    Q_INVOKABLE QString logFilesPath();
    Q_INVOKABLE QStringList algorithms();
    Q_INVOKABLE void logsPathToClipBoard();

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

private:
    Manager* pManager;

};

#endif // QMLMANAGER_H
