INCLUDE_DIRECTORIES(${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
#	${SQLITE3_INCLUDE_DIR}
	${SPATIALITE_INCLUDE_DIR}
	${ZLIB_INCLUDE_DIR}
	${Boost_INCLUDE_DIR}
	${PROTOBUF_INCLUDE_DIRS}
	${GLFW_INCLUDE_DIRS}
	${PNG_INCLUDE_DIRS}
	${CAIRO_INCLUDE_DIR}
	${FREETYPE_INCLUDE_DIRS}
#	${PROJ4_INCLUDE_DIR}
	${FONTCONFIG_INCLUDE_DIR}
	${PNG_INCLUDE_DIRS}
	${GEOS_INCLUDE_DIR}
	${LUA_INCLUDE_DIR}
#        ${SHAPELIB_INCLUDE_DIR}
	${CMAKE_CURRENT_SOURCE_DIR}/util
	${CMAKE_CURRENT_SOURCE_DIR}/server
	${CMAKE_CURRENT_SOURCE_DIR}/convert
	${CMAKE_CURRENT_SOURCE_DIR}/convert/parser
	${CMAKE_CURRENT_SOURCE_DIR}/renderer
	${CMAKE_CURRENT_SOURCE_DIR}/map
	${CMAKE_CURRENT_SOURCE_DIR}/io
	${CMAKE_CURRENT_SOURCE_DIR}/mesh
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/vcglib
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/triangle
	${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/pugixml
        ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/sqlite3/src/
)

IF ( WIN32 )
ADD_DEFINITIONS( -DBOOST_USE_WINDOWS_H -DYYDEBUG=1)
SET(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
ENDIF( WIN32 )

ADD_DEFINITIONS( -std=c++11 )

find_package(OpenMP)
if (OPENMP_FOUND)
	set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
	set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()

SET (SRC_ROOT ${CMAKE_CURRENT_SOURCE_DIR})

#ADD_SUBDIRECTORY(3rdparty/sqlite3)
ADD_SUBDIRECTORY(convert)
#ADD_SUBDIRECTORY(renderer)
#ADD_SUBDIRECTORY(server)
ADD_SUBDIRECTORY(tools)
#ADD_SUBDIRECTORY(viewer)
