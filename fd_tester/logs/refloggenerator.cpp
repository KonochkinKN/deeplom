#include "refloggenerator.h"

#include <cmath>

#include <QDebug>

RefLogGenerator::RefLogGenerator(QObject *parent)
    : QObject(parent)
    , mIsWriting(false)
    , pLogger(nullptr)
{

}

RefLogGenerator::~RefLogGenerator()
{
    delete pLogger;
}

void RefLogGenerator::setTitle(QString data)
{
    if(!mIsWriting)
    {
        mTitle = data;
        emit titleChanged(mTitle);
    }
}

void RefLogGenerator::setVideoFile(QString data)
{
    if(!mIsWriting)
    {
        mVideoFile = data;
        emit videoFileChanged(mVideoFile);
    }
}

void RefLogGenerator::setFirstFrame(quint32 data)
{
    if(!mIsWriting)
    {
        mFirstFrame = data;
        mCurrentFrame = data;
        emit firstFrameChanged(mFirstFrame);
    }
}

void RefLogGenerator::setCurrentFrame(quint32 data)
{
    mCurrentFrame = data;
    emit currentFrameChanged(mCurrentFrame);
}

void RefLogGenerator::stopWriting()
{
    if (!mIsWriting)
        return;

    this->saveLog();
    mIsWriting = false;
    emit isWritingChanged(mIsWriting);
}

void RefLogGenerator::startWriting()
{
    if (!this->isValid())
        return;

    mIsWriting = true;
    emit isWritingChanged(mIsWriting);
}

bool RefLogGenerator::isValid()
{
    QString errMsg = "";

    if (mIsWriting)
        errMsg += "Writing is already started\n";

    if (mTitle.isEmpty())
        errMsg += "Title is not provided\n";

    if (mVideoFile.isEmpty())
        errMsg += "Video file is not provided\n";

    if (!errMsg.isEmpty())
        emit message("Some errors occured:\n" + errMsg);

   return errMsg.isEmpty();
}

void RefLogGenerator::saveStrobe(QRectF strobe, int angle)
{
    if (mCurrentFrame > mFirstFrame + mDataList.size())
    {
        emit message("Error: There was a break in the record");
        return;
    }

    QPolygonF resPoly;
    float x, y;
    float sin = std::sin(angle * M_PIl / 180);
    float cos = std::cos(angle * M_PIl / 180);
    // top-left
    x = strobe.x();
    y = strobe.y();
    resPoly.append(QPointF(x, y));
    // top-right
    x += strobe.width()*cos;
    y += strobe.width()*sin;
    resPoly.append(QPointF(x, y));
    // bottom-right
    x -= strobe.height()*sin;
    y += strobe.height()*cos;
    resPoly.append(QPointF(x, y));
    // bottom-left
    x -= strobe.width()*cos;
    y -= strobe.width()*sin;
    resPoly.append(QPointF(x, y));

    if (mCurrentFrame == mFirstFrame + mDataList.size())
    {
        mDataList.append(resPoly);
    }
    else if (mCurrentFrame < mFirstFrame)
    {
        QList <QPolygonF> newList;
        newList.append(resPoly);
        for (uint i = 1; i < mFirstFrame - mCurrentFrame; i++)
            newList.append(QPolygonF());

        newList.append(mDataList);
        mDataList = newList;
        mFirstFrame = mCurrentFrame;
    }
    else
        mDataList[mCurrentFrame-mFirstFrame] = resPoly;
}

void RefLogGenerator::saveLog()
{
    delete pLogger;
    pLogger = new Logger(mTitle, alg::None, mFirstFrame, mVideoFile);

    if (!pLogger->writeHeader())
    {
        emit message("Error: Can't log file header");
        pLogger->close();
        return;
    }

    foreach (QPolygonF poly, mDataList)
    {
        if (!pLogger->writeNextBlock(poly, 0))
        {
            emit message("Error: Can't log data");
            pLogger->close();
            return;
        }
    }

    emit message(QString("File %1 saved succesfully!").
                 arg(pLogger->fileAbsPath()));

    pLogger->close();
}
