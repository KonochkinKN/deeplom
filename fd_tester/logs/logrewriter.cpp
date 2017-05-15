#include "logrewriter.h"
#include "logheader.h"
#include "logger.h"
#include "manager.h"

#include <QUrl>
#include <QDebug>

LogRewriter::LogRewriter(QObject *parent) : QObject(parent)
{

}

LogRewriter::~LogRewriter()
{

}

bool LogRewriter::changeVideoFile(QString logFile, QString videoFile)
{
    QFile video(QUrl(videoFile).path());
    if (!video.exists())
    {
        __print << tr("Invalid video file");
        return false;
    }

    Logger* reader = new Logger(Manager::instance()->logsPath() + logFile +
                                Manager::instance()->logFilesExtension());
    LogHeader header = reader->readHeader();

    if (!header.isValid())
    {
        __print << tr("Invalid log file name");
        return false;
    }

    if (reader->atEnd())
    {
        __print << tr("Empry log file");
        return false;
    }

    Logger* writer = new Logger(header.title, header.algorithmType,
                                header.firstFrame, QUrl(videoFile).toString());
    if (!writer->writeHeader())
    {
        __print << tr("Log creating error");
        return false;
    }

    while (!reader->atEnd())
    {
        QPair <QPolygonF, quint64> block = reader->readNextBlock();
        if (!writer->writeNextBlock(block.first, block.second))
        {
            __print << tr("Log writing error");
            return false;
        }
    }

    reader->close();
    writer->close();

    delete reader;
    delete writer;

    return true;
}
