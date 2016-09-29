SET ( RSC_TOOL_SOURCES
	${SRC_ROOT}/util/database.cpp
	${SRC_ROOT}/util/database.hpp
	${SRC_ROOT}/util/dictionary.cpp
	${SRC_ROOT}/util/dictionary.hpp
	${SRC_ROOT}/tools/makeres.cpp
)

ADD_EXECUTABLE(makeres ${RSC_TOOL_SOURCES})
TARGET_LINK_LIBRARIES(makeres ${SQLITE3_LIBRARY} ${Boost_LIBRARIES} pthread dl z)

SET ( CHECK_MAP_SOURCES
	${SRC_ROOT}/vector/mapsforge_map_reader.cpp
	${SRC_ROOT}/vector/mapsforge_map_reader.hpp
	${SRC_ROOT}/renderer/tms.cpp
	${SRC_ROOT}/renderer/tms.hpp
	${SRC_ROOT}/util/dictionary.cpp
	${SRC_ROOT}/util/dictionary.hpp

	${SRC_ROOT}/renderer/theme.cpp
	${SRC_ROOT}/renderer/theme.hpp

	${SRC_ROOT}/renderer/svg/rendering.cpp
	${SRC_ROOT}/renderer/svg/rendering.hpp
	${SRC_ROOT}/renderer/svg/document.cpp
	${SRC_ROOT}/renderer/svg/document.hpp
	${SRC_ROOT}/renderer/svg/util.hpp
	${SRC_ROOT}/renderer/svg/util.cpp

	${SRC_ROOT}/renderer/render_text.cpp
	${SRC_ROOT}/renderer/collision_checker.cpp
	${SRC_ROOT}/renderer/collision_checker.hpp
	${SRC_ROOT}/renderer/renderer.cpp
	${SRC_ROOT}/renderer/renderer.hpp
	${SRC_ROOT}/renderer/geom_util.cpp

	${SRC_ROOT}/renderer/image_buffer.cpp
	${SRC_ROOT}/renderer/image_buffer.hpp

	${SRC_ROOT}/3rdparty/pugixml/pugixml.cpp

	${SRC_ROOT}/tools/check_map.cpp
)


ADD_EXECUTABLE(mf_check_map ${CHECK_MAP_SOURCES})
TARGET_LINK_LIBRARIES(mf_check_map ${Boost_LIBRARIES} ${CAIRO_LIBRARY} ${FONTCONFIG_LIBRARIES}
	${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${PNG_LIBRARIES}  ${GEOS_LIBRARY})
