# set the include path for X, qt and KDE
INCLUDES = $(all_includes)

# these are the headers for your project
noinst_HEADERS = knxinstaller.h ExecProcess.h

# let automoc handle all of the meta source files (moc)
METASOURCES = AUTO

messages: rc.cpp
	$(EXTRACTRC) `find . -name \*.ui -o -name \*.rc` > rc.cpp
	$(XGETTEXT) *.cpp -o $(podir)/knxinstaller.pot

KDE_ICON = AUTO

#########################################################################
# APPLICATION SECTION
#########################################################################
# this is the program that gets installed.  it's name is used for all
# of the other Makefile.am variables
bin_PROGRAMS = knxinstaller

# the application source, library search path, and link libraries
knxinstaller_SOURCES = main.cpp knxinstallerwidget.cpp \
			knxinstallerwidgetbase.ui knxinstaller.cpp \
			ExecProcess.cpp knxinstaller-script
knxinstaller_LDFLAGS = $(KDE_RPATH) $(all_libraries)
knxinstaller_LDADD = -lcrypt $(LIB_KDEUI)

# this is where the desktop file will go 
shelldesktopdir   = $(kde_appsdir)/Kanotix
shelldesktop_DATA = knxinstaller.desktop

# this is where the shell's XML-GUI resource file goes
shellrcdir   = $(kde_datadir)/knxinstaller
shellrc_DATA = knxinstallerui.rc

bin_SCRIPTS = kanotix-check-knofig kanotix-update knxinstaller-script \
	knxinstaller-recover
