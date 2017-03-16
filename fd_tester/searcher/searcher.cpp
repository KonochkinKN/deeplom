#include "searcher.h"
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
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/nonfree/features2d.hpp>

namespace
{
const int MIN_DIST = 0.001;
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

cv::Mat Searcher::getResult()
{
    return mResultImg;
}

qint64 Searcher::getElapsedTime()
{
    return mElapsedTime;
}

void Searcher::detect()
{
    __print;
    if (mTemplate.empty())
    {
        emit error("Invalid template");
    }

    if (mInputImg.empty())
    {
        emit error("Invalid input image");
    }

    cv::Ptr<cv::Feature2D> detector = new cv::SIFT(400);
    cv::Ptr<cv::DescriptorMatcher> matcher = new cv::FlannBasedMatcher();

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

    mResultImg = mInputImg;

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
            line(mResultImg, scene_corners.at(0), scene_corners.at(1), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(1), scene_corners.at(2), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(2), scene_corners.at(3), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(3), scene_corners.at(0), cv::Scalar(0, 255, 0), 2 );
        }
    }
    mElapsedTime = timer->elapsed();
    if(mElapsedTime < 100)
        QThread::msleep(100 - mElapsedTime);

    detector.release();
    matcher.release();
    emit detected();
}
