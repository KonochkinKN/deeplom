#include "analyzer.h"
#include "manager.h"
#include "logger.h"
#include "logheader.h"
#include "polymath.h"

#include <QFile>
#include <QDebug>

Analyzer::Analyzer(QObject *parent)
    : QObject(parent)
    , mRefFrame(0)
    , mTestFrame(0)
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

    /*// check files
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
    delete logReader;*/

    if (!this->readRefLog() || !this->readTestLog())
        return;

    // check data
    if ((mRefFrame + mRefData.size() < mTestFrame) ||
            (mTestFrame + mTestData.size() < mRefFrame))
    {
        emit message(tr("Logs are incomparable"));
        return;
    }

    this->countEfficiency();
}

bool Analyzer::readRefLog()
{
    mRefFrame = 0;
    mRefData.clear();

    // check file
    if (mRefLogFile.isEmpty())
    {
        emit message(tr("No reference log file provided"));
        return false;
    }

    // check and read ref log
    Logger* refReader = new Logger(mRefLogFile);
    LogHeader refHeader = refReader->readHeader();

    if (!refHeader.isReference() || !refHeader.isValid())
    {
        emit message(tr("Invalid reference log file provided"));
        delete refReader;
        return false;
    }

    if (refReader->atEnd())
    {
        emit message(tr("Empty reference log"));
        delete refReader;
        return false;
    }

    mRefFrame = refHeader.firstFrame;
    while(!refReader->atEnd())
        mRefData.append(refReader->readNextBlock());

    refReader->close();
    delete refReader;
    return true;
}

bool Analyzer::readTestLog()
{
    mTestFrame = 0;
    mTestData.clear();

    // check file
    if (mLogForTestTitle.isEmpty())
    {
        emit message(tr("No log for test file provided"));
        return false;
    }

    // check and read test log
    Logger* logReader = new Logger(mLogForTestFile);
    LogHeader logHeader = logReader->readHeader();

    if (logHeader.isReference() || !logHeader.isValid())
    {
        emit message(tr("Invalid log for test file provided"));
        delete logReader;
        return false;
    }

    if (logReader->atEnd())
    {
        emit message(tr("Empty log for test"));
        delete logReader;
        return false;
    }

    mTestFrame = logHeader.firstFrame;
    while(!logReader->atEnd())
        mTestData.append(logReader->readNextBlock());

    logReader->close();
    delete logReader;
    return true;
}

void Analyzer::countEfficiency()
{
    double eff = 0.0;   // summary efficiency
    double avgRA = 0.0; // average reference strobe area
    double avgTA = 0.0; // average test strobe area
    double avgIA = 0.0; // average intersection strobe area
    double avgXD = 0.0; // average x-axis center difference
    double avgYD = 0.0; // average y-axis center difference
    double avgAD = 0.0; // average absolut center difference

    quint32 firstFrame = std::max(mRefFrame, mTestFrame);
    quint32 lastFrame = std::min(mTestData.size() + mTestFrame,
                                 mRefData.size() + mRefFrame);

    for (quint32 i = firstFrame; i < lastFrame; i++)
    {
        QPolygonF refR = mRefData.at(i - mRefFrame).first;
        QPolygonF testR = mTestData.at(i - mTestFrame).first;
        QPolygonF insR = refR.intersected(testR);
        QPointF refC = PolyMath::instance()->massCenter(refR);
        QPointF testC = PolyMath::instance()->massCenter(testR);
        QPointF diffC = refC - testC;
        double refA = PolyMath::instance()->area(refR);
        double testA = PolyMath::instance()->area(testR);
        double insA = PolyMath::instance()->area(insR);
        // area
        avgRA += refA;
        avgTA += testA;
        avgIA += insA;
        // center
        avgXD += diffC.x();
        avgYD += diffC.y();
        avgAD += std::sqrt(QPointF::dotProduct(diffC, diffC));
        // summary
        eff += insA / refA;
    }

    quint32 n = lastFrame - firstFrame;
    eff /= n;
    avgRA /= n;
    avgTA /= n;
    avgIA /= n;
    avgXD /= n;
    avgYD /= n;
    avgAD /= n;

    mResult.append(tr("Efficiency = %1\%").arg(eff*100));
    mResult.append(tr("Area charactersistics:"));
    mResult.append(tr("    Average reference strobe area: %1").arg(avgRA));
    mResult.append(tr("    Average test strobe area: %1").arg(avgTA));
    mResult.append(tr("    Average intersection strobe area: %1").arg(avgIA));
    mResult.append(tr("Centers charactersistics:"));
    mResult.append(tr("    Average x-axis center difference: %1").arg(avgXD));
    mResult.append(tr("    Average y-axis center difference: %1").arg(avgYD));
    mResult.append(tr("    Average average absolut center difference: %1").arg(avgAD));
    emit resultChanged(mResult);

}
