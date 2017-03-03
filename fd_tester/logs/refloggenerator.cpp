#include "refloggenerator.h"

RefLogGenerator::RefLogGenerator(QObject *parent)
    : QObject(parent)
    , mIsWriting(false)
    , pLogger(nullptr)
{

}

RefLogGenerator::~RefLogGenerator()
{
    pLogger->deleteLater();
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
        emit firstFrameChanged(mFirstFrame);
    }
}

void RefLogGenerator::stopWriting()
{
    pLogger->close();
    mIsWriting = false;
    emit isWritingChanged(mIsWriting);
}

void RefLogGenerator::startWriting()
{
    mIsWriting = true;
    emit isWritingChanged(mIsWriting);

    pLogger->deleteLater();
    pLogger = new Logger(mTitle, alg::None, mFirstFrame, mVideoFile);

    pLogger->writeHeader();
}

void RefLogGenerator::write(/*some_params*/)
{
    QPolygon poly;
    // convert some_params to QPolygon

    pLogger->writeNextBlock(poly, 0);
}
