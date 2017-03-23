#include "storage.h"

#include <QFile>
#include <QUrl>

Storage::Storage(QObject *parent) : QObject(parent)
{

}

Storage::~Storage()
{

}

void Storage::saveFilePath(int key, QString fileName)
{
    QSettings settings;
    QStringList oldFiles;

    settings.beginGroup(mGroup);
    oldFiles = settings.value(mKeyList.at(key), QStringList()).toStringList();
    settings.endGroup();

    if(!oldFiles.contains(fileName))
    {
        oldFiles.append(fileName);
        settings.beginGroup(mGroup);
        settings.setValue(mKeyList.at(key), oldFiles);
        settings.endGroup();
    }
}

QStringList Storage::loadFilePaths(int key)
{
    QStringList files;
    QSettings settings;
    QStringList realFiles;

    settings.beginGroup(mGroup);
    files = settings.value(mKeyList.at(key), "").toStringList();
    settings.endGroup();

    // check existence
    foreach(QString fileName, files)
    {
        if(QFile(QUrl(fileName).path()).exists())
            realFiles.append(fileName);
    }

    return realFiles;
}
