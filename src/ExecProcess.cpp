//
// C++ Implementation: ExecProcess
//
// Description: Execute a Process in a Shell (/bin/sh)
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ExecProcess.h"

ExecProcess::ExecProcess()
{
    _buffer = QString::null;
    _process = new KProcess();

    connect(_process, SIGNAL(receivedStdout(KProcess*, char*, int)),
            this, SLOT(slotProcessOutput(KProcess*, char*, int)));

}

ExecProcess::~ExecProcess()
{
}

void ExecProcess::setCommand(QString command)
{
    // make clean
    _process->clearArguments();
    _buffer = QString::null;
    
    *_process << "/bin/sh";
    *_process << "-c";
    *_process << command;
}


void ExecProcess::start(bool block)
{
    if( block )
        _process->start(KProcess::Block, KProcess::Stdout);
    else
        _process->start(KProcess::DontCare, KProcess::Stdout);
}

QString ExecProcess::getBuffer()
{
    return _buffer;
}

int ExecProcess::exitStatus()
{
    return _process->exitStatus();
}

bool ExecProcess::normalExit()
{
    return _process->normalExit();
}

void ExecProcess::slotProcessOutput(KProcess* process, char* buffer, int len)
{
    if (process != _process) return;

    _buffer.append(QString::fromLocal8Bit(buffer, len));
}

