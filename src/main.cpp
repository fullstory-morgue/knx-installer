//
// C++ Implementation: main
//
// Description: Check if knxinstaller is run by root and display the Main-Window
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include "ExecProcess.h"

#include "knxinstaller.h"
#include <kapplication.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static const char description[] =
    I18N_NOOP("A KDE KPart Application");

static const char version[] = "0.1";

static KCmdLineOptions options[] =
{
//    { "+[URL]", I18N_NOOP( "Document to open" ), 0 },
    KCmdLineLastOption
};

int main(int argc, char **argv)
{
    KAboutData about("knxinstaller", I18N_NOOP("Kanotix-Installer by A. Loibl"), version, description,
                     KAboutData::License_Custom, "(C) 2005 Andreas Loibl", 0, 0, "andreas@andreas-loibl.de");
    about.addAuthor( "Andreas Loibl", 0, "andreas@andreas-loibl.de" );
    KCmdLineArgs::init(argc, argv, &about);
    KCmdLineArgs::addCmdLineOptions( options );
    KApplication app;
    knxinstaller *mainWin = 0;

    if (app.isRestored())
    {
        RESTORE(knxinstaller);
    }
    else
    {
        // no session.. just start up normally
        KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

        /// @todo do something with the command line args here
        
        ExecProcess *shell = new ExecProcess();
        shell->setCommand( "id -u" );
        shell->start( true );
        
        if ( shell->getBuffer().toInt() > 0 )
        {
            shell->setCommand( "kanotix-su knxinstaller" );
            shell->start( false );
            return 0;
        }
        
        mainWin = new knxinstaller();
        app.setMainWidget( mainWin );
        mainWin->show();

        args->clear();
    }

    // mainWin has WDestructiveClose flag by default, so it will delete itself.
    return app.exec();
}

