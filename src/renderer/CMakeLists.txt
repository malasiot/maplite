SET ( RENDERER_SOURCES
	
	${SRC_ROOT}/util/tms.cpp
	${SRC_ROOT}/util/tms.hpp
	${SRC_ROOT}/util/dictionary.cpp
	${SRC_ROOT}/util/dictionary.hpp

	${SRC_ROOT}/3rdparty/pugixml/pugixml.cpp

	${SRC_ROOT}/renderer/theme.cpp
	${SRC_ROOT}/renderer/theme.hpp

	${SRC_ROOT}/renderer/svg/rendering.cpp
	${SRC_ROOT}/renderer/svg/rendering.hpp
	${SRC_ROOT}/renderer/svg/document.cpp
	${SRC_ROOT}/renderer/svg/document.hpp
	${SRC_ROOT}/renderer/svg/util.hpp
	${SRC_ROOT}/renderer/svg/util.cpp

	${SRC_ROOT}/renderer/text_engine.cpp
	${SRC_ROOT}/renderer/text_engine.hpp
	${SRC_ROOT}/renderer/collision_checker.cpp
	${SRC_ROOT}/renderer/collision_checker.hpp
	${SRC_ROOT}/renderer/renderer.cpp
	${SRC_ROOT}/renderer/renderer.hpp
	${SRC_ROOT}/renderer/geom_util.cpp

	${SRC_ROOT}/renderer/image_buffer.cpp
	${SRC_ROOT}/renderer/image_buffer.hpp

	${SRC_ROOT}/renderer/symbols.cpp
	${SRC_ROOT}/renderer/symbols.hpp
)


ADD_LIBRARY(mapsforge_renderer ${RENDERER_SOURCES})
TARGET_LINK_LIBRARIES(mapsforge_renderer ${CAIRO_LIBRARY} ${FONTCONFIG_LIBRARIES}
	${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${PNG_LIBRARIES}  ${GEOS_LIBRARY} ${Boost_LIBRARIES})

INSTALL(TARGETS mapsforge_renderer DESTINATION lib  )
