#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QPair>
#include <QObject>
#include <QPolygonF>

#include "logheader.h"

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(QString title, quint32 algType, quint32 firstFrame,
                    QString video, QObject* parent = 0);
    explicit Logger(QString path, QObject* parent = 0);
    virtual ~Logger();

    LogHeader readHeader();
    QPair<QPolygonF, qint64> readNextBlock();
    QString fileAbsPath() const;
    bool atEnd() const;
    void close();

public slots:
    bool writeHeader();
    bool writeNextBlock(QPolygonF strobe, qint64);

private:
    QFile mFile;
    LogHeader mHeader;
};

#endif // LOGGER
