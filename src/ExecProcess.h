//
// C++ Interface: ExecProcess
//
// Description: Execute a Process in a Shell (/bin/sh)
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef EXECPROCESS_H
#define EXECPROCESS_H

#include <kprocess.h>
#include <iostream.h>
#include <fstream.h>

class ExecProcess : public QObject
{
    Q_OBJECT

public:
    ExecProcess();
    ~ExecProcess();

    QString getBuffer();
    int exitStatus();
    bool normalExit();
    void setCommand(QString command);
    void start(bool block=true);

protected:
    KProcess* _process;
    QString _buffer;

protected slots:
    void slotProcessOutput(KProcess* process, char* buffer, int buflen);
};

/*
class ExecProcess : public QObject
{
    Q_OBJECT

public:
    ExecProcess() : _process(0) {};
    ~ExecProcess();

    void start();
    QString getBuffer();

signals:
    void finished(const QString& output);

protected:
    KProcess* _process;
    QString _buffer;

protected slots:
    void slotProcessExited(KProcess* process);
    void slotProcessOutput(KProcess* process, char* buffer, int buflen);
};*/

#endif
