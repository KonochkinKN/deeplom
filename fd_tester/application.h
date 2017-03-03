#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QStringList>

#include "manager.h"

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QObject *parent = 0);
    virtual ~Application();

    Q_PROPERTY(QStringList logs READ logs NOTIFY logsChanged)

    Q_INVOKABLE QStringList logs(){return Manager::instance()->logs();}

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

signals:
    void logsChanged(QStringList logList);

};

#endif // APPLICATION_H
