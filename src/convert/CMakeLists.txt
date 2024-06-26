SET ( OSM2MAP_SOURCES

	${SRC_ROOT}/convert/osm2map.cpp


	${SRC_ROOT}/convert/tag_filter.cpp
	${SRC_ROOT}/convert/tag_filter.hpp

	${SRC_ROOT}/convert/tag_filter_context.cpp
	${SRC_ROOT}/convert/tag_filter_context.hpp

	${SRC_ROOT}/convert/tag_filter_config_parser.hpp
	${SRC_ROOT}/convert/tag_filter_config_parser.cpp

	${SRC_ROOT}/convert/tag_filter_rule.hpp
	${SRC_ROOT}/convert/tag_filter_rule.cpp

	${SRC_ROOT}/convert/osm_processor.cpp
	${SRC_ROOT}/convert/osm_processor.hpp

	${SRC_ROOT}/convert/geom_utils.cpp
	${SRC_ROOT}/convert/geom_utils.hpp


	${SRC_ROOT}/convert/osm_polygon.cpp

	${SRC_ROOT}/convert/osm_pbf_reader.cpp
	${SRC_ROOT}/convert/osm_pbf_reader.hpp
	${SRC_ROOT}/convert/osm_xml_reader.cpp
	${SRC_ROOT}/convert/osm_xml_reader.hpp

        ${SRC_ROOT}/convert/osm_document.cpp
        ${SRC_ROOT}/convert/osm_document.hpp

        ${SRC_ROOT}/convert/osm_storage.hpp

        ${SRC_ROOT}/convert/osm_storage_memory.hpp

	${SRC_ROOT}/convert/lua_context.hpp
	${SRC_ROOT}/convert/lua_context.cpp

	${SRC_ROOT}/io/mapsforge_map_writer.hpp
	${SRC_ROOT}/io/mapsforge_map_writer.cpp

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
	${SRC_ROOT}/util/progress_stream.hpp
	${SRC_ROOT}/util/progress_stream.cpp

        ${SRC_ROOT}/util/xml_pull_parser.hpp
        ${SRC_ROOT}/util/xml_pull_parser.cpp

        ${SRC_ROOT}/convert/osm_storage_memory.hpp
        ${SRC_ROOT}/convert/osm_storage_db.hpp
        ${SRC_ROOT}/convert/osm_storage_db.cpp


)

SET ( OSM2POI_SOURCES

        ${SRC_ROOT}/convert/osm2poi.cpp

        ${SRC_ROOT}/convert/tag_filter.cpp
        ${SRC_ROOT}/convert/tag_filter.hpp

        ${SRC_ROOT}/convert/tag_filter_context.cpp
        ${SRC_ROOT}/convert/tag_filter_context.hpp

        ${SRC_ROOT}/convert/tag_filter_config_parser.hpp
        ${SRC_ROOT}/convert/tag_filter_config_parser.cpp

        ${SRC_ROOT}/convert/tag_filter_rule.hpp
        ${SRC_ROOT}/convert/tag_filter_rule.cpp

        ${SRC_ROOT}/convert/poi_processor.cpp
        ${SRC_ROOT}/convert/poi_processor.hpp

        ${SRC_ROOT}/convert/geom_utils.cpp
        ${SRC_ROOT}/convert/geom_utils.hpp

        ${SRC_ROOT}/convert/osm_polygon.cpp

        ${SRC_ROOT}/convert/osm_pbf_reader.cpp
        ${SRC_ROOT}/convert/osm_pbf_reader.hpp
        ${SRC_ROOT}/convert/osm_xml_reader.cpp
        ${SRC_ROOT}/convert/osm_xml_reader.hpp

        ${SRC_ROOT}/convert/osm_storage_memory.hpp
        ${SRC_ROOT}/convert/osm_storage_db.hpp
        ${SRC_ROOT}/convert/osm_storage_db.cpp

        ${SRC_ROOT}/convert/osm_document.cpp
        ${SRC_ROOT}/convert/osm_document.hpp

        ${SRC_ROOT}/convert/lua_context.hpp
        ${SRC_ROOT}/convert/lua_context.cpp

        ${SRC_ROOT}/io/mapsforge_poi_writer.hpp
        ${SRC_ROOT}/io/mapsforge_poi_writer.cpp

        ${SRC_ROOT}/io/poi_categories.hpp
        ${SRC_ROOT}/io/poi_categories.cpp

        ${SRC_ROOT}/3rdparty/pugixml/pugixml.cpp

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
        ${SRC_ROOT}/util/progress_stream.hpp
        ${SRC_ROOT}/util/progress_stream.cpp

        ${SRC_ROOT}/util/xml_pull_parser.hpp
        ${SRC_ROOT}/util/xml_pull_parser.cpp

        ${SRC_ROOT}/util/text_util.cpp
        ${SRC_ROOT}/util/text_util.hpp
)

