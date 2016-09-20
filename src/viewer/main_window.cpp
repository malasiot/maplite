#include <QDesktopServices>
#include <QDirIterator>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QSettings>
#include <QApplication>
#include <QClipboard>
#include <QToolBar>
#include <QDebug>
#include <QFileDialog>
#include <QDockWidget>
#include <QStatusBar>
#include <QGraphicsView>
#include <QVBoxLayout>
#include <QLabel>

#include <fstream>
#include <sstream>

#include "main_window.hpp"
#include "map_widget.hpp"
#include "map_view.hpp"
#include "map_server.hpp"
#include "logger.hpp"

using namespace std ;

extern QStringList application_data_dirs_ ;

MainWindow *MainWindow::instance_ = 0 ;

MainWindow::~MainWindow() {}

MainWindow::MainWindow(int &argc, char *argv[])
{
    instance_ = this ;

    initBasemaps() ;

    readAppSettings() ;

    //createTools() ;
    createWidgets() ;

    readGuiSettings();
/*
    parseArguments(argc, argv) ;

    feature_index_ = new MapFeatureIndex(this) ;

    QString feature_index_path = QDesktopServices::storageLocation(QDesktopServices::DataLocation)  ;
    QDir(feature_index_path).mkpath(".") ;

    if ( !feature_index_->open(feature_index_path + "/features") )
    {
        delete feature_index_ ;
        feature_index_ = 0 ;
    }

    current_folder_id_ = 1 ;
    current_collection_id_ = 0 ;

    undo_group_ = new QUndoGroup(this) ;

    initBasemaps() ;

    readAppSettings() ;

    createTools() ;
    createWidgets() ;

    createActions() ;
    createMenus() ;
    createToolBars() ;
    createDocks() ;


    readGuiSettings();
    */
}


bool MainWindow::parseArguments(int &argc, char *argv[])
{
    return true ;
}

void MainWindow::createTools()
{
    /*
    pan_tool_ = new PanTool(this) ;
    zoom_tool_ = new ZoomTool(this) ;
    polygon_tool_ = new PolygonTool(this) ;
    waypoint_tool_ = new PointTool(this) ;
    select_tool_ = new FeatureSelectTool(this) ;
    edit_tool_ = new FeatureEditTool(this) ;



    undo_group_->addStack(((PolygonTool *)polygon_tool_)->undo_stack_) ;
    undo_group_->addStack(((FeatureEditTool *)edit_tool_)->undo_stack_) ;
    */
}

void MainWindow::createDocks()
{
    /*
    feature_library_dock_ = new QDockWidget(this) ;
    feature_library_dock_->setObjectName("fldock") ;

    QSplitter *splitter = new QSplitter(feature_library_dock_) ;
    splitter->setOrientation(Qt::Vertical) ;
    splitter->setHandleWidth(2);

    feature_library_view_ = new FeatureLibraryView(feature_index_, feature_library_dock_);
    feature_list_view_ = new FeatureListView(feature_index_, feature_library_dock_);
    splitter->addWidget(feature_library_view_);
    splitter->addWidget(feature_list_view_);

    connect(feature_library_view_, SIGNAL(collectionClicked(quint64, quint64)), this, SLOT(onCollectionSelected(quint64, quint64))) ;
    connect(feature_library_view_, SIGNAL(folderClicked(quint64)), this, SLOT(onFolderSelected(quint64))) ;
    connect(feature_library_view_, SIGNAL(collectionClicked(quint64, quint64)), feature_list_view_, SLOT(populate(quint64, quint64))) ;
    connect(feature_library_view_, SIGNAL(zoomOnRect(QRectF)), map_widget_, SLOT(zoomToRect(QRectF))) ;
    connect(feature_list_view_, SIGNAL(featuresSelected(QVector<quint64>)), map_widget_, SLOT(selectFeatures(QVector<quint64>))) ;
    connect(feature_list_view_, SIGNAL(zoomOnRect(QRectF)), map_widget_, SLOT(zoomToRect(QRectF))) ;
    connect(select_tool_, SIGNAL(featureClicked(quint64)), feature_library_view_, SLOT(onFeatureClicked(quint64))) ;
    connect(feature_library_view_->model(), SIGNAL(dataChanged(QModelIndex,QModelIndex)), map_widget_, SLOT(invalidateOverlay())) ;

    feature_library_dock_->setWidget(splitter) ;


    addDockWidget(Qt::LeftDockWidgetArea, feature_library_dock_);
    */
}

