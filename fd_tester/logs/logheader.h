#ifndef LOGHEADER_H
#define LOGHEADER_H

#include <QDateTime>
#include "manager.h"

class LogHeader
{
public:
    LogHeader()
        : version(-1)
        , algorithmType(alg::None)
    {}

    LogHeader(const LogHeader& rhs)
        : title(rhs.title)
        , algorithmType(rhs.algorithmType)
        , logDateTime(rhs.logDateTime)
        , firstFrame(rhs.firstFrame)
        , videoFile(rhs.videoFile)
    {}
    ~LogHeader(){}

    int version;
    QString title;
    quint32 algorithmType;
    QDateTime logDateTime;
    quint32 firstFrame;
    QString videoFile;
    bool isReference() const {return algorithmType == alg::None;}
    bool isValid() const
    {
        return ((algorithmType < (uint)alg::algToString.size())
                && (version == __version) && !title.isEmpty());
    }
};

#endif // LOGHEADER_H
