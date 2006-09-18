//
// C++ Interface: knxinstaller
//
// Description: Display the Main-Window
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef _KNXINSTALLER_H_
#define _KNXINSTALLER_H_

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kmainwindow.h>

/**
 * @short Application Main Window
 * @author Andreas Loibl <andreas@andreas-loibl.de>
 * @version 0.3.5
 */
class knxinstaller : public KMainWindow
{
    Q_OBJECT
public:
    /**
     * Default Constructor
     */
    knxinstaller();

    /**
     * Default Destructor
     */
    virtual ~knxinstaller();
};

#endif // _KNXINSTALLER_H_
