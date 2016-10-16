#include "main_window.hpp"

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QPixmapCache>
#include <QDebug>

#include <stdexcept>
#include <iostream>

using namespace std ;

QStringList application_data_dirs_ ;

void setupDirs(int &argc, char **argv)
{
    // location of user data folder default
    QString userDataDir = QDesktopServices::storageLocation(QDesktopServices::DataLocation) ;

    // or provided in command line
    int i ;
    for(  i=1 ; i<argc ; i++ )
    {
        if ( strncmp(argv[i], "--data", 10) == 0)
        {
            if ( i+1 < argc )
            {
                application_data_dirs_ << argv[i+1];

                for(int j = i ; j<argc-2 ; j++)
                    argv[j] = argv[j+2] ;

                argc -= 2 ;
            }
        }
    }

    // determine application path

    QString nativeAppPath = QCoreApplication::applicationDirPath() ;
    QCoreApplication::addLibraryPath(nativeAppPath + "/plugins/") ;

    // detect if running from the source tree

    QDir rootPath(nativeAppPath) ;

    rootPath.cdUp() ;

    QString dataDir = rootPath.absolutePath() + "/data/" ;

    application_data_dirs_ << dataDir ;
    application_data_dirs_ << userDataDir ;

}


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("ITI");
    QCoreApplication::setApplicationName("maplite");

    setupDirs(argc, argv) ;

    MainWindow win(argc, argv);

    QPixmapCache::setCacheLimit(100*1024) ;

    win.show() ;

    return app.exec();

}
