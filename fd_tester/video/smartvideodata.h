#ifndef SMARTVIDEODATA_H
#define SMARTVIDEODATA_H

#include "videodata.h"
#include "searcher.h"

#include <QObject>
#include <QThread>

class SmartVideoData : public VideoData
{
    Q_OBJECT

public:
    explicit SmartVideoData(QObject *parent = 0);
    ~SmartVideoData();

    Q_PROPERTY(bool isDetecting READ isDetecting
               NOTIFY isDetectingChanged)
    Q_PROPERTY(QString templateImage READ templateImage
               WRITE setTemplateImage NOTIFY templateImageChanged)

    Q_INVOKABLE bool isDetecting(){return mIsDetecting;}
    Q_INVOKABLE QString templateImage(){return mTemplate;}

public slots:
    void detect();
    void stopDetecting();
    void startDetecting();
    void setTemplateImage(QString imgPath);

private slots:
    void onDetected();

signals:
    void detected();
    void message(QString txt);
    void isDetectingChanged(bool detecting);
    void templateImageChanged(QString imgPath);

private:
    bool mIsDetecting;
    QString mTemplate;
    Searcher* pSearcher;
    QThread* pThread;
};

#endif // SMARTVIDEODATA_H
