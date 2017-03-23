#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include <QSettings>
#include <QStringList>

#include "manager.h"

class Storage : public QObject
{
    Q_OBJECT

public:
    explicit Storage(QObject *parent = 0);
    virtual ~Storage();

    Q_INVOKABLE QStringList loadFilePaths(int key);
    Q_INVOKABLE void saveFilePath(int key, QString filePath);

    enum Files
    {
        VideoFile = mng::VideoFile,
        ObjectFile = mng::ObjectFile
    };

    Q_ENUMS(Files)

private:
    const QString mGroup = "FILES";
    const QStringList mKeyList = {"VIDEOS", "OBJECTS"};
};

#endif // STORAGE_H
