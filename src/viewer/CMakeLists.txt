

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
        ${CMAKE_CURRENT_SOURCE_DIR}/spatialindex/include/
)

IF ( WIN32 )
ADD_DEFINITIONS( -DBOOST_USE_WINDOWS_H )
SET(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
ENDIF( WIN32 )


SET(QT_USE_QTNETWORK TRUE)
SET(QT_USE_QTXML TRUE)
INCLUDE(${QT_USE_FILE})

add_subdirectory(spatialindex)

SET(MAPLITE_VIEWER_SOURCES
	viewer.cpp
	MapWidget.cpp
	TileProvider.cpp
	MapFileTileProvider.cpp
	NetworkTileProvider.cpp
	XYZTileProvider.cpp
	MapTool.cpp
	MapFeature.cpp
	MapFeatureIndex.cpp
	KMLReader.cpp
	GPXReader.cpp
	FeatureLibraryPanel.cpp
	FileImportDialog.cpp
	MainWindow.cpp


	Projection.cpp

	Value.cpp

	GeometryUtil.cpp

        ${SRC_ROOT}/io/mapsforge_map_reader.cpp
        ${SRC_ROOT}/io/serializer.cpp
        ${SRC_ROOT}/util/dictionary.cpp
        ${SRC_ROOT}/util/xml_reader.cpp
        ${SRC_ROOT}/util/database.cpp
        ${SRC_ROOT}/util/zfstream.cpp
        ${SRC_ROOT}/util/base64.cpp

        minizip/unzip.c
        minizip/ioapi.c

)

SET(MAPLITE_VIEWER_HEADERS
	MapWidget.h
	TileProvider.h
	MapFileTileProvider.h
	NetworkTileProvider.h
	XYZTileProvider.h
	MapTool.h
	MapFeatureIndex.h
	MapFeatureCollection.h
	FeatureLibraryPanel.h
	FileImportDialog.h
	MainWindow.h
	DataImport.h

	Database.h
	zfstream.h
	XmlReader.h
	XmlDocument.h
	ParseUtil.h
	Cache.h
	Logger.h
	Value.h
	MapFile.h

	Renderer.h
	ImageBuffer.h

	GeometryUtil.h

	Style.h
	Feature.h
	Raster.h

	ResourceCache.h
	CollisionChecker.h

	svg/Util.h
	svg/Document.h
	svg/Rendering.h
)

SET(MAPLITE_VIEWER_FORMS )
SET(MAPLITE_VIEWER_RESOURCES mdi.qrc )

QT4_WRAP_CPP(MAPLITE_VIEWER_MOC_SOURCES ${MAPLITE_VIEWER_HEADERS})
QT4_WRAP_UI(MAPLITE_VIEWER_UI_HEADERS ${MAPLITE_VIEWER_FORMS} )
QT4_ADD_RESOURCES(MAPLITE_VIEWER_RC_SOURCES ${MAPLITE_VIEWER_RESOURCES} )

IF ( WIN32 )
ADD_EXECUTABLE(mapviewer WIN32 ${MAPLITE_VIEWER_SOURCES} ${MAPLITE_VIEWER_MOC_SOURCES} ${MAPLITE_VIEWER_RC_SOURCES})
ELSE ( WIN32 )
ADD_EXECUTABLE(mapviewer ${MAPLITE_VIEWER_SOURCES} ${MAPLITE_VIEWER_MOC_SOURCES} ${MAPLITE_VIEWER_RC_SOURCES})
ENDIF ( WIN32 )

TARGET_LINK_LIBRARIES(mapviewer
        mapsforge_renderer
        spatialindex
	${Boost_LIBRARIES} ${CAIRO_LIBRARY} ${FONTCONFIG_LIBRARIES}
	${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${PNG_LIBRARIES}
        ${SQLITE3_LIBRARY} ${PROJ4_LIBRARY} ${SPATIALITE_LIBRARY} ${ZLIB_LIBRARY}
        ${GEOS_LIBRARY}
	${QT_LIBRARIES})

INSTALL(TARGETS mapviewer DESTINATION bin)

ADD_EXECUTABLE(test_pois test_pois.cpp MapFile.cpp POI.cpp Database.cpp Projection.cpp Dictionary.cpp XmlDocument.cpp XmlReader.cpp)
TARGET_LINK_LIBRARIES(test_pois ${Boost_LIBRARIES} ${SQLITE3_LIBRARY} ${SPATIALITE_LIBRARY} ${PROJ4_LIBRARY} ${ZLIB_LIBRARY})
