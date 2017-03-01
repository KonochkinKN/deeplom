#ifndef LOGHEADER_H
#define LOGHEADER_H

#include <QDateTime>
#include "manager.h"

class LogHeader
{
public:
    LogHeader() :
        algorithmType(alg::None)
    {}

    LogHeader(const LogHeader& rhs)
        : algorithmType(rhs.algorithmType)
        , logDateTime(rhs.logDateTime)
        , firstFrame(rhs.firstFrame)
        , videoFile(rhs.videoFile)
    {}
    ~LogHeader(){}

    quint32 algorithmType;
    QDateTime logDateTime;
    quint32 firstFrame;
    QString videoFile;
    bool isReference() const {return algorithmType == alg::None;}
    bool isValid() const {return algorithmType < (uint)alg::algToString.size();}
};

#endif // LOGHEADER_H
