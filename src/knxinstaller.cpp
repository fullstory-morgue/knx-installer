//
// C++ Implementation: knxinstaller
//
// Description: Display the Main-Window
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <kmainwindow.h>
#include <klocale.h>

#include "knxinstaller.h"
#include "knxinstallerwidget.h"

knxinstaller::knxinstaller()
    : KMainWindow( 0, "knxinstaller" )
{
    setCentralWidget( new knxinstallerWidget( this ) );
}

knxinstaller::~knxinstaller()
{
}

#include "knxinstaller.moc"
