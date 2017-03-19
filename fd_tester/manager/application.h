#ifndef APPLICATION_H
#define APPLICATION_H

#include <QObject>
#include <QStringList>
#include <QMessageBox>

#include "manager.h"

class Application : public QObject
{
    Q_OBJECT

public:
    explicit Application(QObject *parent = 0);
    virtual ~Application();

    Q_INVOKABLE QStringList logs(){return Manager::instance()->logs();}
    Q_INVOKABLE QString logFilesPath(){return pManager->logsPath();}
    Q_INVOKABLE QStringList algorithms(){return alg::algToString;}
    Q_INVOKABLE void aboutQt(){return QMessageBox::aboutQt(nullptr);}

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

#endif // APPLICATION_H
