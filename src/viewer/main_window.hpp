#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QSet>
#include <QDir>

class QComboBox ;
class QStatusBar ;
class QLabel ;
class MapWidget ;
class MapTool ;
class TileProvider ;
class QActionGroup ;
class QDockWidget ;
class QUndoGroup ;
class QUndoStack ;
class FeatureLibraryView ;
class FeatureListView ;
class MapOverlayManager ;
class QDir ;

#include "map_manager.hpp"
#include "map_overlay.hpp"

#include "theme.hpp"
#include "geometry.hpp"

struct ThemeBinding {
    std::shared_ptr<RenderTheme> theme_ ;
    QString layer_ ;
};


class MainWindow : public QMainWindow
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);
    void closeBasemaps();

public:

    MainWindow(int &argc, char *argv[]);
    ~MainWindow()  {
    }


    static MainWindow *instance() { return instance_ ; }


public Q_SLOTS:

    void baseMapChanged() ;
    void updateMenus();
    void toolChanged();
    void importFiles() ;
    void onNewOverlay(const MapOverlayPtr &) ;
    void onCollectionSelected(quint64 collection_id, quint64 feature_id = 0) ;
    void onFolderSelected(quint64 folder_id) ;
    void displayCoords(const QPointF &coords) ;

Q_SIGNALS:


protected:

    bool parseArguments(int &argc, char *argv[]) ;

    void createTools() ;
    void createWidgets() ;
    void initMaps() ;
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDocks() ;

    void readGuiSettings();
    void writeGuiSettings();

    void readAppSettings() ;
    void writeAppSettings() ;

    MapWidget *map_widget_ ;
    MapTool *pan_tool_, *zoom_tool_, *polygon_tool_, *waypoint_tool_, *edit_tool_, *select_tool_;

    MapManager maps_ ;
    std::string default_theme_, default_layer_, default_map_ ;
    QPointF default_center_ ;
    int default_zoom_ ;

    QMenu *maps_menu_, *file_menu_, *edit_menu_ ;
    QToolBar *map_tool_bar_ ;
    QStatusBar *status_bar_ ;
    QLabel * status_coords_, * status_middle_ ;
    QDockWidget *feature_library_dock_ ;
    FeatureLibraryView *feature_library_view_ ;
    FeatureListView *feature_list_view_ ;

    QAction *poly_tool_act_, *wpt_tool_act_, *edit_tool_act_, *select_tool_act_ ;
    QActionGroup *maps_actions_, *map_tools_actions_ ;
    QAction *import_files_action_,  *undo_act_, *redo_act_ ;
    QUndoGroup *undo_group_ ;


    QSharedPointer<MapOverlayManager> overlay_manager_ ;

    quint64 current_folder_id_, current_collection_id_ ;

    static MainWindow *instance_ ;

};














#endif
