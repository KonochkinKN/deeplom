#ifndef SMARTVIDEODATA_H
#define SMARTVIDEODATA_H

#include "videodata.h"
#include "searcher.h"
#include "manager.h"

#include <QObject>
#include <QThread>

class SmartVideoDataPrivate;

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
    Q_PROPERTY(int algType READ algType WRITE setAlgType
               NOTIFY algTypeChanged)
    Q_PROPERTY(qint64 iterationTime READ iterationTime
               NOTIFY iterationTimeChanged)

    Q_INVOKABLE int algType();
    Q_INVOKABLE bool isDetecting();
    Q_INVOKABLE QString templateImage();
    Q_INVOKABLE qint64 iterationTime();

public slots:
    void stopDetecting();
    void startDetecting();
    void setAlgType(int type);
    void setTemplateImage(QString imgPath);

private slots:
    void onDetected();

signals:
    void message(QString txt);
    void algTypeChanged(int type);
    void iterationTimeChanged(qint64 time);
    void isDetectingChanged(bool detecting);
    void templateImageChanged(QString imgPath);

private:
    Q_DECLARE_PRIVATE(SmartVideoData)
    SmartVideoDataPrivate* const d_ptr;
};

#endif // SMARTVIDEODATA_H
