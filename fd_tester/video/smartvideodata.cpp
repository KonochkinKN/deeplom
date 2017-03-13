#include "smartvideodata.h"
#include "smartvideodata_p.h"

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
    , d_ptr(new SmartVideoDataPrivate)
{
    Q_D(SmartVideoData);
    d->q_ptr = this;

    connect(d, &SmartVideoDataPrivate::detected,
            this, &SmartVideoData::onDetected);
}

SmartVideoData::~SmartVideoData()
{
    d_ptr->deleteLater();
}

int SmartVideoData::algType()
{
    Q_D(SmartVideoData);
    return d->mAlgType;
}

bool SmartVideoData::isDetecting()
{
    Q_D(SmartVideoData);
    return d->mIsDetecting;
}

QString SmartVideoData::templateImage()
{
    Q_D(SmartVideoData);
    return d->mTemplate;
}

qint64 SmartVideoData::iterationTime()
{
    Q_D(SmartVideoData);
    return d->mIterationTime;
}

void SmartVideoData::startDetecting()
{
    Q_D(SmartVideoData);
    if(!d->pThread->isRunning())
        d->pThread->start();
//    if(!this->isDetecting())
//    {
//        connect(this, &SmartVideoData::positionChanged, this,
//                &SmartVideoData::detect, Qt::DirectConnection);
//        mIsDetecting = true;
//        emit isDetectingChanged(mIsDetecting);
//    }
}

void SmartVideoData::stopDetecting()
{
    Q_D(SmartVideoData);
    if(d->pThread->isRunning())
        d->pThread->requestInterruption();
//    if(this->isDetecting())
//    {
//        disconnect(this, &SmartVideoData::positionChanged,
//                   this, &SmartVideoData::detect);
//        mIsDetecting = false;
//        emit isDetectingChanged(mIsDetecting);
//    }
}

void SmartVideoData::setTemplateImage(QString imgPath)
{
    Q_D(SmartVideoData);
    d->mTemplate = imgPath;
    emit templateImageChanged(d->mTemplate);
}

void SmartVideoData::onDetected()
{
    Q_D(SmartVideoData);

    if( !this->pSurface )
        return;

    cv::Mat frame = d->pSearcher->getResult().first;

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

    this->setPosition(mFrame+1);
    d->mIterationTime = d->pSearcher->getElapsedTime();
    emit iterationTimeChanged(d->mIterationTime);
}

void SmartVideoData::setAlgType(int type)
{
    Q_D(SmartVideoData);
    if(type != alg::None && type < alg::algToString.size())
    {
        d->mAlgType = type;
        emit algTypeChanged(d->mAlgType);
    }
}

SmartVideoDataPrivate::SmartVideoDataPrivate(QObject *parent)
    : QObject(parent)
    , mAlgType(alg::None)
    , mIsDetecting(false)
    , mIterationTime(0)
    , pThread(new QThread())
{
    this->moveToThread(pThread);
    connect(pThread, &QThread::started, this,
            &SmartVideoDataPrivate::detect);
}

SmartVideoDataPrivate::~SmartVideoDataPrivate()
{
    if (pThread->isRunning())
    {
        pThread->quit();
        pThread->wait();
    }

    delete pThread;
//    delete pSearcher;
}

void SmartVideoDataPrivate::detect()
{
    Q_Q(SmartVideoData);

    pSearcher = new Searcher();
    __print;
    mIsDetecting = true;
    emit q->isDetectingChanged(mIsDetecting);

    pSearcher->setAlgoritm(mAlgType);
    pSearcher->setTemplate(mTemplate);


    connect(pSearcher, &Searcher::detected, this,
            &SmartVideoDataPrivate::detected);
    connect(pSearcher, &Searcher::error, q, &SmartVideoData::message);

    while(!pThread->isInterruptionRequested())
    {
        cv::Mat frame;
        q->pCapture->read(frame);
        pSearcher->setInputImage(frame);
        pSearcher->detect();
    }

    disconnect(pSearcher, &Searcher::detected, this,
            &SmartVideoDataPrivate::detected);
    disconnect(pSearcher, &Searcher::error, q, &SmartVideoData::message);

    mIsDetecting = false;
    emit q->isDetectingChanged(mIsDetecting);
    delete pSearcher;
    pThread->quit();
}
