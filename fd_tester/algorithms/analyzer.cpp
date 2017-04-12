#include "analyzer.h"
#include "manager.h"
#include "logger.h"
#include "logheader.h"
#include "polymath.h"

#include <QFile>
#include <QDebug>
#include <QJsonDocument>

Analyzer::Analyzer(QObject *parent)
    : QObject(parent)
    , mRefFrame(0)
    , mTestFrame(0)
    , mCurrentAlgorithm(alg::None)
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

void Analyzer::setTestLog(QString title)
{
    mTestLogFile.clear();
    mTestLogTitle.clear();
    mCurrentVideo.clear();
    mCurrentAlgorithm = alg::None;

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

    mTestLogFile = absPath;
    mTestLogTitle = title;

    emit testLogChanged(mTestLogTitle);
}

void Analyzer::analyze()
{
    mResult.clear();
    emit resultChanged(mResult);

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
    if (mTestLogTitle.isEmpty())
    {
        emit message(tr("No log for test file provided"));
        return false;
    }

    // check and read test log
    Logger* logReader = new Logger(mTestLogFile);
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
    mCurrentVideo = logHeader.videoFile;
    mCurrentAlgorithm = logHeader.algorithmType;
    while(!logReader->atEnd())
        mTestData.append(logReader->readNextBlock());

    logReader->close();
    delete logReader;
    return true;
}

void Analyzer::countEfficiency()
{
    Data data;
    data.setNull();

    quint32 firstFrame = std::max(mRefFrame, mTestFrame);
    quint32 lastFrame = std::min(mTestData.size() + mTestFrame,
                                 mRefData.size() + mRefFrame);

    for (quint32 i = firstFrame; i < lastFrame; i++)
    {
        data.avgDT += mTestData.at(i - mTestFrame).second;
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
        data.avgRA += refA;
        data.avgTA += testA;
        data.avgIA += insA;
        // center
        data.avgXD += diffC.x();
        data.avgYD += diffC.y();
        data.avgAD += std::sqrt(QPointF::dotProduct(diffC, diffC));
        // summary
        if (!testA && !refA)
        {
            data.eff += 1.0;
        }
        else if (refA  > 0)
        {
            data.eff += insA / refA;
        }

        if (!insA)
            data.detErrors++;
    }

    data.totFrames = lastFrame - firstFrame;
    data.average();

    this->setResult(data);
    this->saveResult(data);
}

void Analyzer::setResult(const Data &data)
{
    mResult.clear();
    mResult.append(tr("Efficiency = %1\%").arg(data.eff*100));
    mResult.append(tr("Average detection time: %1").arg(data.avgDT));
    mResult.append(tr("Total frames processed: %1").arg(data.totFrames));
    mResult.append(tr("Detection errors: %1").arg(data.detErrors));
    mResult.append(tr("Area charactersistics:"));
    mResult.append(tr("    Average reference strobe area: %1").arg(data.avgRA));
    mResult.append(tr("    Average test strobe area: %1").arg(data.avgTA));
    mResult.append(tr("    Average intersection strobe area: %1").arg(data.avgIA));
    mResult.append(tr("Centers charactersistics:"));
    mResult.append(tr("    Average x-axis center difference: %1").arg(data.avgXD));
    mResult.append(tr("    Average y-axis center difference: %1").arg(data.avgYD));
    mResult.append(tr("    Average average absolute center difference: %1").arg(data.avgAD));
    emit resultChanged(mResult);
}

void Analyzer::saveResult(const Data &data)
{
    QString path = Manager::instance()->dataPath() +
            alg::algToString.at(this->mCurrentAlgorithm) + "_" +
            QDateTime::currentDateTime().toString("dd.MM.yyyy_hh:mm:ss") +
            Manager::instance()->dataFilesExtension();

    QFile file(path);

    if (file.isOpen())
        return;

    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    QJsonDocument doc(this->parseData(data));
    file.write(doc.toJson());

    file.close();
}

QJsonObject Analyzer::parseData(const Data &data)
{
    QJsonObject jsonData
    {
        {"Algorithm", alg::algToString.at(this->mCurrentAlgorithm)},
        {"Date", QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss")},
        {"Video", this->mCurrentVideo},
        {"Efficiency", data.eff},
        {"Avg det time", data.avgDT},
        {"Fames", data.totFrames},
        {"Errors", data.detErrors},
        {"Area", QJsonObject(
            {{"Avg ref area", data.avgRA},
             {"Avg test area", data.avgTA},
             {"Avg ins area", data.avgIA}})},
        {"Centers", QJsonObject(
            {{"Avg x diff", data.avgXD},
             {"Avg y diff", data.avgYD},
             {"Avg abs diff", data.avgAD}})}
    };

    return jsonData;
}
