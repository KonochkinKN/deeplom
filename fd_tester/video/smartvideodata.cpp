#include "smartvideodata.h"

#include <QAbstractVideoSurface>
#include <QPixmap>
#include <QTimer>
#include <QDebug>
#include <QImage>
#include <QUrl>
#include <QDir>

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
            &SmartVideoData::onDetected, Qt::DirectConnection);
}

SmartVideoData::~SmartVideoData()
{
    if (pThread->isRunning())
    {
        pThread->quit();
        pThread->wait();
    }
    pThread->deleteLater();
    pSearcher->deleteLater();
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
        emit templateImageChanged(mTemplate);
    }
}

void SmartVideoData::detect()
{
    cv::Mat frame;
    this->pCapture->read(frame);
    pSearcher->setInputImage(frame);
    pSearcher->startDetecting();
}

void SmartVideoData::onDetected()
{
    if( !this->pSurface )
        return;

    cv::Mat frame = pSearcher->getResult().first;

    if(frame.empty())
        return;

    cv::cvtColor(frame, frame, CV_RGB2RGBA, 4);

    QImage frameImg((const uchar *) frame.data, frame.cols, frame.rows,
                    frame.step, QImage::Format_ARGB32);
    frameImg.bits(); // enforce deep copy

    QVideoFrame vFrame(frameImg);

    QVideoFrame::PixelFormat pixelFormat =
            QVideoFrame::pixelFormatFromImageFormat( frameImg.format() );

    this->closeSurface();
    this->mFormat = QVideoSurfaceFormat(
                QPixmap::fromImage(frameImg).size(), pixelFormat);
    this->pSurface->start( this->mFormat );

    this->pSurface->present( vFrame );
    emit positionChanged(this->mFrame);
}