SET ( OSM2SQLITE_SOURCES

        ${SRC_ROOT}/convert/osm2sqlite.cpp


        ${SRC_ROOT}/convert/tag_filter.cpp
        ${SRC_ROOT}/convert/tag_filter.hpp

        ${SRC_ROOT}/convert/tag_filter_context.cpp
        ${SRC_ROOT}/convert/tag_filter_context.hpp

        ${SRC_ROOT}/convert/tag_filter_config_parser.hpp
        ${SRC_ROOT}/convert/tag_filter_config_parser.cpp

        ${SRC_ROOT}/convert/tag_filter_rule.hpp
        ${SRC_ROOT}/convert/tag_filter_rule.cpp

        ${SRC_ROOT}/convert/osm_converter.cpp
        ${SRC_ROOT}/convert/osm_converter.hpp

        ${SRC_ROOT}/convert/geom_utils.cpp
        ${SRC_ROOT}/convert/geom_utils.hpp

        ${SRC_ROOT}/convert/osm_polygon.cpp

        ${SRC_ROOT}/convert/osm_pbf_reader.cpp
        ${SRC_ROOT}/convert/osm_pbf_reader.hpp
        ${SRC_ROOT}/convert/osm_xml_reader.cpp
        ${SRC_ROOT}/convert/osm_xml_reader.hpp

        ${SRC_ROOT}/convert/osm_document.cpp
        ${SRC_ROOT}/convert/osm_document.hpp

        ${SRC_ROOT}/convert/osm_storage.hpp

        ${SRC_ROOT}/convert/osm_storage_memory.hpp

        ${SRC_ROOT}/convert/lua_context.hpp
        ${SRC_ROOT}/convert/lua_context.cpp

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
        ${SRC_ROOT}/util/progress_stream.hpp
        ${SRC_ROOT}/util/progress_stream.cpp

        ${SRC_ROOT}/util/xml_pull_parser.hpp
        ${SRC_ROOT}/util/xml_pull_parser.cpp

        ${SRC_ROOT}/convert/osm_storage_memory.hpp
        ${SRC_ROOT}/convert/osm_storage_db.hpp
        ${SRC_ROOT}/convert/osm_storage_db.cpp


)

PROTOBUF_GENERATE_CPP(OSM_PROTO_SOURCES OSM_PROTO_HEADERS ${SRC_ROOT}/protobuf/osmformat.proto ${SRC_ROOT}/protobuf/fileformat.proto)
SET_SOURCE_FILES_PROPERTIES(${OSM_PROTO_SOURCES} ${OSM_PROTO_HEADERS} PROPERTIES GENERATED TRUE)

FIND_PACKAGE(BISON REQUIRED)
FIND_PACKAGE(FLEX REQUIRED)

FLEX_TARGET(TAG_FILTER_SCANNER ${SRC_ROOT}/convert/tag_filter.l  ${SRC_ROOT}/convert/parser/tag_filter_scanner.cpp)
BISON_TARGET(TAG_FILTER_PARSER ${SRC_ROOT}/convert/tag_filter.y  ${SRC_ROOT}/convert/parser/tag_filter_parser.cpp)

ADD_FLEX_BISON_DEPENDENCY(TAG_FILTER_SCANNER TAG_FILTER_PARSER COMPILE_FLAGS "-v -Wconflicts-sr")

LIST(APPEND OSM2MAP_SOURCES ${FLEX_TAG_FILTER_SCANNER_OUTPUTS} ${BISON_TAG_FILTER_PARSER_OUTPUTS}
	${OSM_PROTO_SOURCES} ${OSM_PROTO_HEADERS}
)

ADD_EXECUTABLE(osm2map ${OSM2MAP_SOURCES})
TARGET_LINK_LIBRARIES(osm2map ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES} sqlite3 ${SPATIALITE_LIBRARY} ${SHAPELIB_LIBRARY} ${Boost_LIBRARIES} ${LUA_LIBRARIES} dl)

INSTALL(TARGETS osm2map DESTINATION bin  )


LIST(APPEND OSM2SQLITE_SOURCES ${FLEX_TAG_FILTER_SCANNER_OUTPUTS} ${BISON_TAG_FILTER_PARSER_OUTPUTS}
        ${OSM_PROTO_SOURCES} ${OSM_PROTO_HEADERS}
)

ADD_EXECUTABLE(osm2sqlite ${OSM2SQLITE_SOURCES})
TARGET_LINK_LIBRARIES(osm2sqlite ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES} sqlite3 ${SPATIALITE_LIBRARY} ${SHAPELIB_LIBRARY} ${Boost_LIBRARIES} ${LUA_LIBRARIES} dl)

INSTALL(TARGETS osm2sqlite DESTINATION bin  )



LIST(APPEND OSM2POI_SOURCES ${FLEX_TAG_FILTER_SCANNER_OUTPUTS} ${BISON_TAG_FILTER_PARSER_OUTPUTS}
        ${OSM_PROTO_SOURCES} ${OSM_PROTO_HEADERS}
)

ADD_EXECUTABLE(osm2poi ${OSM2POI_SOURCES})
TARGET_LINK_LIBRARIES(osm2poi ${PROTOBUF_LIBRARIES} ${ZLIB_LIBRARIES} sqlite3 ${ICU_LIBRARIES} ${SPATIALITE_LIBRARY} ${SHAPELIB_LIBRARY} ${Boost_LIBRARIES} ${LUA_LIBRARIES} dl)

INSTALL(TARGETS osm2poi DESTINATION bin  )
