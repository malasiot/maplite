# - Find CAIRO library
# Find the native CAIRO includes and library
# This module defines
#  CAIRO_INCLUDE_DIR, where to find CAIRO.h, etc.
#  CAIRO_LIBRARIES, libraries to link against to use CAIRO.
#  CAIRO_FOUND, If false, do not try to use CAIRO.
# also defined, but not for general use are
#  CAIRO_LIBRARY, where to find the CAIRO library.

FIND_PATH(CAIRO_INCLUDE_DIR cairo.h PATH_SUFFIXES cairo)


SET(CAIRO_NAMES ${CAIRO_NAMES} cairo libcairo cairo2 libcairo2)
FIND_LIBRARY(CAIRO_LIBRARY NAMES ${CAIRO_NAMES} )

# handle the QUIETLY and REQUIRED arguments and set CAIRO_FOUND to TRUE if 
# all listed variables are TRUE
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(CAIRO  DEFAULT_MSG  CAIRO_LIBRARY  CAIRO_INCLUDE_DIR)

IF(CAIRO_FOUND)
  SET( CAIRO_LIBRARIES ${CAIRO_LIBRARY} )
ENDIF(CAIRO_FOUND)

MARK_AS_ADVANCED(CAIRO_INCLUDE_DIR CAIRO_LIBRARY)
