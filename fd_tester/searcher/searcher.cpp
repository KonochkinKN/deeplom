#include "searcher.h"

#include <QList>
#include <QThread>
#include <QDebug>
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
    , mAlgorithm(alg::None)
    , mIsOptimal(false)
    , mElapsedTime(0)
    , mSceneCorners(std::vector<cv::Point2f>(4, cv::Point2f(0, 0)))
{

}

Searcher::~Searcher()
{

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
        return;
    }

    if(mInputImg.empty())
    {
        emit error("Invalid image");
        return;
    }

    cv::Ptr<cv::Feature2D> detector;
    cv::Ptr<cv::DescriptorMatcher> matcher;
    switch(mAlgorithm)
    {
    case alg::ORB:
    {
        detector = new cv::ORB(500, 2);
        matcher = new cv::BFMatcher(cv::NORM_HAMMING, true);
        break;
    }
    case alg::ORB2:
    {
        detector = new cv::ORB(500, 2, 8, 31, 0, 4);
        matcher = new cv::BFMatcher(cv::NORM_HAMMING2, true);
        break;
    }
    case alg::SIFT:
    {
        detector = new cv::SIFT(400);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::SURF:
    {
        detector = new cv::SURF(400);
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    case alg::BRISK:
    {
        detector = new cv::BRISK();
        matcher = new cv::FlannBasedMatcher();
        break;
    }
    default:
    {
        emit error("Unknown algorithm");
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
        __print;
        detector->operator()(mTemplate, cv::Mat(), tmpKeypoints, tmpDescriptors);
        __print;
        detector->operator()(mInputImg, cv::Mat(), imgKeypoints, imgDescriptors);
    }
    catch(cvflann::FLANNException& e)
    {
        __print;
        emit error(tr("OpenCV FLANN exception: %1").arg(e.what()));
        return;
    }
    catch(cv::Exception& e)
    {
        __print;
        emit error(tr("OpenCV exception: %1").arg(e.what()));
        return;
    }

    __print;
    if(mAlgorithm == alg::BRISK)
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

//    cv::drawMatches(mTemplate, tmpKeypoints, mInputImg, imgKeypoints,
//                    good_matches, mResultImg, cv::Scalar::all(-1),
//                    cv::Scalar::all(-1), std::vector<char>(),
//                    cv::DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );

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
//            line(mResultImg, scene_corners.at(0) + cv::Point2f(mTemplate.cols, 0),
//                 scene_corners.at(1) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
//            line(mResultImg, scene_corners.at(1) + cv::Point2f(mTemplate.cols, 0),
//                 scene_corners.at(2) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
//            line(mResultImg, scene_corners.at(2) + cv::Point2f(mTemplate.cols, 0),
//                 scene_corners.at(3) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
//            line(mResultImg, scene_corners.at(3) + cv::Point2f(mTemplate.cols, 0),
//                 scene_corners.at(0) + cv::Point2f(mTemplate.cols, 0), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(0), scene_corners.at(1), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(1), scene_corners.at(2), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(2), scene_corners.at(3), cv::Scalar(0, 255, 0), 2 );
            line(mResultImg, scene_corners.at(3), scene_corners.at(0), cv::Scalar(0, 255, 0), 2 );
            mSceneCorners = scene_corners;
        }
    }
    mElapsedTime = timer->elapsed();
    if(mElapsedTime < 100)
        QThread::msleep(100 - mElapsedTime);

    detector.release();
    matcher.release();
    emit detected();
}

QPair<cv::Mat, QString> Searcher::getResult()
{
    QString alg;
    QString timeInfo = tr(" with elapsed time = %1 msec")
            .arg(QString::number(mElapsedTime));
    if(mAlgorithm > -1 && mAlgorithm < alg::algToString.size())
    {
        alg = alg::algToString.at(mAlgorithm);
    }
    else
    {
        alg = "Unknown";
    }

    return QPair<cv::Mat, QString>(mResultImg, alg + timeInfo);
}

std::vector<cv::Point2f> Searcher::getStrobe()
{
    return mSceneCorners;
}
