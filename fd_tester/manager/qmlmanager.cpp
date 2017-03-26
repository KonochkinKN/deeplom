#include "qmlmanager.h"

#include <QApplication>
#include <QMessageBox>
#include <QClipboard>

QmlManager::QmlManager(QObject *parent)
    : QObject(parent)
    , pManager(Manager::instance())
{

}

QmlManager::~QmlManager()
{
    pManager->destroy();
}

void QmlManager::aboutQt()
{
    return QMessageBox::aboutQt(nullptr);
}

QString QmlManager::mission()
{
    return pManager->mission();
}

QStringList QmlManager::logs()
{
    return pManager->logs();
}

QString QmlManager::logFilesPath()
{
    return pManager->logsPath();
}

QStringList QmlManager::algorithms()
{
    return alg::algToString;
}

void QmlManager::logsPathToClipBoard()
{
    QApplication::clipboard()->setText(this->logFilesPath());
}
