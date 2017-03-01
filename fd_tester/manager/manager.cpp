#include "manager.h"

#include <QDir>
#include <QObject>

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