void MainWindow::createWidgets()
{

    view_ = new MapView(this) ;


    map_widget_ = new MapWidget() ;

    scene_.addItem(map_widget_) ;
    scene_.setActiveWindow(map_widget_) ;

    view_->setScene(&scene_);
    view_->setFrameShape(QFrame::NoFrame);
    view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view_->setMainWidget(map_widget_);

    setCentralWidget(view_);

    status_bar_ = new QStatusBar(this) ;
    setStatusBar(status_bar_) ;

    // Do multipart status bar
    status_coords_ = new QLabel("", this);
    status_coords_->setAlignment(Qt::AlignLeft);
    status_coords_->setFixedWidth(200) ;
    status_middle_ = new QLabel("", this);
    status_bar_->addWidget(status_coords_, 0);
    status_bar_->addWidget(status_middle_, 0);

}

Q_DECLARE_METATYPE(MapTool *)

void MainWindow::createActions()
{
    /*
    // files

    import_files_action_ = new QAction(tr("Import files ..."), this);
    connect(import_files_action_, SIGNAL(triggered()), this, SLOT(importFiles()));

    // maps
    maps_actions_ = new QActionGroup(this) ;

    Q_FOREACH(QSharedPointer<TileProvider> p, base_maps_)
    {
        QAction *mapAct = new QAction(p->description(), this);
        connect(mapAct, SIGNAL(triggered()), this, SLOT(baseMapChanged()));
        mapAct->setCheckable(true);
        mapAct->setProperty("id", p->name()) ;

        if ( p->name() == default_map_ )
            mapAct->setChecked(true) ;

        maps_actions_->addAction(mapAct) ;
    }

    map_tools_actions_ = new QActionGroup(this) ;

    QAction *panToolAct = new QAction(QIcon(":/images/pan-tool.png"), tr("Pan map view"), this);
    connect(panToolAct, SIGNAL(triggered()), this, SLOT(toolChanged()));
    panToolAct->setCheckable(true) ;
    panToolAct->setChecked(true) ;
    panToolAct->setData(QVariant::fromValue<MapTool *>(pan_tool_)) ;
    map_tools_actions_->addAction(panToolAct) ;

    QAction *zoomToolAct = new QAction(QIcon(":/images/zoom-rect.png"), tr("Zoom to Rectangle"), this);
    connect(zoomToolAct, SIGNAL(triggered()), this, SLOT(toolChanged()));
    zoomToolAct->setCheckable(true);
    zoomToolAct->setData(QVariant::fromValue<MapTool *>(zoom_tool_)) ;
    map_tools_actions_->addAction(zoomToolAct) ;

    edit_tool_act_ = new QAction(QIcon(":/images/feature-edit.png"), tr("Edit feature"), this);
    connect(edit_tool_act_, SIGNAL(triggered()), this, SLOT(toolChanged()));
    edit_tool_act_->setCheckable(true);
    edit_tool_act_->setData(QVariant::fromValue<MapTool *>(edit_tool_)) ;
    edit_tool_act_->setEnabled(true) ;
    map_tools_actions_->addAction(edit_tool_act_) ;

    select_tool_act_ = new QAction(QIcon(":/images/feature-edit.png"), tr("Select feature"), this);
    connect(select_tool_act_, SIGNAL(triggered()), this, SLOT(toolChanged()));
    select_tool_act_->setCheckable(true);
    select_tool_act_->setData(QVariant::fromValue<MapTool *>(select_tool_)) ;
    select_tool_act_->setEnabled(true) ;
    map_tools_actions_->addAction(select_tool_act_) ;

    poly_tool_act_ = new QAction(QIcon(":/images/polygon-tool.png"), tr("Trace a track"), this);
    connect(poly_tool_act_, SIGNAL(triggered()), this, SLOT(toolChanged()));
    poly_tool_act_->setCheckable(true);
    poly_tool_act_->setData(QVariant::fromValue<MapTool *>(polygon_tool_)) ;
    poly_tool_act_->setDisabled(true) ;
    map_tools_actions_->addAction(poly_tool_act_) ;

    wpt_tool_act_ = new QAction(QIcon(":/images/flag-blue.png"), tr("Place a waypoint"), this);
    connect(wpt_tool_act_, SIGNAL(triggered()), this, SLOT(toolChanged()));
    wpt_tool_act_->setCheckable(true);
    wpt_tool_act_->setData(QVariant::fromValue<MapTool *>(waypoint_tool_)) ;
    wpt_tool_act_->setDisabled(true) ;
    map_tools_actions_->addAction(wpt_tool_act_) ;

    undo_act_ = new QAction(tr("Undo"), this) ;
    undo_act_->setShortcut(QKeySequence(tr("Ctrl+Z")));
    connect(undo_act_, SIGNAL(triggered()), undo_group_, SLOT(undo()));

    redo_act_ = new QAction(tr("Redo"), this) ;
    redo_act_->setShortcut(QKeySequence(tr("Shift+Ctrl+Z")));
    connect(redo_act_, SIGNAL(triggered()), undo_group_, SLOT(redo()));
*/
}

