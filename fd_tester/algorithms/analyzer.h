#ifndef ANALYZER_H
#define ANALYZER_H

#include <QObject>

class Analyzer : public QObject
{
    Q_OBJECT

public:
    explicit Analyzer(QObject *parent = 0);
    virtual ~Analyzer();

    Q_PROPERTY(QStringList result READ result NOTIFY resultChanged)
    Q_PROPERTY(QString refLog READ refLog
               WRITE setRefLog NOTIFY refLogChanged)
    Q_PROPERTY(QString logForTest READ logForTest
               WRITE setLogForTest NOTIFY logForTestChanged)

    Q_INVOKABLE QStringList result(){return mResult;}
    Q_INVOKABLE QString refLog(){return mRefLogTitle;}
    Q_INVOKABLE QString logForTest(){return mLogForTestTitle;}

public slots:
    void analyze();
    void setRefLog(QString title);
    void setLogForTest(QString title);

signals:
    void message(QString txt);
    void resultChanged(QStringList res);
    void refLogChanged(QString logTitle);
    void logForTestChanged(QString logTitle);

private:
    QStringList mResult;
    QString mRefLogFile;
    QString mRefLogTitle;
    QString mLogForTestFile;
    QString mLogForTestTitle;

    void countEfficiency();
};

#endif // ANALYZER_H
