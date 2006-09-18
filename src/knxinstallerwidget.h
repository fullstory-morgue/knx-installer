//
// C++ Interface: knxinstallerWidget
//
// Description: Main Window of the KANOTIX installer
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _KNXINSTALLERWIDGET_H_
#define _KNXINSTALLERWIDGET_H_

#include "knxinstallerwidgetbase.h"

#include <kprocess.h>

class knxinstallerWidget : public knxinstallerWidgetBase
{
    Q_OBJECT

public:
    knxinstallerWidget(QWidget* parent = 0, const char* name = 0, WFlags fl = 0 );
    ~knxinstallerWidget();
    QString processOutput;
    void checkDevfs();
    /*$PUBLIC_FUNCTIONS$*/

protected:
    /*$PROTECTED_FUNCTIONS$*/

protected slots:
    /*$PROTECTED_SLOTS$*/

public slots:
    virtual void cmdClose_clicked();
    virtual void cmdBack_clicked();
    virtual void cmdNext_clicked();
    virtual void getOutput(KProcess*, char*, int);
    virtual void tab_changed(QWidget*);
    virtual void installerType_changed();
    virtual void rereadFstab();
    virtual void startParted();
    virtual void rootPart_changed();
    virtual void installationMode_changed(int);
    void partMan_exited(bool);

private:
    void readFstab();
    QString userpass_crypt;
    QString rootpass_crypt;
    QString services_start;
    QPoint window_position;
};

#endif

