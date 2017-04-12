#ifndef MANAGER_H
#define MANAGER_H

#include <QStandardPaths>
#include <QStringList>
#include <QString>

namespace alg
{
enum Algorithms
{
    None,
    ORB,
    ORB2,
    SIFT,
    SURF,
    BRISK
};
const QStringList algToString({"None", "ORB", "ORB2",
                               "SIFT", "SURF", "BRISK"});
}

namespace mng
{
enum FileTypes
{
    VideoFile,
    ObjectFile
};
}

class Manager
{
public:
    static Manager* instance();
    static void destroy();

    QString mission();
    QString consoleMission();
    QString logsDir(){return "logs";}
    QString dataDir(){return "data";}
    QString logFilesExtension(){return ".log";}
    QString dataFilesExtension(){return ".txt";}
    QString logsPath(){return mDataLocation.arg(this->logsDir());}
    QString dataPath(){return mDataLocation.arg(this->dataDir());}
    QStringList logs();

private:
    Manager();
    ~Manager();
    const QString mDataLocation = QStandardPaths::writableLocation(
                QStandardPaths::DataLocation) + "/%1/";
};

#endif // MANAGER_H
