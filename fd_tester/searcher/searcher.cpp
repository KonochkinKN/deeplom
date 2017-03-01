#include "searcher.h"

#include <QList>
#include <QElapsedTimer>

#include <cmath>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

Searcher::Searcher(QObject *parent)
    : QObject(parent)
    , mAlgorithm(-1)
    , mIsOptimal(false)
    , pThread(new QThread())
    , mElapsedTime(0)
    , mSceneCorners(std::vector<cv::Point2f>(4, cv::Point2f(0, 0)))
{
    this->moveToThread(pThread);
    connect(pThread, &QThread::started, this, &Searcher::chooseDetect);
}

Searcher::~Searcher()
{
    cv::destroyAllWindows();
    if(pThread->isRunning())
        pThread->quit();
    pThread->deleteLater();
}

void Searcher::startDetecting()
{
    if(!pThread->isRunning())
        pThread->start();
}

qint64 Searcher::getElapsedTime()
{
    return mElapsedTime;
}

void Searcher::setAlgoritm(int alg)
{
    mAlgorithm = alg;
    mResultImg.release();
}

void Searcher::setTemplate(QString path)
{
    mTemplate = cv::imread(path.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    mTemplateRes = mTemplate;
}

void Searcher::setTemplate(cv::Mat image)
{
    mTemplate = image;
    mTemplateRes = image;
}

void Searcher::setInputImage(QString path)
{
    mInputImg = cv::imread(path.toStdString(), CV_LOAD_IMAGE_GRAYSCALE);
    mInputImgRes = mInputImg;
}

void Searcher::setInputImage(cv::Mat image)
{
    mInputImg = image;
    mInputImgRes = image;
}

void Searcher::detect()
{
    mTemplate = mTemplateRes;
    mInputImg = mInputImgRes;

    if(mTemplate.empty())
    {
        emit error("Invalid template");
        pThread->quit();
        return;
    }

    if(mInputImg.empty())
    {
        emit error("Invalid image");
        pThread->quit();
        return;
    }

    cv::Ptr<cv::Feature2D> detector;
    cv::Ptr<cv::DescriptorMatcher> matcher;
    switch(mAlgorithm)
    {
    case eORB:
    {
        detector = new cv::ORB(500, 2);
        matcher = new cv::BFMatcher(cv::NORM_HAMMING, true);
        break;
    }
    case eORB2:
    {
        detector = new cv::ORB(500, 2, 8, 31, 0, 4);
        matcher = new cv::BFMatcher(cv::NORM_HAMMING2, true);
        break;
    }
    case eSIFT:
    {
        detector = new cv::SIFT(400);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case eSURF:
    {
        detector = new cv::SURF(400);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case eBRISK:
    {
        detector = new cv::BRISK();
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    default:
    {
        emit error("Unknown algorithm");
        pThread->quit();
        return;
    }
    }

    std::vector<cv::KeyPoint> tmpKeypoints;
    std::vector<cv::KeyPoint> imgKeypoints;

    std::vector<cv::DMatch> matches;
    std::vector<cv::DMatch> good_matches;

    std::vector<cv::Point2f> obj;
    std::vector<cv::Point2f> scene;

    cv::Mat tmpDescriptors;
    cv::Mat imgDescriptors;

    QElapsedTimer* timer = new QElapsedTimer;
    timer->start();
    try
    {
        detector->operator()(mTemplate, cv::Mat(), tmpKeypoints, tmpDescriptors);
        detector->operator()(mInputImg, cv::Mat(), imgKeypoints, imgDescriptors);
    }
    catch(cv::Exception& e)
    {
        emit error(tr("OpenCV exception: %1").arg(e.what()));
        pThread->quit();
        return;
    }

    if(mAlgorithm == eBRISK)
    {
        tmpDescriptors.convertTo(tmpDescriptors, CV_32F);
        imgDescriptors.convertTo(imgDescriptors, CV_32F);
    }
    matcher->match(tmpDescriptors, imgDescriptors, matches);

    double max_dist = 0;
    double min_dist = 100;

    for(int i = 0; i < tmpDescriptors.rows; i++)
    {
        double dist = matches[i].distance;
        if( dist < min_dist && dist > MIN_DIST) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }

    for(uint i = 0; i < matches.size(); i++)
    {
        if(matches.at(i).distance <= 2 * min_dist)
            good_matches.push_back(matches.at(i));
    }

    cv::drawMatches(mTemplate, tmpKeypoints, mInputImg, imgKeypoints,
                    good_matches, mResultImg, cv::Scalar::all(-1),
                    cv::Scalar::all(-1), std::vector<char>(),
                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

    if(good_matches.size() >= 4)
    {
        for(uint i = 0; i < good_matches.size(); i++)
        {
            obj.push_back(tmpKeypoints.at(good_matches.at(i).queryIdx).pt);
            scene.push_back(imgKeypoints.at(good_matches.at(i).trainIdx).pt);
        }

        cv::Mat H = cv::findHomography(obj, scene, CV_RANSAC);

        std::vector<cv::Point2f> obj_corners(4);
        obj_corners[0] = cv::Point(0, 0);
        obj_corners[1] = cv::Point(mTemplate.cols, 0);
        obj_corners[2] = cv::Point(mTemplate.cols, mTemplate.rows);
        obj_corners[3] = cv::Point(0, mTemplate.rows);
        std::vector<cv::Point2f> scene_corners(4);

        cv::perspectiveTransform(obj_corners, scene_corners, H);

        if(scene_corners.at(0).x < scene_corners.at(1).x &&
                scene_corners.at(1).y < scene_corners.at(2).y &&
                scene_corners.at(3).x < scene_corners.at(2).x &&
                scene_corners.at(0).y < scene_corners.at(3).y)
        {
            line(mResultImg, scene_corners.at(0) + cv::Point2f(mTemplate.cols, 0),
                 scene_corners.at(1) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(1) + cv::Point2f(mTemplate.cols, 0),
                 scene_corners.at(2) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(2) + cv::Point2f(mTemplate.cols, 0),
                 scene_corners.at(3) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(3) + cv::Point2f(mTemplate.cols, 0),
                 scene_corners.at(0) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
            mSceneCorners = scene_corners;
        }
    }
    mElapsedTime = timer->elapsed();

    emit detected();
    detector.release();
    matcher.release();
    pThread->quit();
}

QPair<cv::Mat, QString> Searcher::getResult()
{
    QString alg;
    QString timeInfo = tr(" with elapsed time = %1 msec")
            .arg(QString::number(mElapsedTime));
    switch(mAlgorithm)
    {
    case eORB:
    {
        alg = "ORB";
        break;
    }
    case eORB2:
    {
        alg = "ORB2";
        break;
    }
    case eSIFT:
    {
        alg = "SIFT";
        break;
    }
    case eSURF:
    {
        alg = "SURF";
        break;
    }
    case eBRISK:
    {
        alg = "BRISK";
        break;
    }
    default:
        alg = "Unknown";
    }

    return QPair<cv::Mat, QString>(mResultImg, alg + timeInfo);
}

bool Searcher::isDetecting()
{
    return pThread->isRunning();
}

void Searcher::chooseDetect()
{
    if(mIsOptimal)
        this->optimalDetect();
    else
        this->detect();
}

void Searcher::optimalDetect()
{
    mTemplate = mTemplateRes;
    mInputImg = mInputImgRes;

    if(mInputImg.empty() || mTemplate.empty())
    {
        emit error("Invalid images");
        pThread->quit();
        return;
    }

    Searcher* searchers = new Searcher[5];
    searchers[0].setAlgoritm(eORB);
    searchers[1].setAlgoritm(eORB2);
    searchers[2].setAlgoritm(eSIFT);
    searchers[3].setAlgoritm(eSURF);
    searchers[4].setAlgoritm(eBRISK);

    while(searchers[0].isDetecting() ||
          searchers[1].isDetecting() ||
          searchers[2].isDetecting() ||
          searchers[3].isDetecting() ||
          searchers[4].isDetecting()){}

    int N = -1;
    double dx1, dx2, dy1, dy2, diag1, diag2, deltaDiags;
    double dDiags = 100;
    std::vector<cv::Point2f> strobe;

    for(int i = 0; i < 5; i++)
    {
        strobe = searchers[i].getStrobe();
        dx1 = strobe.at(0).x - strobe.at(2).x;
        dy1 = strobe.at(0).y - strobe.at(2).y;
        dx2 = strobe.at(1).x - strobe.at(3).x;
        dy2 = strobe.at(1).y - strobe.at(3).y;
        diag1 = sqrt(pow(dx1, 2) + pow(dy1, 2));
        diag2 = sqrt(pow(dx2, 2) + pow(dy2, 2));
        deltaDiags = std::abs(diag1 - diag2);
        if(diag1 > 10 && diag2 > 10 && dDiags > deltaDiags)
        {
            dDiags = deltaDiags;
            N = i;
        }
    }
    if(N != -1)
    {
        mElapsedTime = searchers[N].getElapsedTime();
        mResultImg = searchers[N].getResult().first;
        mAlgorithm = N;
        emit detected();
    }
    else
    {
        mElapsedTime = searchers[0].getElapsedTime();
        mResultImg = searchers[0].getResult().first;
        mAlgorithm = 0;
        emit error("Can't find object\n"
                   "Try again with another options or template");
        emit detected();
    }
    searchers->deleteLater();
    pThread->quit();
}

void Searcher::setOptimalDetect(bool flag)
{
    mIsOptimal = flag;
}

std::vector<cv::Point2f> Searcher::getStrobe()
{
    return mSceneCorners;
}
