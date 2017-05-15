#ifndef LOGREWRITER_H
#define LOGREWRITER_H

#include <QObject>

class LogRewriter : public QObject
{
    Q_OBJECT

public:
    explicit LogRewriter(QObject *parent = 0);
    ~LogRewriter();

signals:

public slots:
    bool changeVideoFile(QString logFile, QString videoFile);
};

#endif // LOGREWRITER_H
