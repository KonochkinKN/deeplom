#include "qmlmanager.h"

#include <QApplication>
#include <QMessageBox>
#include <QClipboard>
#include <QFile>
#include <QDir>

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

QString QmlManager::cleanLogs()
{
    QString fullPath = pManager->logsPath();
    QDir dir(fullPath);
    QStringList files = dir.entryList(QDir::Files);

    for(int i = 0; i < files.size(); i++)
    {
        QFile file(fullPath + files.at(i));
        if (!file.exists())
            return tr("Log %1 file was not found.").arg(files.at(i));

        if (!file.remove())
            return tr("Log %1 was not removed.").arg(files.at(i));
    }

    return tr("Logs dir are clear");
}

QString QmlManager::cleanInvalidLogs()
{
    QString fullPath = pManager->logsPath();
    QDir dir(fullPath);
    QStringList files = dir.entryList(QDir::Files);
    QStringList logs = pManager->logs();

    foreach (QString log, logs)
        log += pManager->logFilesExtension();

    for(int i = 0; i < files.size(); i++)
    {
        if (!logs.contains(files.at(i)))
        {
            QFile file(fullPath + files.at(i));
            if (!file.exists())
                return tr("Log %1 file was not found.").arg(files.at(i));

            if (!file.remove())
                return tr("Log %1 was not removed.").arg(files.at(i));
        }
    }

    return tr("Invalid logs deleted");
}
