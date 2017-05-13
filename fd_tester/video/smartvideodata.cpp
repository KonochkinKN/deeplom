#include "smartvideodata.h"
#include "manager.h"

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
    , mAlgType(alg::None)
    , pTimer(new QTimer())
    , pLogger(nullptr)
    , mIsDetecting(false)
    , mIterationTime(0)
    , pSearcher(new Searcher())
{

}

SmartVideoData::~SmartVideoData()
{
    delete pLogger;
    pTimer->deleteLater();
    pSearcher->deleteLater();
}

int SmartVideoData::algType()
{
    return this->mAlgType;
}

bool SmartVideoData::isDetecting()
{
    return this->mIsDetecting;
}

qint64 SmartVideoData::iterationTime()
{
    return this->mIterationTime;
}

QString SmartVideoData::templateImage()
{
    return this->mTemplate;
}

void SmartVideoData::setAlgType(int type)
{
    if (this->mAlgType != type && type != alg::None &&
            type < alg::algToString.size())
    {
        this->mAlgType = type;
        emit algTypeChanged(this->mAlgType);
    }
}

void SmartVideoData::setTemplateImage(QString imgPath)
{
    this->mTemplate = QUrl(imgPath).path();
    emit templateImageChanged(this->mTemplate);
}

void SmartVideoData::startDetecting()
{
    delete pLogger;
    pLogger = new Logger(alg::algToString.at(this->mAlgType), this->mAlgType,
                         this->position(), this->mSource);

    if (!this->pLogger->writeHeader())
    {
        emit message(tr("Error occured: Unable to log"));
        return;
    }

    this->mConnection = QObject::connect(pSearcher, &Searcher::detected, [=]
    {
        this->pTimer->singleShot(100, this, &SmartVideoData::onDetected);
    });

    connect(pSearcher, &Searcher::error, this, &SmartVideoData::message);
    connect(pSearcher, &Searcher::error, this, &SmartVideoData::stopDetect);

    this->mIsDetecting = true;
    emit isDetectingChanged(this->mIsDetecting);

    // start this shit
    this->detect();
}

void SmartVideoData::stopDetecting()
{
    this->stopDetect();

    emit message(tr("Log saved succesfully"));
}

void SmartVideoData::detect()
{
    if (!this->isDetecting())
        return;

    cv::Mat frame;
    this->pCapture->read(frame);

    this->pSearcher->setAlgorithm(this->mAlgType);
    this->pSearcher->setTemplate(this->mTemplate);
    this->pSearcher->setInputImage(frame);
    this->pSearcher->detect();
}

void SmartVideoData::onDetected()
{
    if (!this->pSurface)
        return;

    if (!this->mIsDetecting)
        return;

    cv::Mat frame = this->pSearcher->getResult();

    if (frame.empty())
        return;

    if (!this->pLogger->writeNextBlock(this->pSearcher->getStrobe(),
                                       this->pSearcher->getElapsedTime()))
    {
        emit message(tr("Error occured: Unable to log result"));
        this->stopDetecting();
        return;
    }

    cv::cvtColor(frame, frame, CV_RGB2RGBA, 4);

    QImage frameImg((const uchar *) frame.data, frame.cols, frame.rows,
                    frame.step, QImage::Format_ARGB32);
    frameImg.bits(); // enforce deep copy

    QVideoFrame vFrame(frameImg);
    QVideoFrame::PixelFormat pixelFormat =
            QVideoFrame::pixelFormatFromImageFormat(frameImg.format());

    this->closeSurface();
    this->mFormat = QVideoSurfaceFormat(
                QPixmap::fromImage(frameImg).size(), pixelFormat);

    this->pSurface->start(this->mFormat);
    this->pSurface->present(vFrame);

    this->setPosition(this->mFrame + 1);
    this->setIterationTime(this->pSearcher->getElapsedTime());

    this->detect();
}

void SmartVideoData::stopDetect()
{
    this->pLogger->close();

    QObject::disconnect(mConnection);
    disconnect(pSearcher, &Searcher::error, this, &SmartVideoData::message);

    this->mIsDetecting = false;
    emit isDetectingChanged(this->mIsDetecting);

    this->setIterationTime(0);
}

void SmartVideoData::setIterationTime(qint64 time)
{
    this->mIterationTime = time;
    emit iterationTimeChanged(this->mIterationTime);
}
