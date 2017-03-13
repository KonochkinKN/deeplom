#ifndef SEARCHER_H
#define SEARCHER_H

#include <QPair>
#include <QObject>
#include <QThread>

#include <opencv/cv.h>

#include "manager.h"

namespace
{
const double MIN_DIST = 0.001;
}

class Searcher : public QObject
{
    Q_OBJECT

public:
    explicit Searcher(QObject *parent = 0);
    ~Searcher();

public slots:
    void detect();
    qint64 getElapsedTime();
    void setAlgoritm(int alg);
    void setTemplate(QString path);
    void setTemplate(cv::Mat image);
    void setInputImage(QString path);
    void setInputImage(cv::Mat image);
    QPair<cv::Mat, QString> getResult();
    std::vector<cv::Point2f> getStrobe();

signals:
    void detected();
    void error(QString msg);

private:
    int mAlgorithm;
    bool mIsOptimal;
    qint64 mElapsedTime;

    cv::Mat mTemplate;
    cv::Mat mInputImg;
    cv::Mat mResultImg;
    cv::Mat mTemplateRes;
    cv::Mat mInputImgRes;
    std::vector<cv::Point2f> mSceneCorners;
};

#endif // SEARCHER_H
