#ifndef __MAIN_WINDOW_H__
#define __MAIN_WINDOW_H__

#include <QMainWindow>
#include <QMap>
#include <QList>
#include <QSet>
#include <QGraphicsScene>

#include <memory>

class QComboBox ;
class QStatusBar ;
class QLabel ;
class MapView ;
class MapWidget ;
class MapTool ;
class TileProvider ;
class QActionGroup ;
class QDockWidget ;
class QUndoGroup ;
class QUndoStack ;
class FeatureLibraryView ;
class FeatureListView ;
class MapFeatureIndex ;

class MapServer ;
class QGraphicsScene ;


class MainWindow : public QMainWindow
{
    Q_OBJECT

    void closeEvent(QCloseEvent *event);

public:

    MainWindow() ;
    ~MainWindow() ;

    MainWindow(int &argc, char *argv[]);

    static MainWindow *instance() { return instance_ ; }

public Q_SLOTS:

    void baseMapChanged() ;
    void updateMenus();
    void toolChanged();
    void importFiles() ;

    void displayCoords(const QPointF &coords) ;

Q_SIGNALS:


protected:

    bool parseArguments(int &argc, char *argv[]) ;

    void createTools() ;
    void createWidgets() ;
    void initBasemaps() ;
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createDocks() ;

    void readGuiSettings();
    void writeGuiSettings();

    void readAppSettings() ;
    void writeAppSettings() ;

    Q_DISABLE_COPY(MainWindow)

    QMap<QString, QString> base_maps_ ;
    std::shared_ptr<MapServer> map_server_ ;

    QGraphicsScene scene_ ;
    MapView *view_ ;
    MapWidget *map_widget_ ;


    MapTool *pan_tool_, *zoom_tool_, *polygon_tool_, *waypoint_tool_, *edit_tool_, *select_tool_;

    QMenu *maps_menu_, *file_menu_, *edit_menu_ ;
    QToolBar *map_tool_bar_ ;
    QStatusBar *status_bar_ ;
    QLabel * status_coords_, * status_middle_ ;
    QDockWidget *feature_library_dock_ ;

    QAction *poly_tool_act_, *wpt_tool_act_, *edit_tool_act_, *select_tool_act_ ;
    QActionGroup *maps_actions_, *map_tools_actions_ ;
    QAction *import_files_action_,  *undo_act_, *redo_act_ ;
    QUndoGroup *undo_group_ ;

    QPointF default_center_ ;
    QString default_map_ ;
    int default_zoom_ ;

    MapFeatureIndex *feature_index_ ;

    quint64 current_folder_id_, current_collection_id_ ;

    static MainWindow *instance_ ;

};














#endif
