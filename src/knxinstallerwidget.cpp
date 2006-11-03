//
// C++ Implementation: knxinstallerWidget
//
// Description: Main Window of the KANOTIX installer
//
//
// Author: Andreas Loibl <andreas@andreas-loibl.de>, (C) 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#include <qtabwidget.h>
#include <qlabel.h>
#include <qtable.h>
#include <qcombobox.h>
#include <qpushbutton.h>
#include <qstringlist.h>
#include <qcheckbox.h>
#include <qbuttongroup.h>
#include <qregexp.h>
#include <qfile.h>
#include <qdir.h>
#include <qmime.h>
#include <qlineedit.h>
#include <qtextbrowser.h>
#include <qmessagebox.h>
#include <klocale.h>
#include <kprocess.h>
#include <kpassdlg.h>
#include <iostream.h>
#include <unistd.h>
#include <fstream.h>

#include "knxinstallerwidget.h"
#include "ExecProcess.h"

knxinstallerWidget::knxinstallerWidget(QWidget* parent, const char* name, WFlags fl)
        : knxinstallerWidgetBase(parent,name,fl)
{
    this->readFstab();

    // Default ist Neuinstallation, darum Update-Tab entfernen
    tabWidget->removePage(tabUpdate);
    tabWidget->removePage(tabRecover);
    
    // Vorhandene .knofig parsen
    QFile knofig( QDir::home().filePath(".knofig") );
    QString temp;
    QString variable;
    QString wert;
    
    if( knofig.exists() )
    {
        cout << "Eine .knofig ist vorhanden, sie wird eingelesen..." << endl;
        knofig.open( IO_ReadOnly );
        while ( !knofig.atEnd() )
        {
            knofig.readLine( temp, 512 );
            
            // Kommentare löschen
            temp = temp.section( "#", 0, 0 );
            temp = temp.simplifyWhiteSpace();
            if ( temp.isNull() ) {
                continue;
            }
            
            if ( temp.contains( "=") )
            {
                variable = temp.section( "=", 0, 0 );
                wert = temp.mid( variable.length()+2, temp.length()-variable.length()-3 );
                
                cout << "Variable: '" << variable << "'\tWert: '" << wert << "'" << endl;
                if( variable == "HD_FORMAT" )
                    if( wert == "yes" ) chkRootFormat->setChecked( true ); else chkRootFormat->setChecked( false );
                else if( variable == "HD_FSTYPE" )
                    inputRootFormat->setCurrentText( wert );
                else if( variable == "HD_CHOICE" )
                    inputRootPartition->setCurrentText( wert );
                else if( variable == "NAME_NAME" )
                    inputName->setText( wert );
                else if( variable == "USER_NAME" )
                    inputUserName->setText( wert );
                else if( variable == "HOST_NAME" )
                    inputHostName->setText( wert );
                else if( variable == "BOOT_WHERE" )
                    if( wert == "mbr" ) inputBootOrt->setCurrentItem( 0 ); else inputBootOrt->setCurrentItem( 1 ); 
                else if( variable == "BOOT_DISK" )
                    if( wert == "yes" ) inputBootDisk->setChecked( true ); else inputBootDisk->setChecked( false );
                else if( variable == "BOOT_LOADER" )
                    if( wert == "lilo" ) inputBootmanager->setCurrentItem( 1 ); else inputBootmanager->setCurrentItem( 0 ); 
                else if( variable == "HD_MAP" )
                {
                    if ( wert.contains( ":" ) )
                    {
                        partTable->setNumRows( 0 );
                        QStringList map_list = QStringList::split( " ", wert );
                        for ( QStringList::Iterator it = map_list.begin(); it != map_list.end(); ++it ) {
                            QString map = *it;
                            int tempRow = partTable->numRows();
                            partTable->insertRows( tempRow );
                            partTable->setText( tempRow, 0, map.section( ":", 0, 0 ) );
                            partTable->setText( tempRow, 1, map.section( ":", 1, 1 ) );
                        }
                        chkMountpoints->setChecked( true );
                        this->readFstab();
                    }
                }
                else if( variable == "USERPASS_CRYPT" )
                {
                    userpass_crypt=wert;
                    inputUserPW1->insert("******");
                    inputUserPW2->insert("******");
                }
                else if( variable == "ROOTPASS_CRYPT" )
                {
                    rootpass_crypt=wert;
                    inputRootPW1->insert("******");
                    inputRootPW2->insert("******");
                }
                else if( variable == "SERVICES_START" )
                    services_start=wert;
                else if( variable == "AUTOLOGIN" )
                    if( wert == "yes" ) chkAutoLogin->setChecked( true ); else chkAutoLogin->setChecked( false );
            }
        }
        knofig.close();
    }
}

