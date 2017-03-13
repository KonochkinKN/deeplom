#ifndef SMARTVIDEODATA_P_H
#define SMARTVIDEODATA_P_H

#include "searcher.h"
#include "manager.h"

#include <QObject>
#include <QThread>

class SmartVideoData;

class SmartVideoDataPrivate : public QObject
{
    Q_OBJECT
    Q_DECLARE_PUBLIC(SmartVideoData)

public:
    explicit SmartVideoDataPrivate(QObject* parent = 0);
    virtual ~SmartVideoDataPrivate();

public slots:
    void detect();

signals:
    void detected();

private:
    int mAlgType;
    bool mIsDetecting;
    QString mTemplate;
    qint64 mIterationTime;

    QThread* pThread;
    Searcher* pSearcher;
    SmartVideoData* q_ptr;
};

#endif // SMARTVIDEODATA_P_H
