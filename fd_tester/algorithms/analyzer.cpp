#include "analyzer.h"
#include "manager.h"
#include "logger.h"
#include "logheader.h"
#include "polymath.h"

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

    if (title.isEmpty())
        return;

    QString absPath = Manager::instance()->logsPath() + title +
            Manager::instance()->logFilesExtension();

    QFile file(absPath);

    if(!file.exists())
    {
        emit message(tr("No such reference log"));
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

    if (title.isEmpty())
        return;

    QString absPath = Manager::instance()->logsPath() + title +
            Manager::instance()->logFilesExtension();

    QFile file(absPath);

    if(!file.exists())
    {
        emit message(tr("No such testing log"));
        return;
    }

    mLogForTestFile = absPath;
    mLogForTestTitle = title;

    emit logForTestChanged(mLogForTestTitle);
}

void Analyzer::analyze()
{
    mResult.clear();
    emit resultChanged(mResult);

    // check files
    if (mRefLogFile.isEmpty())
    {
        emit message(tr("No reference log file provided"));
        return;
    }

    if (mLogForTestTitle.isEmpty())
    {
        emit message(tr("No log for test file provided"));
        return;
    }

    // check and read ref log
    Logger* refReader = new Logger(mRefLogFile);
    LogHeader refHeader = refReader->readHeader();

    if (!refHeader.isReference() || !refHeader.isValid())
    {
        emit message(tr("Invalid reference log file provided"));
        delete refReader;
        return;
    }

    if (refReader->atEnd())
    {
        emit message(tr("Empty reference log"));
        delete refReader;
        return;
    }

    quint32 refFrame = refHeader.firstFrame;
    QList<QPair<QPolygonF, qint64>> refData;

    while(!refReader->atEnd())
        refData.append(refReader->readNextBlock());

    refReader->close();
    delete refReader;

    // check and read test log
    Logger* logReader = new Logger(mLogForTestFile);
    LogHeader logHeader = logReader->readHeader();

    if (logHeader.isReference() || !logHeader.isValid())
    {
        emit message(tr("Invalid log for test file provided"));
        delete logReader;
        return;
    }

    if (logReader->atEnd())
    {
        emit message(tr("Empty log for test"));
        delete logReader;
        return;
    }

    quint32 logFrame = logHeader.firstFrame;
    QList<QPair<QPolygonF, qint64>> logData;

    while(!logReader->atEnd())
        logData.append(logReader->readNextBlock());

    logReader->close();
    delete logReader;

    // check data
    if ((refFrame + refData.size() < logFrame) ||
            (logFrame + logData.size() < refFrame))
    {
        emit message(tr("Logs are incomparable"));
        return;
    }

    //=========================================//
    //              COMPARE THEM               //
    //               (some way)                //
    //=========================================//

    mResult.append("100%!!!"); // must be full info
    mResult.append("Но это не точно");
    emit resultChanged(mResult);
}

void Analyzer::countEfficiency()
{
    /* QPolygonF::intersected(QPolygonF& other);
     * Возвращает пересечение полигонов
     * QPolygonF::subtracted(QPolygonF& other);
     * Возвращает разность полигонов
     * + PolyMath
     */
}