knxinstallerWidget::~knxinstallerWidget()
{}

void knxinstallerWidget::readFstab()
{
    // Partitionen in die Tabelle laden
    QFile fstab( "/etc/fstab" );
    QString temp;

    // Tabelle leeren
    // partTable->setNumRows( 0 );
    for( int j = 0; j < partTable->numRows(); j++ )
    {
        if ( ! partTable->text(j, 1) )
        {
            partTable->removeRow( j );
            j--;
        }
    }
    
    fstab.open( IO_ReadOnly );
    while ( !fstab.atEnd() ) {
        fstab.readLine( temp, 512 );

        // Kommentare löschen
        temp = temp.section( "#", 0, 0 );
        temp = temp.simplifyWhiteSpace();
        if ( temp.isNull() ) {
            continue;
        }
        
        // Festplatten-Partitionen raussuchen
        if ( temp.contains( QRegExp( "^/dev/(hd|sd|ide|scsi)" ) ) )
        {
            // keine Swap-Partitionen!
            if ( temp.section( " ", 1, 1 ).contains( "/" ) )
            {
                int tempRow = partTable->numRows();
                bool already_in_list = false;
                for( int j = 0; j < partTable->numRows(); j++ )
                {
                    if ( partTable->text(j, 0) == temp.section( " ", 0, 0 ) )
                    {
                        already_in_list=true;
                    }
                }
                if( ! already_in_list || ! partTable->numRows() )
                {
                    partTable->insertRows( tempRow );
                    partTable->setText( tempRow, 0, temp.section( " ", 0, 0 ) );
                }
                //partTable->setText( tempRow, 1, temp.section( " ", 1, 1 ) );
            }
        }
    }
    fstab.close();

    // Partitionen, die als Root-Partition in Frage kommen in die ComboBox laden
    ExecProcess *shell = new ExecProcess();
    shell->setCommand("fdisk -l");
    shell->start(true);
    
    inputRootPartition->clear();
    inputRootPartition_Update->clear();
    inputRootPartition_Recover->clear();
    
    QStringList fdisk_zeilen = QStringList::split( "\n", shell->getBuffer() );
    for ( QStringList::Iterator it = fdisk_zeilen.begin(); it != fdisk_zeilen.end(); ++it ) {
        QString zeile = *it;
        if ( zeile.contains( "Linux" ) && !zeile.lower().contains( "swap" ) )
        {
            inputRootPartition->insertItem( zeile.section( " ", 0, 0 ) );
            inputRootPartition_Update->insertItem( zeile.section( " ", 0, 0 ) );
            inputRootPartition_Recover->insertItem( zeile.section( " ", 0, 0 ) );
        }
    }
    // Root-Partition vorauswählen
    for ( int j = 0; j < partTable->numRows(); j++ )
    {
        if ( partTable->text( j, 0 ) == inputRootPartition->currentText() )
        {
            partTable->setText( j, 1, "/" );
        }
    }
    
    shell->setCommand("awk '{if(($2==0)&&($4~/^[hs]d/)){print $4;}}' /proc/partitions");
    shell->start(true);
    inputBootOrt_Recover->clear();
    inputClearOrt_Recover->clear();
    QStringList harddisks = QStringList::split( "\n", shell->getBuffer() );
    for ( QStringList::Iterator it = harddisks.begin(); it != harddisks.end(); ++it ) {
        QString harddisk = *it;
        inputBootOrt_Recover->insertItem( i18n("MBR of %1").arg("/dev/"+harddisk) );
        inputClearOrt_Recover->insertItem( "/dev/"+harddisk );
    }
    inputBootOrt_Recover->insertItem( i18n("Rootpartition") );
}

