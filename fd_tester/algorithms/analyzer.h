#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>
#include <QJsonObject>

namespace
{
struct Data
{
    double eff;     // summary efficiency
    double avgDT;   // average detection time
    int detErrors;  // summary detection errors
    int totFrames;  // total frames processed
    double avgRA;   // average reference strobe area
    double avgTA;   // average test strobe area
    double avgIA;   // average intersection strobe area
    double avgXD;   // average x-axis center difference
    double avgYD;   // average y-axis center difference
    double avgAD;   // average absolute center difference

    void average()
    {
        if (this->totFrames > 0)
        {
            this->eff   /= this->totFrames;
            this->avgDT /= this->totFrames;
            this->avgRA /= this->totFrames;
            this->avgTA /= this->totFrames;
            this->avgIA /= this->totFrames;
            this->avgXD /= this->totFrames;
            this->avgYD /= this->totFrames;
            this->avgAD /= this->totFrames;
        }
    }

    void setNull()
    {
        this->eff       = 0.0;
        this->avgDT     = 0.0;
        this->detErrors = 0;
        this->totFrames = 0;
        this->avgRA     = 0.0;
        this->avgTA     = 0.0;
        this->avgIA     = 0.0;
        this->avgXD     = 0.0;
        this->avgYD     = 0.0;
        this->avgAD     = 0.0;
    }
};
}

class Analyzer : public QObject
{
    Q_OBJECT

public:
    explicit Analyzer(QObject *parent = 0);
    virtual ~Analyzer();

    Q_PROPERTY(QStringList result READ result NOTIFY resultChanged)
    Q_PROPERTY(QString refLog READ refLog
               WRITE setRefLog NOTIFY refLogChanged)
    Q_PROPERTY(QString testLog READ testLog
               WRITE setTestLog NOTIFY testLogChanged)

    Q_INVOKABLE QStringList result(){return mResult;}
    Q_INVOKABLE QString refLog(){return mRefLogTitle;}
    Q_INVOKABLE QString testLog(){return mTestLogTitle;}

public slots:
    void analyze();
    void setRefLog(QString title);
    void setTestLog(QString title);

signals:
    void message(QString txt);
    void resultChanged(QStringList res);
    void refLogChanged(QString logTitle);
    void testLogChanged(QString logTitle);

private:
    QStringList mResult;
    QString mRefLogFile;
    QString mRefLogTitle;
    QString mTestLogFile;
    QString mTestLogTitle;

    // Insider information
    quint32 mRefFrame;
    quint32 mTestFrame;
    QString mCurrentVideo;
    int mCurrentAlgorithm;
    QList<QPair<QPolygonF, qint64>> mRefData;
    QList<QPair<QPolygonF, qint64>> mTestData;

    bool readRefLog();
    bool readTestLog();
    void countEfficiency();
    void setResult(const Data &data);
    void saveResult(const Data &data);
    QJsonObject parseData(const Data &data);
};

#endif // ANALYZER_H
