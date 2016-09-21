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
	${SRC_ROOT}/map/geom_helpers.cpp
	${SRC_ROOT}/map/geom_helpers.hpp
	${SRC_ROOT}/util/dictionary.cpp
	${SRC_ROOT}/util/dictionary.hpp

	${SRC_ROOT}/renderer/theme.cpp
	${SRC_ROOT}/renderer/theme.hpp

	${SRC_ROOT}/3rdparty/pugixml/pugixml.cpp

	${SRC_ROOT}/tools/check_map.cpp
)


ADD_EXECUTABLE(mf_check_map ${CHECK_MAP_SOURCES})
TARGET_LINK_LIBRARIES(mf_check_map ${Boost_LIBRARIES})
