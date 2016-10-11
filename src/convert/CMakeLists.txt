SET ( OSM2MFTILES_SOURCES

	${SRC_ROOT}/convert/osm2mbtiles.cpp

	${SRC_ROOT}/convert/filter_config.cpp
	${SRC_ROOT}/convert/osm_rule_parser.cpp
	${SRC_ROOT}/convert/osm_filter_functions.cpp
	${SRC_ROOT}/convert/osm_processor.cpp
	${SRC_ROOT}/convert/osm_processor.hpp
	${SRC_ROOT}/convert/osm_polygon.cpp
	${SRC_ROOT}/convert/osm_pbf_reader.cpp
	${SRC_ROOT}/convert/osm_document.cpp
	${SRC_ROOT}/convert/filter_config.hpp
	${SRC_ROOT}/convert/osm_filter_functions.hpp
	${SRC_ROOT}/convert/osm_document.hpp
	${SRC_ROOT}/convert/osm_rule_parser.hpp
        ${SRC_ROOT}/convert/shp_processor.cpp
        ${SRC_ROOT}/convert/shp_processor.hpp


	${SRC_ROOT}/io/mapsforge_database.hpp
	${SRC_ROOT}/io/mapsforge_map_writer.cpp
	${SRC_ROOT}/io/mapsforge_map_reader.cpp
	${SRC_ROOT}/io/serializer.cpp
	${SRC_ROOT}/io/serializer.hpp

	${SRC_ROOT}/util/dictionary.cpp
	${SRC_ROOT}/util/xml_reader.cpp
	${SRC_ROOT}/util/database.cpp
	${SRC_ROOT}/util/zfstream.cpp
	${SRC_ROOT}/util/base64.cpp


	${SRC_ROOT}/util/tms.cpp
	${SRC_ROOT}/util/tms.hpp

	${SRC_ROOT}/util/dictionary.hpp
	${SRC_ROOT}/util/xml_reader.hpp
	${SRC_ROOT}/util/database.hpp
	${SRC_ROOT}/util/zfstream.hpp
	${SRC_ROOT}/util/base64.hpp
)

PROTOBUF_GENERATE_CPP(OSM_PROTO_SOURCES OSM_PROTO_HEADERS ${SRC_ROOT}/protobuf/osmformat.proto ${SRC_ROOT}/protobuf/fileformat.proto)
SET_SOURCE_FILES_PROPERTIES(${OSM_PROTO_SOURCES} ${OSM_PROTO_HEADERS} PROPERTIES GENERATED TRUE)

MESSAGE(${OSM_PROTO_SOURCES})
FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

FLEX_TARGET(OSM_FILTER_SCANNER ${SRC_ROOT}/convert/osm.l  ${SRC_ROOT}/convert/parser/osm_scanner.cpp)
BISON_TARGET(OSM_FILTER_PARSER ${SRC_ROOT}/convert/osm.y  ${SRC_ROOT}/convert/parser/osm_parser.cpp)

ADD_FLEX_BISON_DEPENDENCY(OSM_FILTER_SCANNER OSM_FILTER_PARSER COMPILE_FLAGS "-Wconflicts-sr")

LIST(APPEND OSM2MFTILES_SOURCES ${FLEX_OSM_FILTER_SCANNER_OUTPUTS} ${BISON_OSM_FILTER_PARSER_OUTPUTS}
	${OSM_PROTO_SOURCES} ${OSM_PROTO_HEADERS}
)

ADD_EXECUTABLE(osm2mf ${OSM2MFTILES_SOURCES})
TARGET_LINK_LIBRARIES(osm2mf ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES} ${SQLITE3_LIBRARY} ${SPATIALITE_LIBRARY} ${SHAPELIB_LIBRARY} ${Boost_LIBRARIES})

INSTALL(TARGETS osm2mf DESTINATION bin  )
