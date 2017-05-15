#include "searcher.h"
#include "polymath.h"
#include "manager.h"

#include <QList>
#include <QDebug>
#include <QThread>
#include <QElapsedTimer>

#include <cmath>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/core/core.hpp>
#include <opencv2/core/types_c.h>
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/nonfree/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>

namespace
{
const float MIN_DIST = 0.001;
}

Searcher::Searcher(QObject *parent)
    : QObject(parent)
    , mAlgorithm(alg::None)
    , mElapsedTime(0)
{

}

Searcher::~Searcher()
{

}

void Searcher::setInputImage(cv::Mat image)
{
    mInputImg = image;
}

void Searcher::setTemplate(QString path)
{
    mTemplate = cv::imread(path.toStdString().c_str(), CV_LOAD_IMAGE_GRAYSCALE);
}

void Searcher::setAlgorithm(int alg)
{
    mAlgorithm = alg;
    mResultImg.release();
}

cv::Mat Searcher::getResult() const
{
    return this->mResultImg;
}

qint64 Searcher::getElapsedTime() const
{
    return this->mElapsedTime;
}

void Searcher::detect()
{
    mElapsedTime = 0;
    mCurrentStrobe.clear();

    if (mTemplate.empty())
    {
        emit error(tr("Invalid template"));
        return;
    }

    if (mInputImg.empty())
    {
        emit error(tr("Invalid input image"));
        return;
    }

    // choose approach
    cv::Ptr<cv::Feature2D> detector;
    cv::Ptr<cv::DescriptorMatcher> matcher;
    switch(mAlgorithm)
    {
    case alg::ORB:
    {
        detector = new cv::ORB(500, 1.2f);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::ORB4:
    {
        detector = new cv::ORB(500, 1.2f, 8, 31, 0, 4);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::SIFT:
    {
        detector = new cv::SIFT(500);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::SURF:
    {
        detector = new cv::SURF(100);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::USURF:
    {
        detector = new cv::SURF(100, 4, 3, true, true);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::BRISK:
    {
        detector = new cv::BRISK(30, 4, 1.0f);
        matcher = new cv::FlannBasedMatcher(
                    new cv::flann::LshIndexParams(20, 10, 2));
        break;
    }
    default:
    {
        emit error(tr("Unknown algorithm"));
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

    // detect
    QElapsedTimer* timer = new QElapsedTimer;
    timer->start();
    try
    {
        detector->operator()(mTemplate, cv::Mat(), tmpKeypoints, tmpDescriptors);
        detector->operator()(mInputImg, cv::Mat(), imgKeypoints, imgDescriptors);
    }
    catch(cvflann::FLANNException& e)
    {
        emit error(tr("OpenCV FLANN exception: %1").arg(e.what()));
        return;
    }
    catch(cv::Exception& e)
    {
        emit error(tr("OpenCV exception: %1").arg(e.what()));
        return;
    }

    // check elapsed time
    mElapsedTime = timer->elapsed();

    if (mAlgorithm == alg::ORB || mAlgorithm == alg::ORB4)
    {
        tmpDescriptors.convertTo(tmpDescriptors, CV_32F);
        imgDescriptors.convertTo(imgDescriptors, CV_32F);
    }

    // match
    matcher->match(tmpDescriptors, imgDescriptors, matches);

    double max_dist = 0;
    double min_dist = 100;

    // check distances
    for (int i = 0; i < tmpDescriptors.rows; i++)
    {
        double dist = matches[i].distance;
        if (dist < min_dist && dist > MIN_DIST) min_dist = dist;
        if (dist > max_dist) max_dist = dist;
    }

    for (uint i = 0; i < matches.size(); i++)
    {
        if(matches.at(i).distance <= 2 * min_dist)
            good_matches.push_back(matches.at(i));
    }

    mResultImg = mInputImg;

    // get strobe if its possible
    if (good_matches.size() >= 4)
    {
        for (uint i = 0; i < good_matches.size(); i++)
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

        if (PolyMath::instance()->cvIsCorrect(scene_corners))
        {
            line(mResultImg, scene_corners.at(0), scene_corners.at(1), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(1), scene_corners.at(2), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(2), scene_corners.at(3), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(3), scene_corners.at(0), cv::Scalar(0, 255, 0), 2 );

            mCurrentStrobe = PolyMath::instance()->cvToQPoly(scene_corners);
        }
    }

    if(mElapsedTime < 25)
        QThread::msleep(25 - mElapsedTime);

    detector.release();
    matcher.release();
    emit detected();
}

QPolygonF Searcher::getStrobe() const
{
    return this->mCurrentStrobe;
}