void MainWindow::createMenus()
{
    /*
    file_menu_ = menuBar()->addMenu(tr("File")) ;
    file_menu_->addAction(import_files_action_) ;

    connect(file_menu_, SIGNAL(aboutToShow()), this, SLOT(updateMenus())) ;

    edit_menu_ = menuBar()->addMenu(tr("Edit")) ;
    edit_menu_->addAction(undo_act_) ;
    edit_menu_->addAction(redo_act_) ;
    connect(edit_menu_, SIGNAL(aboutToShow()), this, SLOT(updateMenus())) ;

    maps_menu_ = menuBar()->addMenu(tr("Maps")) ;
    maps_menu_->addActions(maps_actions_->actions()) ;
    connect(maps_menu_, SIGNAL(aboutToShow()), this, SLOT(updateMenus())) ;
    */
}


void MainWindow::createToolBars()
{
    /*
    map_tool_bar_ = new QToolBar("MapTools") ;
    map_tool_bar_->setObjectName("MapToolsTB") ;

    map_tool_bar_->addActions(map_tools_actions_->actions());

    addToolBar(map_tool_bar_) ;
    */

}

void MainWindow::initBasemaps()
{


/*
    Q_FOREACH(QString folder, application_data_dirs_)
    {
        QDirIterator dit(folder, QDirIterator::Subdirectories);

        while (dit.hasNext()) {

            dit.next();

            if ( !dit.fileInfo().isDir() ) {

                QString filename = dit.fileName();

                if ( filename == "maps.json" ) {
                    QFile f(dit.filePath()) ;
                    if ( f.open(QFile::ReadOnly | QFile::Text) ) {
                        QTextStream strm(&f) ;
                        QString json = strm.readAll() ;

                        bool ok;
                        QtJson::JsonObject config = QtJson::parse(json.toUtf8().constData(), ok).toMap();

                        auto jmaps = config["maps"].toMap() ;

                        for( QString mid: jmaps.keys() ) {
                            auto map_config = jmaps[mid].toMap() ;
                            QString name = map_config["name"].toString() ;
                            QString source = map_config["source"].toString() ;

                            QDir src_dir(dit.fileInfo().absolutePath() + "/" + source ) ;

                            if ( src_dir.isReadable() ) {
                                base_maps_.insert(mid, name) ;
                                sources.insert(make_pair(mid.toAscii().constData(), src_dir.canonicalPath().toUtf8().constData())) ;
                            }
                        }

                        if(!ok) {
                            qFatal("An error occurred during parsing");
                        }
                    }
                }
                else if ( filename == "assets.sqlite" ) {
                    asset_source = dit.filePath().toUtf8().constData() ;
                }
            }
        }
    }
*/
    // error if assets or source empty


}

void MainWindow::readGuiSettings()
{
    QSettings settings ;

    restoreState(settings.value("gui/mainWinState").toByteArray()) ;
    QPoint p = settings.value("gui/pos", QPoint(0, 0)).toPoint() ;
    QSize sz = settings.value("gui/size", QSize(1000, 800)).toSize() ;
    move(p) ;
    resize(sz) ;

 //   feature_library_view_->restoreState(settings.value("gui/library_view_state").toByteArray()) ;

}

