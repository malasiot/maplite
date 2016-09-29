PROTOBUF_GENERATE_CPP(MT_PROTO_SOURCES MT_PROTO_HEADERS ${SRC_ROOT}/protobuf/mesh_tile.proto)

SET ( TILE_SERVER_SOURCES

	${SRC_ROOT}/util/base64.cpp
	${SRC_ROOT}/util/base64.hpp
	${SRC_ROOT}/util/database.cpp
	${SRC_ROOT}/util/database.hpp

	${SRC_ROOT}/util/dictionary.cpp
	${SRC_ROOT}/util/dictionary.hpp

	${SRC_ROOT}/util/logger.cpp
	${SRC_ROOT}/util/logger.hpp

	${SRC_ROOT}/server/map_server.cpp
	${SRC_ROOT}/server/map_server.hpp

#	${SRC_ROOT}/server/mesh_tile_renderer.cpp
#	${SRC_ROOT}/server/mesh_tile_renderer.hpp

	${SRC_ROOT}/renderer/tms.cpp
	${SRC_ROOT}/renderer/tms.hpp


	${SRC_ROOT}/server/connection.cpp
	${SRC_ROOT}/server/connection.hpp
	${SRC_ROOT}/server/connection_manager.hpp
	${SRC_ROOT}/server/connection_manager.cpp
	${SRC_ROOT}/server/io_service_pool.cpp
	${SRC_ROOT}/server/io_service_pool.hpp
	${SRC_ROOT}/server/reply.cpp
	${SRC_ROOT}/server/reply.hpp
	${SRC_ROOT}/server/request_handler.hpp
	${SRC_ROOT}/server/request.hpp
	${SRC_ROOT}/server/request_parser.cpp
	${SRC_ROOT}/server/request_parser.hpp
	${SRC_ROOT}/server/server.cpp
	${SRC_ROOT}/server/server.hpp
	${SRC_ROOT}/server/http_parser.c
	${SRC_ROOT}/server/http_parser.h
	${SRC_ROOT}/server/request_handler_factory.hpp

	${SRC_ROOT}/server/shader_config.cpp
	${SRC_ROOT}/server/shader_config.hpp
	${SRC_ROOT}/server/map_server_handler_factory.cpp
	${SRC_ROOT}/server/map_server_handler_factory.hpp
	${SRC_ROOT}/server/gl_rendering_loop.hpp
	${SRC_ROOT}/server/tile_request_handler.hpp
	${SRC_ROOT}/server/tile_request_handler.cpp
	${SRC_ROOT}/server/vector_tile_request_handler.hpp
	${SRC_ROOT}/server/vector_tile_request_handler.cpp
	${SRC_ROOT}/server/asset_request_handler.hpp
	${SRC_ROOT}/server/asset_request_handler.cpp

	${SRC_ROOT}/3rdparty/pugixml/pugixml.cpp

	${SRC_ROOT}/server/feature_collection.cpp
	${SRC_ROOT}/server/feature_collection.hpp
	${SRC_ROOT}/server/kml_reader.cpp
	${SRC_ROOT}/server/kml_reader.hpp
	${SRC_ROOT}/server/gpx_reader.cpp
	${SRC_ROOT}/server/gpx_reader.hpp
	${SRC_ROOT}/server/jp2_decoder.cpp
	${SRC_ROOT}/server/jp2_decoder.hpp
	${SRC_ROOT}/server/raster_tile_cache.cpp
	${SRC_ROOT}/server/raster_tile_cache.hpp
	${SRC_ROOT}/server/raster_tile_source.cpp
	${SRC_ROOT}/server/raster_tile_source.hpp
	${SRC_ROOT}/server/raster_request_handler.cpp
	${SRC_ROOT}/server/raster_request_handler.hpp
#	${SRC_ROOT}/server/gl_tile_request_handler.cpp
#	${SRC_ROOT}/server/gl_tile_request_handler.hpp
	${SRC_ROOT}/server/png_writer.cpp
	${SRC_ROOT}/server/png_writer.hpp
)

ADD_LIBRARY(mapserver STATIC ${TILE_SERVER_SOURCES} ${MT_PROTO_SOURCES} ${MT_PROTO_HEADERS})
TARGET_LINK_LIBRARIES(mapserver ${SQLITE3_LIBRARY} ${Boost_LIBRARIES} ${PROTOBUF_LIBRARIES} ${PNG_LIBRARIES}
${GLFW_LIBRARIES} ${GLEW_LIBRARIES} ${OPENGL_LIBRARIES} ${OPENJPEG_LIBRARIES} X11 Xxf86vm Xrandr Xi pthread dl z)

ADD_EXECUTABLE(tileserver ${SRC_ROOT}/server/tile_server.cpp)
TARGET_LINK_LIBRARIES(tileserver mapserver)

ADD_EXECUTABLE(testjpeg ${SRC_ROOT}/server/test_jpeg.cpp)
TARGET_LINK_LIBRARIES(testjpeg mapserver)
