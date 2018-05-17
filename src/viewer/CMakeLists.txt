

INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${SQLITE3_INCLUDE_DIR}
	${SPATIALITE_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${CAIRO_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
	${PROJ4_INCLUDE_DIR}
	${FONTCONFIG_INCLUDE_DIR}
	${PNG_INCLUDE_DIRS}
	${PROTOBUF_INCLUDE_DIRS}
        ${HARFBUZZ_INCLUDE_DIRS}
	${GEOS_INCLUDE_DIR}
        ${QT_INCLUDES}
        ${CMAKE_CURRENT_SOURCE_DIR}/spatialindex/include/
)

IF ( WIN32 )
ADD_DEFINITIONS( -DBOOST_USE_WINDOWS_H )
SET(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
ENDIF( WIN32 )


#SET(QT_USE_QTNETWORK TRUE)
#SET(QT_USE_QTXML TRUE)
#INCLUDE(${QT_USE_FILE})

add_subdirectory(spatialindex)

SET(MAPLITE_VIEWER_SOURCES
    viewer.cpp
    main_window.cpp
    map_widget.cpp
    tile_provider.cpp
    map_file_tile_provider.cpp
    network_tile_provider.cpp
	xyz_tile_provider.cpp
    map_overlay.cpp
    map_overlay_manager.cpp
    map_tool.cpp
    overlay_library_panel.cpp
    file_import_dialog.cpp
	map_manager.cpp
	popup.cpp
	overlay_import.cpp
    tile_cache.cpp

    ${SRC_ROOT}/io/mapsforge_map_reader.cpp
    ${SRC_ROOT}/io/serializer.cpp
    ${SRC_ROOT}/util/dictionary.cpp
    ${SRC_ROOT}/util/xml_reader.cpp
    ${SRC_ROOT}/util/database.cpp
    ${SRC_ROOT}/util/zfstream.cpp
    ${SRC_ROOT}/util/base64.cpp
    ${SRC_ROOT}/util/logger.cpp

    overlays/gps_overlays.cpp

    io/gpx_reader.cpp
    io/kml_reader.cpp
    io/minizip/unzip.c
    io/minizip/ioapi.c
)

SET(MAPLITE_VIEWER_HEADERS

    main_window.hpp
    map_widget.hpp
    tile_provider.hpp
    xyz_tile_provider.hpp
    map_file_tile_provider.hpp
    network_tile_provider.hpp
    map_overlay.hpp
    map_overlay_manager.hpp
    map_tool.hpp
    overlay_library_panel.hpp
    file_import_dialog.hpp
    data_import.hpp
    map_manager.hpp
    popup.hpp
    overlay_import.hpp
    tile_cache.hpp

    overlays/gps_overlays.hpp

    io/gpx_reader.hpp
    io/kml_reader.hpp
)

SET(MAPLITE_VIEWER_FORMS )
SET(MAPLITE_VIEWER_RESOURCES mdi.qrc )
set(CMAKE_AUTOMOC ON)
#QT4_WRAP_CPP(MAPLITE_VIEWER_MOC_SOURCES ${MAPLITE_VIEWER_HEADERS})
#QT4_WRAP_UI(MAPLITE_VIEWER_UI_HEADERS ${MAPLITE_VIEWER_FORMS} )
#QT4_ADD_RESOURCES(MAPLITE_VIEWER_RC_SOURCES ${MAPLITE_VIEWER_RESOURCES} )

IF ( WIN32 )
ADD_EXECUTABLE(mapviewer WIN32 ${MAPLITE_VIEWER_SOURCES} ${MAPLITE_VIEWER_MOC_SOURCES} ${MAPLITE_VIEWER_RC_SOURCES})
ELSE ( WIN32 )
ADD_EXECUTABLE(mapviewer ${MAPLITE_VIEWER_SOURCES} ${MAPLITE_VIEWER_MOC_SOURCES} ${MAPLITE_VIEWER_RC_SOURCES})
ENDIF ( WIN32 )

QT5_USE_MODULES(mapviewer Core Widgets Network Xml)

TARGET_LINK_LIBRARIES(mapviewer
        mapsforge_renderer
        spatialindex
	${Boost_LIBRARIES} ${CAIRO_LIBRARY} ${FONTCONFIG_LIBRARIES}
	${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${PNG_LIBRARIES}
        sqlite3 ${PROJ4_LIBRARY} ${SPATIALITE_LIBRARY} ${ZLIB_LIBRARY}
        ${GEOS_LIBRARY}
        dl
        )



INSTALL(TARGETS mapviewer DESTINATION bin)

#ADD_EXECUTABLE(test_pois test_pois.cpp MapFile.cpp POI.cpp Database.cpp Projection.cpp Dictionary.cpp XmlDocument.cpp XmlReader.cpp)
#TARGET_LINK_LIBRARIES(test_pois ${Boost_LIBRARIES} ${SQLITE3_LIBRARY} ${SPATIALITE_LIBRARY} ${PROJ4_LIBRARY} ${ZLIB_LIBRARY})
