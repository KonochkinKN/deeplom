#include "logger.h"
#include "manager.h"

#include <QDebug>
#include <QDataStream>

Logger::Logger(quint32 algType, quint32 firstFrame,
               QString video, QObject* parent)
    : QObject(parent)
{
    mHeader.algorithmType = algType;
    mHeader.firstFrame = firstFrame;
    mHeader.videoFile = video;
    QString filePath = QString("%1%2_%3%4")
            .arg(Manager::instance()->logsPath())
            .arg(alg::algToString.at(algType))
            .arg(QDateTime::currentDateTime().toString("dd_MM_yyyy_hh:mm:ss"))
            .arg(Manager::instance()->logFilesExtension());

    mFile.setFileName(filePath);
}

Logger::Logger(QString path, QObject *parent)
    : QObject(parent)
{
    mFile.setFileName(path);
}

Logger::~Logger()
{
    this->close();
}

LogHeader Logger::readHeader()
{
    if (mFile.isOpen())
        return LogHeader();

    if (!mFile.open(QIODevice::ReadOnly))
        return LogHeader();

    LogHeader header;
    QDataStream out (&mFile);
    out >> header.algorithmType;
    if (!header.isReference())
        return header;

    out >> header.logDateTime;
    out >> header.firstFrame;
    out >> header.videoFile;

    return header;
}

QPair<QPolygon, qint64> Logger::readNextBlock()
{
    QPair<QPolygon, qint64> result;
    if (!mFile.isOpen())
        return result;

    QDataStream out(&mFile);
    out >> result.first;
    out >> result.second;

    return result;
}

QString Logger::fileAbsPath() const
{
    return mFile.fileName();
}

bool Logger::atEnd() const
{
    return mFile.atEnd();
}

bool Logger::writeHeader()
{
    if (mFile.isOpen())
        return false;

    if (!mFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return false;

    mHeader.logDateTime = QDateTime::currentDateTime();

    QDataStream in(&mFile);

    in << mHeader.algorithmType;
    in << mHeader.logDateTime;
    in << mHeader.firstFrame;
    in << mHeader.videoFile;

    return (in.status() == QDataStream::Ok);
}

bool Logger::writeNextBlock(QPolygon strobe, qint64 delay)
{
    if (!mFile.isOpen())
        return false;

    QDataStream in(&mFile);
    in << strobe;
    in << delay;
    return (in.status() == QDataStream::Ok);
}

void Logger::close()
{
    if (mFile.isOpen())
        mFile.close();
}
