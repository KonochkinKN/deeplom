#ifndef LOGGER_H
#define LOGGER_H

#include <QFile>
#include <QPair>
#include <QObject>
#include <QPolygon>

#include "logheader.h"

class Logger : public QObject
{
    Q_OBJECT

public:
    explicit Logger(quint32 algType, quint32 firstFrame,
                    QString video, QObject* parent = 0);
    explicit Logger(QString path, QObject* parent = 0);
    virtual ~Logger();

    LogHeader readHeader();
    QPair<QPolygon, qint64> readNextBlock();
    QString fileAbsPath() const;
    bool atEnd() const;
    void close();

public slots:
    bool writeHeader();
    bool writeNextBlock(QPolygon strobe, qint64);

private:
    QFile mFile;
    LogHeader mHeader;
};

#endif // LOGGER
