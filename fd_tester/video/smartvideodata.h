#ifndef SMARTVIDEODATA_H
#define SMARTVIDEODATA_H

#include "videodata.h"
#include "searcher.h"

#include <QMetaObject>
#include <QObject>
#include <QTimer>

class SmartVideoData : public VideoData
{
    Q_OBJECT

public:
    explicit SmartVideoData(QObject* parent = 0);
    virtual ~SmartVideoData();

    Q_PROPERTY(bool isDetecting READ isDetecting
               NOTIFY isDetectingChanged)
    Q_PROPERTY(QString templateImage READ templateImage
               WRITE setTemplateImage NOTIFY templateImageChanged)
    Q_PROPERTY(int algType READ algType WRITE setAlgType
               NOTIFY algTypeChanged)
    Q_PROPERTY(qint64 iterationTime READ iterationTime
               NOTIFY iterationTimeChanged)

    Q_INVOKABLE int algType();
    Q_INVOKABLE bool isDetecting();
    Q_INVOKABLE qint64 iterationTime();
    Q_INVOKABLE QString templateImage();

public slots:
    void stopDetecting();
    void startDetecting();
    void setAlgType(int type);
    void setTemplateImage(QString imgPath);

private slots:
    void detect();
    void onDetected();

signals:
    void detected();
    void message(QString txt);
    void algTypeChanged(int type);
    void iterationTimeChanged(qint64 time);
    void isDetectingChanged(bool detecting);
    void templateImageChanged(QString imgPath);

private:
    int mAlgType;
    QTimer* pTimer;
    bool mIsDetecting;
    QString mTemplate;
    QMetaObject::Connection mConnection;

    Searcher* pSearcher;
};

#endif // SMARTVIDEODATA_H
