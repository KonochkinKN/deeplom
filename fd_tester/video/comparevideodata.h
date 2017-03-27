#ifndef COMPAREVIDEODATA_H
#define COMPAREVIDEODATA_H

#include <QPolygonF>
#include <QObject>
#include <QList>

#include "videodata.h"
#include "logger.h"

class CompareVideoData : public VideoData
{
    Q_OBJECT

public:
    explicit CompareVideoData(QObject* parent = 0);
    virtual ~CompareVideoData();

    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(QString refLog READ refLog
               WRITE setRefLog NOTIFY refLogChanged)
    Q_PROPERTY(QString testLog READ testLog
               WRITE setTestLog NOTIFY testLogChanged)

    Q_INVOKABLE bool isValid(){return mIsValid;}
    Q_INVOKABLE QString refLog(){return mRefLog;}
    Q_INVOKABLE QString testLog(){return mTestLog;}

public slots:
    void setRefLog(QString data);
    void setTestLog(QString data);

signals:
    void message(QString txt);
    void isValidChanged(bool valid);
    void refLogChanged(QString log);
    void testLogChanged(QString log);

private:
    bool mIsValid;
    QString mRefLog;
    QString mTestLog;
    quint32 mRefFirstFrame;
    quint32 mTestFirstFrame;
    QList<QPolygonF> mRefStrobes;
    QList<QPolygonF> mTestStrobes;

    void timerEvent(QTimerEvent*) override;
};

#endif // COMPAREVIDEODATA_H
