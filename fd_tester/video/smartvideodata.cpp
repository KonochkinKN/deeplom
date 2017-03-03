#include "smartvideodata.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

SmartVideoData::SmartVideoData(QObject *parent)
    : VideoData(parent)
    , mIsDetecting(false)
    , pSearcher(new Searcher())
    , pThread(new QThread())
{
    pSearcher->moveToThread(pThread);
    connect(pSearcher, &Searcher::error, this,
            &SmartVideoData::message, Qt::DirectConnection);

    connect(pSearcher, &Searcher::detected, this,
            &SmartVideoData::detected, Qt::DirectConnection);
}

void SmartVideoData::startDetecting()
{
    if(!this->isDetecting())
    {
        connect(this, &SmartVideoData::positionChanged, this,
                &SmartVideoData::detect, Qt::DirectConnection);
        mIsDetecting = true;
        emit isDetectingChanged(mIsDetecting);
    }
}

void SmartVideoData::stopDetecting()
{
    if(this->isDetecting())
    {
        disconnect(this, &SmartVideoData::positionChanged,
                   this, &SmartVideoData::detect);
        mIsDetecting = false;
        emit isDetectingChanged(mIsDetecting);
    }
}

void SmartVideoData::setTemplateImage(QString imgPath)
{
    if(!pSearcher->isDetecting())
    {
        mTemplate = imgPath;
        pSearcher->setTemplate(imgPath);
        emit templateImageChanged(mTempLate);
    }
}

void SmartVideoData::detect()
{
    cv::Mat frame;
    this->pCapture->read(frame);
    pSearcher->setInputImage(frame);
    pSearcher->startDetecting();
}
