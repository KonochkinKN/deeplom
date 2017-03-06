#include "application.h"

Application::Application(QObject *parent)
    : QObject(parent)
    , pManager(Manager::instance())
{

}

Application::~Application()
{
    pManager->destroy();
}
