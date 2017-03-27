#include "comparevideodata.h"
#include "logheader.h"
#include "manager.h"
#include "logger.h"

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

CompareVideoData::CompareVideoData(QObject* parent)
    : VideoData(parent)
    , mIsValid(false)
    , mRefFirstFrame(0)
    , mTestFirstFrame(0)
{

}

CompareVideoData::~CompareVideoData()
{

}

void CompareVideoData::setRefLog(QString data)
{
    this->stop();
    mIsValid = false;
    emit isValidChanged(mIsValid);

    mRefLog = data;
    emit refLogChanged(data);

    QStringList logs = Manager::instance()->logs();
    if (!logs.contains(mRefLog))
    {
        emit message(tr("Invalid reference log title"));
        return;
    }

    QString logPath = Manager::instance()->logsPath() + mRefLog +
            Manager::instance()->logFilesExtension();

    Logger* reader = new Logger(logPath);
    LogHeader header = reader->readHeader();

    if (!header.isValid())
    {
        emit message(tr("Invalid reference log header"));
        return;
    }

    if (!header.isReference())
    {
        emit message(tr("Invalid reference log header format"));
        return;
    }

    if (reader->atEnd())
    {
        emit message(tr("Empty reference log"));
        return;
    }

    mRefFirstFrame = header.firstFrame;

    while (!reader->atEnd())
    {
        mRefStrobes.append(reader->readNextBlock().first);
    }

    this->setPosition(std::min(mRefFirstFrame, mTestFirstFrame));

    mIsValid = true;
    emit isValidChanged(mIsValid);

    reader->close();
    delete reader;
}

void CompareVideoData::setTestLog(QString data)
{
    this->stop();
    mIsValid = false;
    emit isValidChanged(mIsValid);

    mTestLog = data;
    emit testLogChanged(data);

    QStringList logs = Manager::instance()->logs();
    if (!logs.contains(mTestLog))
    {
        emit message(tr("Invalid test log title"));
        return;
    }

    QString logPath = Manager::instance()->logsPath() + mTestLog +
            Manager::instance()->logFilesExtension();

    Logger* reader = new Logger(logPath);
    LogHeader header = reader->readHeader();

    if (!header.isValid())
    {
        emit message(tr("Invalid test log header"));
        return;
    }

    if (header.isReference())
    {
        emit message(tr("Invalid test log header format"));
        return;
    }

    if (reader->atEnd())
    {
        emit message(tr("Empty test log"));
        return;
    }

    mTestFirstFrame = header.firstFrame;

    while (!reader->atEnd())
    {
        mTestStrobes.append(reader->readNextBlock().first);
    }

    this->setPosition(std::min(mRefFirstFrame, mTestFirstFrame));

    mIsValid = true;
    emit isValidChanged(mIsValid);

    reader->close();
    delete reader;
}

void CompareVideoData::timerEvent(QTimerEvent *)
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

    if (this->isValid())
    {
        if (mFrame < (mRefFirstFrame + mRefStrobes.size()))
        {
            QPolygonF poly = mRefStrobes.at(mFrame - mRefFirstFrame);
            if (poly.size() > 3)
            {
                cv::Point2f pt0 = cv::Point2f(poly.at(0).x(), poly.at(0).y());
                cv::Point2f pt1 = cv::Point2f(poly.at(1).x(), poly.at(1).y());
                cv::Point2f pt2 = cv::Point2f(poly.at(2).x(), poly.at(2).y());
                cv::Point2f pt3 = cv::Point2f(poly.at(3).x(), poly.at(3).y());
                cv::line(frame, pt0, pt1, cv::Scalar(0, 255, 0), 2 );
                cv::line(frame, pt1, pt2, cv::Scalar(0, 255, 0), 2 );
                cv::line(frame, pt2, pt3, cv::Scalar(0, 255, 0), 2 );
                cv::line(frame, pt3, pt0, cv::Scalar(0, 255, 0), 2 );
            }
        }

        if (mFrame < (mTestFirstFrame + mTestStrobes.size()))
        {
            QPolygonF poly = mTestStrobes.at(mFrame - mTestFirstFrame);
            if (poly.size() > 3)
            {
                cv::Point2f pt0 = cv::Point2f(poly.at(0).x(), poly.at(0).y());
                cv::Point2f pt1 = cv::Point2f(poly.at(1).x(), poly.at(1).y());
                cv::Point2f pt2 = cv::Point2f(poly.at(2).x(), poly.at(2).y());
                cv::Point2f pt3 = cv::Point2f(poly.at(3).x(), poly.at(3).y());
                cv::line(frame, pt0, pt1, cv::Scalar(255, 0, 0), 2 );
                cv::line(frame, pt1, pt2, cv::Scalar(255, 0, 0), 2 );
                cv::line(frame, pt2, pt3, cv::Scalar(255, 0, 0), 2 );
                cv::line(frame, pt3, pt0, cv::Scalar(255, 0, 0), 2 );
            }
        }
    }

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
