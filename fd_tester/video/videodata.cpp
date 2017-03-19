#include "videodata.h"

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

VideoData::VideoData(QObject *parent)
    : QObject(parent)
    , pPlaybackTimer(new QTimer())
    , pCapture(new cv::VideoCapture())
    , pSurface(nullptr)
{
    this->updateData(true);
    connect(this, &VideoData::sourceChanged, this, &VideoData::onSourceChanged);

    pPlaybackTimer->setTimerType(Qt::PreciseTimer);
    connect(pPlaybackTimer, &QTimer::timeout, this, &VideoData::onTimeout);
    this->startTimer(50);
}

VideoData::~VideoData()
{
    delete pCapture;
    pCapture = nullptr;
}

QAbstractVideoSurface* VideoData::videoSurface() const
{
    return pSurface;
}

quint32 VideoData::position()
{
    return mFrame;
}

bool VideoData::hasVideo()
{
    return pCapture->isOpened();
}

int VideoData::width()
{
    return mWidth;
}

int VideoData::height()
{
    return mHeight;
}

bool VideoData::playing()
{
    return pPlaybackTimer->isActive();
}

void VideoData::setVideoSurface(QAbstractVideoSurface* surface)
{
    this->closeSurface();
    pSurface = surface;
}

void VideoData::setPosition(quint32 data)
{
    if ( !pCapture->isOpened())
        return;

    if (data >= pCapture->get(CV_CAP_PROP_FRAME_COUNT))
        return;

    mFrame = data;
    if (mFrame != pCapture->get(CV_CAP_PROP_POS_FRAMES))
        pCapture->set(CV_CAP_PROP_POS_FRAMES, data);

    mFrame = pCapture->get(CV_CAP_PROP_POS_FRAMES);
    emit positionChanged(mFrame);
}

void VideoData::setSource(QString data)
{
    mSource = data;
    emit sourceChanged(mSource);
}

void VideoData::play()
{
    if (mFps > 0)
    {
        pPlaybackTimer->start(1000.0/mFps);
        emit playingChanged(this->playing());
    }
}

void VideoData::pause()
{
    pPlaybackTimer->stop();
    emit playingChanged(this->playing());
}

void VideoData::stop()
{
    pPlaybackTimer->stop();
    this->setPosition(0);
    emit playingChanged(this->playing());
}

void VideoData::seek(quint32 frame)
{
    this->setPosition(frame);
}

void VideoData::setFps(double data)
{
    mFps = data;
    emit fpsChanged(mFps);
}

void VideoData::setDurationSec(double data)
{
    mDurationSec = data;
    emit durationSecChanged(mDurationSec);
}

void VideoData::setWidth(int data)
{
    mWidth = data;
    emit widthChanged(mWidth);
}

void VideoData::setHeight(int data)
{
    mHeight = data;
    emit heightChanged(mHeight);
}

void VideoData::onTimeout()
{
    if ( !pCapture->isOpened() )
        return;

    quint32 pos = this->position() + 1;
    if (pos < pCapture->get(CV_CAP_PROP_FRAME_COUNT))
    {
        this->setPosition(this->position() + 1);
    }
    else
    {
        this->stop();
    }
}

void VideoData::onSourceChanged()
{
    emit hasVideoChanged(false);

    try
    {
        pCapture->open(QUrl(mSource).path().toStdString().c_str());
    }
    catch(cv::Exception& e)
    {
        this->updateData(true);
        this->closeSurface();
        __print << e.what();
        return;
    }

    emit hasVideoChanged(this->hasVideo());

    if (!pCapture->isOpened())
    {
        this->updateData(true);
        this->closeSurface();
        __print;
        return;
    }

    this->updateData();
}

void VideoData::updateData(bool null)
{
    if (null)
    {
        this->setDurationSec(-1);
        this->setFps(-1);
        this->setWidth(-1);
        this->setHeight(-1);
        mFrame = 0;
        return;
    }

    double framesTotal = pCapture->get(CV_CAP_PROP_FRAME_COUNT);
    pCapture->set(CV_CAP_PROP_POS_FRAMES, framesTotal-1);

    double duration = pCapture->get(CV_CAP_PROP_POS_MSEC)/1000.0;
    double fps = framesTotal/duration;

    this->setWidth(pCapture->get( CV_CAP_PROP_FRAME_WIDTH) );
    this->setHeight(pCapture->get( CV_CAP_PROP_FRAME_HEIGHT) );

    pCapture->set(CV_CAP_PROP_POS_FRAMES, 0);
    mFrame = pCapture->get( CV_CAP_PROP_POS_FRAMES );

    this->setDurationSec(duration);
    this->setFps(fps);
}

void VideoData::closeSurface()
{
    if( pSurface && pSurface->isActive() )
        pSurface->stop();
}

void VideoData::timerEvent(QTimerEvent*)
{
    if( !pSurface )
        return;

    cv::Mat frame;
    if (pCapture->isOpened() && pCapture->get(CV_CAP_PROP_POS_FRAMES) == mFrame
            && mFrame < mFps*mDurationSec)
    {
        pCapture->read( frame );
    }

    if(frame.empty())
        return;

    cvtColor(frame, frame, CV_RGB2RGBA, 4);

    QImage frameImg((const uchar *) frame.data, frame.cols, frame.rows,
                    frame.step, QImage::Format_ARGB32);
    frameImg.bits(); // enforce deep copy

    QVideoFrame vFrame (frameImg);

    QVideoFrame::PixelFormat pixelFormat =
            QVideoFrame::pixelFormatFromImageFormat( frameImg.format() );

    this->closeSurface();
    mFormat = QVideoSurfaceFormat(
                QPixmap::fromImage(frameImg).size(), pixelFormat);
    pSurface->start( mFormat );

    pSurface->present( vFrame );
    emit positionChanged(mFrame);
}
