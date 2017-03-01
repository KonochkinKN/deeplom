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
    virtual ~Searcher();

public slots:
    void startDetecting();
    qint64 getElapsedTime();
    void setAlgoritm(int alg);
    void setTemplate(QString path);
    void setTemplate(cv::Mat image);
    void setInputImage(QString path);
    void setInputImage(cv::Mat image);
    QPair<cv::Mat, QString> getResult();
    // Optimal detection
    bool isDetecting();
    void setOptimalDetect(bool flag);
    std::vector<cv::Point2f> getStrobe();

private slots:
    void detect();
    void chooseDetect();
    void optimalDetect();

signals:
    void detected();
    void error(QString msg);

private:
    int mAlgorithm;
    bool mIsOptimal;
    QThread* pThread;
    qint64 mElapsedTime;

    cv::Mat mTemplate;
    cv::Mat mInputImg;
    cv::Mat mResultImg;
    cv::Mat mTemplateRes;
    cv::Mat mInputImgRes;
    std::vector<cv::Point2f> mSceneCorners;
};

#endif // SEARCHER_H
