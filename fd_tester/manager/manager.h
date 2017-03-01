#ifndef MANAGER_H
#define MANAGER_H

#include <QStandardPaths>
#include <QString>

class Manager
{
public:
    static Manager* instance();
    static void destroy();

    QString logsDir(){return "logs";}
    QString logFilesExtension(){return ".log";}
    QString logsPath(){return mDataLocation.arg(this->logsDir());}

private:
    Manager();
    ~Manager();
    const QString mDataLocation = QStandardPaths::writableLocation(
                QStandardPaths::DataLocation) + "/%1/";
};

#endif // MANAGER_H