void MainWindow::writeGuiSettings()
{
    QSettings settings ;
    settings.setValue("gui/mainWinState", saveState()) ;
    settings.setValue("gui/pos", pos()) ;
    settings.setValue("gui/size", size()) ;

 //   settings.setValue("gui/library_view_state", feature_library_view_->saveState()) ;
}

void MainWindow::readAppSettings()
{
    QSettings settings ;

    default_zoom_ = settings.value("map/zoom", default_zoom_).toInt() ;
    default_center_ = settings.value("map/center", default_center_).toPointF() ;
    default_map_ = settings.value("map/name", default_map_).toString() ;
}

void MainWindow::writeAppSettings()
{
    QSettings settings ;
/*
    settings.setValue("map/zoom", map_widget_->getZoom()) ;
    settings.setValue("map/center", map_widget_->getCenter()) ;
    settings.setValue("map/name", default_map_) ;
    */
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    writeGuiSettings();
    writeAppSettings();

    event->accept();

    QClipboard *clipboard = QApplication::clipboard() ;
    clipboard->clear() ;
    QCoreApplication::quit() ;
}


void MainWindow::updateMenus()
{
//    undo_act_->setEnabled(undo_group_->canUndo()) ;
//    redo_act_->setEnabled(undo_group_->canRedo()) ;
}


void MainWindow::baseMapChanged()
{
    /*
    QAction *act =  dynamic_cast<QAction *>(sender());

    QString name = act->property("id").toString() ;

    map_widget_->setBasemap(base_maps_[name]) ;
    map_widget_->update() ;

    default_map_ = name ;
    */
}

void MainWindow::toolChanged()
{
    /*
    QAction *act =  dynamic_cast<QAction *>(sender());

    MapTool *tool = act->data().value<MapTool *>() ;

    map_widget_->setTool(tool) ;
*/
}

void MainWindow::importFiles()
{
    /*
    QSettings sts ;

    QString directory = sts.value("gui/lastImportDir").toString() ;

    QStringList file_names = QFileDialog::getOpenFileNames( this, tr("Import Files"), directory,
                                                            tr("All Supported formats (*.gpx *.kml *.kmz *.jpeg *.jpg*);; GPS Exchange Files (*.gpx) ;; Google Maps Files (*.kml *kmz) ;; Geotagged photos (*.jpeg *jpg)"));

    if ( !file_names.empty() )
    {
        sts.setValue("gui/lastImportDir", QFileInfo(file_names.at(0)).absolutePath()) ;
    }


    FileImportDialog dlg(file_names, current_folder_id_, feature_index_, 0) ;
    dlg.exec() ;

    Q_FOREACH(CollectionData *col, dlg.collections_)
    {
        if ( col ) feature_library_view_->addCollection(col) ;
        delete col ;
    }

    Q_FOREACH(CollectionTreeNode *col, dlg.documents_)
    {
        if ( col ) feature_library_view_->addCollectionTree(col) ;
        delete col ;
    }
*/
}

static const char DEG_SIM = 176 ;

static void dms(double ang, int &d, int &m, double &s)
{
    d = ang;

    double r1 = ang - d;
    m = r1*60.0;
    double r2 = r1 - m/60.0;
    s = r2*3600.0;
}

static QString dms(const QPointF &val, unsigned int num_dec_places = 2)
{
    QChar degree_ch(0x00B0) ;
    int d, m ;
    double s ;

    dms(val.y(), d, m, s) ;
    QString lat = QString("%1%2%3%4'%5''").arg((d<0)?'S':'N').arg(abs(d), 2, 10, QLatin1Char('0')).arg(degree_ch)
            .arg(m, 2, 10, QLatin1Char('0')).arg(s, 3+num_dec_places, 'f', num_dec_places, QLatin1Char('0')) ;

    dms(val.x(), d, m, s) ;
    QString lon = QString("%1%2%3%4'%5''").arg((d<0)?'W':'E').arg(abs(d), 2, 10, QLatin1Char('0')).arg(degree_ch)
            .arg(m, 2, 10, QLatin1Char('0')).arg(s, 3+num_dec_places, 'f', num_dec_places, QLatin1Char('0')) ;

    return (lat + "  " + lon ) ;
}


void MainWindow::displayCoords(const QPointF &coords)
{
    QString text = dms(coords) ;

    status_coords_->setText(dms(coords)) ;
}

