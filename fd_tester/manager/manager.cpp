#include "manager.h"
#include "logger.h"

#include <QDir>
#include <QObject>
#include <QStringList>

// logsPath + logs.at(*) + logExt = logAbsPath
// logsPath + logTitle_logDateTime + logExt = logAbsPath

static Manager *manager = nullptr;

Manager* Manager::instance()
{
    if (!manager)
        manager = new Manager();

    return manager;
}

void Manager::destroy()
{
    delete manager;
    manager = nullptr;
}

Manager::Manager()
{
    QDir dir;

    if (!dir.exists(this->logsPath()))
        dir.mkpath(this->logsPath());
}

Manager::~Manager()
{

}

QStringList Manager::logs()
{
    QStringList allLogs;
    QDir dir(Manager::instance()->logsPath());

    QStringList filters;
    filters << QString("*" + Manager::instance()->logFilesExtension());

    QStringList entries = dir.entryList(QDir::Files);

    foreach (QString entry, entries)
    {
       Logger initializers(dir.absolutePath() + "/" + entry);
       LogHeader header = initializers.readHeader();
       if (header.isValid())
       {
           allLogs.append(header.title + "_" + header.
                          logDateTime.toString("dd_MM_yyyy_hh:mm:ss"));
       }
       initializers.close();
    }

    return allLogs;
}
