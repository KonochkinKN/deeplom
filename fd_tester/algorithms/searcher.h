#ifndef SEARCHER_H
#define SEARCHER_H

#include <QObject>

#include <opencv/cv.h>

class Searcher : public QObject
{
    Q_OBJECT

public:
    explicit Searcher(QObject* parent = 0);
    virtual ~Searcher();

public slots:
    void detect();
    cv::Mat getResult();
    qint64 getElapsedTime();
    void setAlgorithm(int alg);
    void setTemplate(QString path);
    void setInputImage(cv::Mat image);

signals:
    void detected();
    void error(QString err);

private:
    int mAlgorithm;
    qint64 mElapsedTime;

    cv::Mat mTemplate;
    cv::Mat mInputImg;
    cv::Mat mResultImg;
};

#endif // SEARCHER_H