void knxinstallerWidget::cmdClose_clicked()
{
    exit(0);
}

QString encrypt( QString pwd )
{
    unsigned long seed[2];
    char salt[] = "$1$........";
    const char *const seedchars =
        "./0123456789ABCDEFGHIJKLMNOPQRST"
        "UVWXYZabcdefghijklmnopqrstuvwxyz";
    int i;
    
    seed[0] = time(NULL);
    seed[1] = getpid() ^ (seed[0] >> 14 & 0x30000);
    
    for (i = 0; i < 8; i++)
        salt[3+i] = seedchars[(seed[i/5] >> (i%5)*6) & 0x3f];
    
    QString password = crypt(pwd, salt);
    
    password.replace(QChar('$'),"\\$");
    
    return( password );
}

void knxinstallerWidget::cmdNext_clicked()
{
    // Shell zum Ausführen vorbereiten
    ExecProcess *shell = new ExecProcess();
    
    if ( tabWidget->currentPageIndex() < tabWidget->count()-1 )
    {
        tabWidget->setCurrentPage(tabWidget->currentPageIndex()+1);
    }
    else
    {
        if ( bgInstallmode->selectedId() == 1 )
        {
            // Kommando zusammenbauen
            QString command = QString("konsole --noclose -e kanotix-update -bl ") + inputBootmanager_Update->currentText().lower()
            + QString(" -bw ");
            if( inputBootOrt_Update->currentItem() == 1 ) command+=QString("partition"); else command+=QString("mbr");
            command+=QString(" -l /tmp/ku-exclude.list ") + inputRootPartition_Update->currentText();
            
            // Exclude-Text abspeichern
            fstream f;
            f.open("/tmp/ku-exclude.list", ios::out);
            f << txtExclude->text();
            f.close();
            
            // Ausführen
            shell->setCommand( command );
            shell->start( false );
            exit( 0 );
        }
        else if ( bgInstallmode->selectedId() == 2 )
        {
            if ( bgRecovermode->selectedId() == 0 )
            {
                QString command = "knxinstaller-recover";
                command+=" --bootloader " + inputBootmanager_Recover->currentText().lower();
                command+=" --root " + inputRootPartition_Recover->currentText();
                if ( inputBootOrt_Recover->currentText().contains("/dev/") )
                {
                    command+=" --type mbr";
                    command+=" --device " + inputBootOrt_Recover->currentText().mid(i18n("MBR of %1").arg("").length());
                }
                else
                    command+=" --type partition";
    
                shell->setCommand( command );
                shell->start( true );
                
                bool isFailure = true;
                
                // Jede Zeile der Ausgabe verarbeiten
                QStringList recoverOutput = QStringList::split( "\n", shell->getBuffer() );
                for ( QStringList::Iterator it = recoverOutput.begin(); it != recoverOutput.end(); ++it ) {
                    QString row = *it;
                    if ( row.startsWith( "ERROR: " ) )
                    {
                        QMessageBox::warning( this, i18n( "Error" ), row.mid( 7 ) );
                    }
                    if ( row.startsWith( "SUCCESS" ) )
                    {
                        isFailure = false;
                    }
                    cout << row << endl;
                }
                
                // Wenn Fehler aufgetreten, dann Funktion beenden
                if ( isFailure ) return;
                
                QMessageBox::information( this, "Kanotix-Installer", i18n( "Bootloader successfully installed." ) );
    
                // tabWidget->setCurrentPage(0);
            }
            else if ( bgRecovermode->selectedId() == 1 )
            {
                if ( QMessageBox::warning( this, i18n( "Clear MBR" ), i18n( "Are you sure that you want to restore\nthe standart MBR to harddisk %1?" ).arg(inputClearOrt_Recover->currentText()), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
                {
                    QString command = "lilo -M "+inputClearOrt_Recover->currentText()+" -s /dev/null";
                    shell->setCommand(command);
                    shell->start(true);
                    if ( shell->normalExit() )
                    {
                        QMessageBox::information( this, "Kanotix-Installer", i18n("Successfully restored default MBR.") );
                        // tabWidget->setCurrentPage(0);
                    }
                    else
                    {
                        QMessageBox::critical( this, "Kanotix-Installer", i18n("An Error occurred while running %1!").arg(command) );
                    }
                }
            }
        }
        else if ( bgInstallmode->selectedId() == 0 )
        {
        // Nur Installer starten
        if ( buttonGroup1->selectedId() == 2 )
        {
            shell->setCommand( "knoppix-installer" );
            shell->start( false );
            exit( 0 );
        }

        bool doChecks = true;
        if ( chkIgnoreCheck->isChecked() )
        {
            if( QMessageBox::warning( this, "Kanotix-Installer", i18n("Warning: You have selected to force the installation.\nThis means, that no RAM-/Swap-/Partition-/Format-Check etc. will be done!\n\nDo you really want to continue?"), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes)
            {
                doChecks = false;
            }
            else
            {
                return;
            }
        }

        if( doChecks ) {
        QString error = "";
        // Daten auf Richtigkeit überprüfen
        if ( ((QString) inputRootPW1->password()) != ((QString) inputRootPW2->password()) )
        {
            tabWidget->setCurrentPage( 3 );
            inputRootPW1->setFocus();
            error = i18n( "The specified Root-Passwords don't match!" );
        }
        else if ( ((QString) inputRootPW1->password()).length() < 6 )
        {
            tabWidget->setCurrentPage( 3 );
            inputRootPW1->setFocus();
            error = i18n( "The specified Root-Password is too short (min. 6 chars)" );
        }
        else if ( ((QString) inputRootPW1->password()).length() > 20 )
        {
            tabWidget->setCurrentPage( 3 );
            inputRootPW1->setFocus();
            error = i18n( "The specified Root-Password is too long (max. 20 chars)" );
        }
        else if ( ((QString) inputUserPW1->password()) != ((QString) inputUserPW2->password()) )
        {
            tabWidget->setCurrentPage( 3 );
            inputUserPW1->setFocus();
            error = i18n( "The specified User-Passwords don't match!" );
        }
        else if ( ((QString) inputUserPW1->password()).length() < 6 )
        {
            tabWidget->setCurrentPage( 3 );
            inputUserPW1->setFocus();
            error = i18n( "The specified User-Password is too short (min. 6 chars)" );
        }
        else if ( ((QString) inputUserPW1->password()).length() > 20 )
        {
            tabWidget->setCurrentPage( 3 );
            inputUserPW1->setFocus();
            error = i18n( "The specified User-Password is too long (max. 20 chars)" );
        }
        else if ( inputUserName->text().length() < 1 )
        {
            tabWidget->setCurrentPage( 3 );
            inputUserName->setFocus();
            error = i18n( "You must enter a Username!" );
        }
        else if ( inputHostName->text().length() < 1 )
        {
            tabWidget->setCurrentPage( 4 );
            inputHostName->setFocus();
            error = i18n( "You must enter a Hostname!" );
        }
        else if ( inputRootPartition->currentText().length() < 1 )
        {
            tabWidget->setCurrentPage( 1 );
            inputRootPartition->setFocus();
            error = i18n( "You must select a Root-Partition!\n\nIf there is nothing to choose you must create a Linux-Partition first..." );
        }
        
        if ( error != "" )
        {
            cmdNext->setText( i18n( "Next" ) );
            QMessageBox::critical( this, i18n( "Error" ), error );
            return;
        }
        }
        
        // Passwörter verschlüsseln oder geladene Passwörter verwenden
        QString strRootPW; QString strUserPW;
        if( inputRootPW1->password() == QString("******") && rootpass_crypt )
            { strRootPW = rootpass_crypt; }
        else
            { strRootPW = encrypt( inputRootPW1->password() ); }

        if( inputUserPW1->password() == QString("******") && userpass_crypt )
            { strUserPW = userpass_crypt; }
        else
            { strUserPW = encrypt( inputUserPW1->password() ); }

        // Services-Default-Wert
        if(!services_start) services_start = "kdm cupsys";
        
        // Datei .knofig erzeugen
        // erstmal temporär, da nicht sicher ist ob wir Schreibrecht für /home/knoppix haben
        // wird später aber dannn per sudo nach /home/knoppix/.knofig verschoben
        fstream f;
        f.open("/tmp/.knofig", ios::out);
f << "REGISTERED=' SYSTEM_MODULE SYSTEM_TYPE HD_MODULE HD_FORMAT HD_FSTYPE HD_CHOICE HD_MAP HD_IGNORECHECK SWAP_MODULE SWAP_AUTODETECT SWAP_CHOICES NAME_MODULE NAME_NAME USER_MODULE USER_NAME USERPASS_MODULE USERPASS_CRYPT ROOTPASS_MODULE ROOTPASS_CRYPT HOST_MODULE HOST_NAME SERVICES_MODULE SERVICES_START BOOT_MODULE BOOT_LOADER BOOT_DISK BOOT_WHERE INSTALL_READY'" << endl << endl;

f << "SYSTEM_MODULE='configured'" << endl;
f << "# Determines if the system is a debian-derivate, a copy from CD or a nice mix of both" << endl
<< "# Possible are: knoppix|debian" << endl
<< "# Default value is: debian" << endl;
f << "SYSTEM_TYPE='debian'" << endl << endl;

f << "HD_MODULE='configured'" << endl;
f << "# Determines if the HD should be formatted. (mkfs.*)" << endl
<< "# Possible are: yes|no" << endl
<< "# Default value is: yes" << endl;
f << "HD_FORMAT='"; if( chkRootFormat->isChecked() ) f << "yes"; else f << "no"; f << "'" << endl;
f << "# Sets the Filesystem type." << endl
<< "# Possible are: ext3|ext2|reiserfs|reiser4|xfs|jfs" << endl
<< "# Default value is: reiserfs" << endl;
f << "HD_FSTYPE='" << inputRootFormat->currentText() << "'" << endl;
f << "# Here the Kanotix-System will be installed" << endl
<< "# This value will be checked by function module_hd_check" << endl;
f << "HD_CHOICE='" << inputRootPartition->currentText() << "'" << endl;
f << "# Here you can give additional mappings. (Experimental) You need to have the partitions formatted yourself and give the correct mappings like: '/dev/hda4:/boot /dev/hda5:/var /dev/hda6:/tmp'" << endl;
f << "HD_MAP='";
if( chkMountpoints->isChecked() )
for ( int j = 0; j < partTable->numRows(); j++ )
{
    // Nur wenn die Länge > 1, da so das root-Verzeichnis ausgelassen wird ("/")
    if ( partTable->text( j, 1 ).length() > 1 )
    {
        f << partTable->text( j, 0 ) << ":";
        f << partTable->text( j, 1 ) << " ";
    }
}
f << "'" << endl;
f << "# If set to yes, the program will NOT check if there is enough space to install knoppix on the selected partition(s). Use at your own risk! Useful for example with HD_MAP if you only have a small root partition." << endl
<< "# Possible are: yes|no" << endl
<< "# Default value is: no" << endl;
f << "HD_IGNORECHECK='"; if( chkMountpoints->isChecked() ) f << "yes"; else f << "no"; f << "'" << endl << endl;

f << "SWAP_MODULE='configured'" << endl;
f << "# If set to yes, the swap partitions will be autodetected." << endl
<< "# Possible are: yes|no" << endl
<< "# Default value is: yes" << endl;
f << "SWAP_AUTODETECT='yes'" << endl;
f << "# The swap partitions to be used by the installed knoppix." << endl
<< "# This value will be checked by function module_swap_check" << endl;
f << "SWAP_CHOICES=''" << endl << endl;

f << "NAME_MODULE='configured'" << endl;
f << "NAME_NAME='" << inputName->text() << "'" << endl << endl;

f << "USER_MODULE='configured'" << endl;
f << "USER_NAME='" << inputUserName->text() << "'" << endl << endl;

f << "USERPASS_MODULE='configured'" << endl;
f << "USERPASS_CRYPT='" << strUserPW << "'" << endl;

f << "ROOTPASS_MODULE='configured'" << endl;
f << "ROOTPASS_CRYPT='" << strRootPW << "'" << endl;

f << "HOST_MODULE='configured'" << endl;
f << "HOST_NAME='" << inputHostName->text() << "'" << endl;

f << "SERVICES_MODULE='configured'" << endl;
f << "# Possible services are for now: kdm cupsys smail ssh samba" << endl
<< "# Default value is: kdm cupsys" << endl;
f << "SERVICES_START='" << services_start << "'" << endl << endl;

f << "BOOT_MODULE='configured'" << endl;
f << "# Chooses the Boot-Loader" << endl
<< "# Possible are: lilo|grub" << endl
<< "# Default value is: grub" << endl;
f << "BOOT_LOADER='" << inputBootmanager->currentText().lower() << "'" << endl;
f << "# If set to 'yes' a boot disk will be created!" << endl
<< "# Possible are: yes|no" << endl
<< "# Default value is: yes" << endl;
f << "BOOT_DISK='"; if( inputBootDisk->isChecked() ) f << "yes"; else f << "no"; f << "'" << endl;
f << "# Where the Boot-Loader will be installed" << endl
<< "# Possible are: mbr|partition" << endl
<< "# Default value is: mbr" << endl;
f << "BOOT_WHERE='"; if( inputBootOrt->currentItem() == 1 ) f << "partition"; else f << "mbr"; f << "'" << endl << endl;
f << "AUTOLOGIN_MODULE='configured'" << endl;
f <<  "# If set to yes, you'll be logged in automatically without entering a password." << endl
<< "# Possible are: yes|no" << endl
<< "# Default value is: no" << endl;
f << "AUTOLOGIN='"; if( chkAutoLogin->isChecked() ) f << "yes"; else f << "no"; f << "'" << endl << endl;
f << "INSTALL_READY='yes'" << endl;

        f.close();
        
        if ( doChecks ) {
        // Die Konfigurationsdatei auf Fehler überprüfen
        shell->setCommand( "kanotix-check-knofig /tmp/.knofig" );
        shell->start( true );
        
        bool isFailure = false;
        
        // Jede Zeile der Ausgabe verarbeiten
        QStringList knofigCheck = QStringList::split( "\n", shell->getBuffer() );
        for ( QStringList::Iterator it = knofigCheck.begin(); it != knofigCheck.end(); ++it ) {
            QString row = *it;
            if ( row.startsWith( "ERROR: " ) )
            {
                QMessageBox::warning( this, i18n( "Error" ), row.mid( 7 ) );
                isFailure = true;
            }
            if ( row.startsWith( "TAB: " ) )
            {
                tabWidget->setCurrentPage( row.mid( 5 ).toInt() );
            }
            cout << row << endl;
        }
        
        // Wenn Fehler aufgetreten, dann Funktion beenden
        if ( isFailure ) return;
        }

        // Die Konfigurationsdatei nach $HOME verschieben
        shell->setCommand( "mv /tmp/.knofig $HOME/.knofig" );
        shell->start(true);
        
        if ( buttonGroup1->selectedId() == 0 )
        {
            if ( QMessageBox::question( this, i18n( "Begin Installation" ), i18n( "The configuration has been saved successfully.\n"
"Do you really want to start the Installation now?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
            {
                shell->setCommand( "knxinstaller-script" );
                shell->start( false );
                exit( 0 );
            }
        }
        else if ( buttonGroup1->selectedId() == 1 )
        {
            if ( QMessageBox::question( this, i18n( "Begin Installation" ), i18n( "The configuration has been saved successfully.\n"
"Do you want to see/manually edit it?" ), QMessageBox::Yes, QMessageBox::No ) == QMessageBox::Yes )
            {
                shell->setCommand( "kwrite $HOME/.knofig" );
                shell->start( false );
                exit( 0 );
            }
        }
        else
        {
            exit(0);
        }
    }
    }
    
}

void knxinstallerWidget::cmdBack_clicked()
{
    if ( tabWidget->currentPageIndex() > 0 )
    tabWidget->setCurrentPage(tabWidget->currentPageIndex()-1);
}

void knxinstallerWidget::getOutput(KProcess *, char *data, int len)
{
    char dst[len+1];
    memmove(dst,data,len);
    dst[len]=0;
    
    processOutput = data;
}

void knxinstallerWidget::installerType_changed()
{
    cmdNext->setText( i18n( "Begin installation" ) );
    cmdNext->setEnabled( true );
    cmdNext->setDefault( true );
}

void knxinstallerWidget::tab_changed(QWidget* widget)
{
    int id = tabWidget->indexOf( widget );
    if ( id < 1 )
    {
        cmdBack->setEnabled( false );
    }
    else
    {
        cmdBack->setEnabled( true );
    }
    if ( id > tabWidget->count()-2 )
    {
        if ( bgInstallmode->selectedId() == 2 )
        {
            cmdNext->setText( i18n( "Begin Recovery" ) );
        }
        else if ( bgInstallmode->selectedId() == 1 )
        {
            cmdNext->setText( i18n( "Begin Update" ) );
        }
        else if ( bgInstallmode->selectedId() == 0 )
        {
            if ( buttonGroup1->selectedId() < 0 )
            {
                cmdNext->setEnabled( false );
            }
            else
            {
                cmdNext->setText( i18n( "Begin installation" ) );
                cmdNext->setEnabled( true );
            }
        }
    }
    else
    {
        cmdNext->setText( i18n( "Next" ) );
        cmdNext->setEnabled( true );
        cmdNext->setDefault( true );
    }
}


void knxinstallerWidget::startParted()
{
    ExecProcess *shell = new ExecProcess();
    shell->setCommand("gparted");
    shell->start(false);
}

void knxinstallerWidget::rereadFstab()
{
    ExecProcess *shell = new ExecProcess();
    shell->setCommand( "rebuildfstab" );
    shell->start( true );

    readFstab();
}

void knxinstallerWidget::partMan_exited(bool changed)
{
    if ( changed )
    {
        if ( QMessageBox::information(NULL, "Kanotix-Installer", i18n("The Partition-Table has been changed.\nDo you want to rebuild the FStab now?"), QMessageBox::Yes | QMessageBox::Default, QMessageBox::No | QMessageBox::Escape) == QMessageBox::Yes )
        {
            rereadFstab();
        }
    }
    this->topLevelWidget()->show();
    this->topLevelWidget()->move(this->window_position);
}

void knxinstallerWidget::rootPart_changed()
{
    for ( int j = 0; j < partTable->numRows(); j++ )
    {
        if ( partTable->text( j, 1 ) == "/" )
        {
            partTable->setText( j, 1, "" );
        }
        if ( partTable->text( j, 0 ) == inputRootPartition->currentText() )
        {
            partTable->setText( j, 1, "/" );
        }
    }
}

void knxinstallerWidget::installationMode_changed(int option)
{
    if( option == 0 )
    {
        if ( bgInstallmode->selectedId() == 2 )
        {
            tabWidget->removePage(tabPartitions);
            tabWidget->removePage(tabGrub);
            tabWidget->removePage(tabUser);
            tabWidget->removePage(tabNetwork);
            tabWidget->removePage(tabInstallation);
            tabWidget->removePage(tabUpdate);
            tabWidget->insertTab(tabRecover, "");
        }
        else if ( bgInstallmode->selectedId() == 1 )
        {
            tabWidget->insertTab(tabUpdate, "");
            tabWidget->removePage(tabPartitions);
            tabWidget->removePage(tabGrub);
            tabWidget->removePage(tabUser);
            tabWidget->removePage(tabNetwork);
            tabWidget->removePage(tabInstallation);
            tabWidget->removePage(tabRecover);
            
            // Die Exclude-Liste in das Textfeld laden
            ExecProcess *shell = new ExecProcess();
            shell->setCommand( "kanotix-update -s" );
            shell->start( true );

            txtExclude->setText( shell->getBuffer() );
        }
        else if ( bgInstallmode->selectedId() == 0 )
        {
            tabWidget->removePage(tabUpdate);
            tabWidget->removePage(tabRecover);
            tabWidget->insertTab(tabPartitions, "");
            tabWidget->insertTab(tabGrub, "");
            tabWidget->insertTab(tabUser, "");
            tabWidget->insertTab(tabNetwork, "");
            tabWidget->insertTab(tabInstallation, "");
        }

        this->languageChange();
        this->readFstab();
    }
}









/*$SPECIALIZATION$*/



#include "knxinstallerwidget.moc"

