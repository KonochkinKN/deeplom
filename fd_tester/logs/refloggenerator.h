#ifndef REFLOGGENERATOR_H
#define REFLOGGENERATOR_H

#include "logger.h"
#include <QObject>

class RefLogGenerator : public QObject
{
    Q_OBJECT

public:
    explicit RefLogGenerator(QObject *parent = 0);
    virtual ~RefLogGenerator();

    Q_PROPERTY(QString title READ title WRITE setTitle
               NOTIFY titleChanged)
    Q_PROPERTY(QString videoFile READ videoFile WRITE setVideoFile
               NOTIFY videoFileChanged)
    Q_PROPERTY(quint32 firstFrame READ firstFrame WRITE setFirstFrame
               NOTIFY firstFrameChanged)
    Q_PROPERTY(quint32 currentFrame READ currentFrame WRITE setCurrentFrame
               NOTIFY currentFrameChanged)
    Q_PROPERTY(bool isWriting READ isWriting NOTIFY isWritingChanged)

    Q_INVOKABLE QString title(){return mTitle;}
    Q_INVOKABLE QString videoFile(){return mVideoFile;}
    Q_INVOKABLE quint32 firstFrame(){return mFirstFrame;}
    Q_INVOKABLE quint32 currentFrame(){return mCurrentFrame;}
    Q_INVOKABLE bool isWriting(){return mIsWriting;}

signals:
    void message(QString txt);
    void titleChanged(QString name);
    void videoFileChanged(QString video);
    void firstFrameChanged(quint32 frame);
    void currentFrameChanged(quint32 frame);
    void isWritingChanged(bool writing);

public slots:
    bool isValid();
    void stopWriting();
    void startWriting();
    void setTitle(QString data);
    void setVideoFile(QString data);
    void setFirstFrame(quint32 data);
    void setCurrentFrame(quint32 data);

    void saveStrobe(QRectF strobe, int angle);
    void saveLog();

private:
    bool mIsWriting;
    QString mTitle;
    QString mVideoFile;
    quint32 mFirstFrame;
    quint32 mCurrentFrame;
    QList <QPolygonF> mDataList;
    Logger* pLogger;

};

#endif // REFLOGGENERATOR_H
