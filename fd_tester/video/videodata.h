#ifndef VIDEODATA_H
#define VIDEODATA_H

#include <QObject>
#include <QString>
#include <QThread>
#include <QVideoSurfaceFormat>

namespace cv{
    class VideoCapture;
}

class QAbstractVideoSurface;

class VideoData : public QObject
{
    Q_OBJECT

public:
    explicit VideoData(QObject *parent = 0);
    virtual ~VideoData();

    Q_PROPERTY(QAbstractVideoSurface* videoSurface READ videoSurface WRITE setVideoSurface)
    Q_PROPERTY(quint32 position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(bool hasVideo READ hasVideo NOTIFY hasVideoChanged)
    Q_PROPERTY(QString source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(double fps READ fps NOTIFY fpsChanged)
    Q_PROPERTY(double durationSec READ durationSec NOTIFY durationSecChanged)
    Q_PROPERTY(int width READ width NOTIFY widthChanged)
    Q_PROPERTY(int height READ height NOTIFY heightChanged)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)
    Q_PROPERTY(quint32 endPosition READ endPosition NOTIFY endPositionChanged)

    Q_INVOKABLE QAbstractVideoSurface* videoSurface() const;
    Q_INVOKABLE quint32 position();
    Q_INVOKABLE bool hasVideo();
    Q_INVOKABLE QString source(){return mSource;}
    Q_INVOKABLE double fps(){return mFps;}
    Q_INVOKABLE double durationSec(){return mDurationSec;}
    Q_INVOKABLE int width();
    Q_INVOKABLE int height();
    Q_INVOKABLE bool playing();
    Q_INVOKABLE quint32 endPosition(){return mEndPosition;}

signals:
    void positionChanged(quint32);
    void sourceChanged(QString);
    void hasVideoChanged(bool);
    void fpsChanged(double);
    void durationSecChanged(double);
    void widthChanged(int);
    void heightChanged(int);
    void playingChanged(bool);
    void endPositionChanged(quint32);
    void endReached();

public slots:
    void setVideoSurface( QAbstractVideoSurface* surface );
    void setPosition(quint32 data);
    void setSource(QString data);
    void play();
    void pause();
    void stop();
    void seek(quint32 frame);

protected slots:
    void setFps(double data);
    void setDurationSec(double data);
    void setWidth(int data);
    void setHeight(int data);
    void onTimeout();
    void onSourceChanged();
    void updateData(bool null = false);
    void setEndPosition(quint32 data);

protected:
    QTimer* pPlaybackTimer;
    cv::VideoCapture* pCapture;
    QVideoSurfaceFormat mFormat;
    QAbstractVideoSurface* pSurface;

    quint32 mFrame;
    quint32 mEndPosition;
    QString mSource;
    double mFps;
    double mDurationSec;
    int mWidth;
    int mHeight;

    void closeSurface();
    virtual void timerEvent(QTimerEvent*) override;
};

#endif // VIDEODATA_H
