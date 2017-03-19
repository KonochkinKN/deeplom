#include "analyzer.h"
#include "manager.h"
#include "logger.h"
#include "logheader.h"

#include <QFile>

Analyzer::Analyzer(QObject *parent)
    : QObject(parent)
{

}

Analyzer::~Analyzer()
{

}

void Analyzer::setRefLog(QString title)
{
    mRefLogFile = "";
    mRefLogTitle = "";

    QString absPath = Manager::instance()->logsPath() + title +
            Manager::instance()->logFilesExtension();

    QFile file(absPath);

    if(!file.exists())
    {
        emit message("No such reference log");
        return;
    }

    mRefLogFile = absPath;
    mRefLogTitle = title;

    emit refLogChanged(mRefLogTitle);
}

void Analyzer::setLogForTest(QString title)
{
    mLogForTestFile = "";
    mLogForTestTitle = "";

    QString absPath = Manager::instance()->logsPath() + title +
            Manager::instance()->logFilesExtension();

    QFile file(absPath);

    if(!file.exists())
    {
        emit message("No such reference log");
        return;
    }

    mLogForTestFile = absPath;
    mLogForTestTitle = title;

    emit logForTestChanged(mLogForTestTitle);
}

void Analyzer::analyze()
{
    mResult = "";
    emit resultChanged(mResult);

    // check files
    if(mRefLogFile.isEmpty())
    {
        emit message("No reference log file provided");
        return;
    }

    if(mLogForTestTitle.isEmpty())
    {
        emit message("No log fr test file provided");
        return;
    }

    Logger* refReader = new Logger(mRefLogFile);
    Logger* logReader = new Logger(mLogForTestFile);

    LogHeader refHeader = refReader->readHeader();
    LogHeader logHeader = logReader->readHeader();

    // check headers
    if(!refHeader.isReference() || !refHeader.isValid())
    {
        emit message("Invalid reference log file provided");
        refReader->close();
        logReader->close();
        refReader->deleteLater();
        logReader->deleteLater();
        return;
    }

    if(logHeader.isReference() || !logHeader.isValid())
    {
        emit message("Invalid log for test file provided");
        refReader->close();
        logReader->close();
        refReader->deleteLater();
        logReader->deleteLater();
        return;
    }

    quint32 refFrame = refHeader.firstFrame;
    quint32 logFrame = logHeader.firstFrame;
    QList<QPair<QPolygonF, qint64>> refData, logData;

    // check data
    while(!refReader->atEnd())
        refData.append(refReader->readNextBlock());

    refReader->close();

    if(refData.isEmpty())
    {
        emit message("Empty reference log");
        logReader->close();
        refReader->deleteLater();
        logReader->deleteLater();
        return;
    }

    if(refFrame + refData.size() < logFrame)
    {
        emit message("Logs are incomparable");
        logReader->close();
        refReader->deleteLater();
        logReader->deleteLater();
        return;
    }

    while(!logReader->atEnd())
        logData.append(logReader->readNextBlock());

    logReader->close();

    if(logData.isEmpty())
    {
        emit message("Empty log for test");
        refReader->deleteLater();
        logReader->deleteLater();
        return;
    }

    if(logFrame + logData.size() < refFrame)
    {
        emit message("Logs are incomparable");
        refReader->deleteLater();
        logReader->deleteLater();
        return;
    }

    //=========================================//
    //              COMPARE THEM               //
    //               (some way)                //
    //=========================================//

    mResult = "100%!!!"; // must be full info
    emit resultChanged(mResult);

    refReader->deleteLater();
    logReader->deleteLater();
}
