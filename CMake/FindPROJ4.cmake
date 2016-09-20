# Find PROJ4
# ~~~~~~~~~
# Copyright (c) 2013, Paul Ramsey <pramsey@cleverelephant.ca>
# (based on FindGEOS.cmake by Mateusz Loskot)
# Redistribution and use is allowed according to the terms of the BSD license.
# For details see the accompanying COPYING-CMAKE-SCRIPTS file.
#
# CMake module to search for PROJ4 library
#
# If it's found it sets PROJ4_FOUND to TRUE
# and following variables are set:
#    PROJ4_INCLUDE_DIR
#    PROJ4_LIBRARY
#

SET(PROJ4_FOUND FALSE)

IF(WIN32)

  IF (MINGW)
    FIND_PATH(PROJ4_INCLUDE_DIR proj_api.h /usr/local/include /usr/include c:/msys/local/include)
    FIND_LIBRARY(PROJ4_LIBRARY NAMES proj PATHS /usr/local/lib /usr/lib c:/msys/local/lib)
  ENDIF (MINGW)

  IF (MSVC)
    FIND_PATH(PROJ4_INCLUDE_DIR proj_api.h $ENV{LIB_DIR}/include $ENV{INCLUDE})
    FIND_LIBRARY(PROJ4_LIBRARY NAMES proj proj_i PATHS 
      "$ENV{LIB}/lib"
      $ENV{LIB}
      c:/msys/local/lib
      NO_DEFAULT_PATH
      )
    IF (PROJ4_LIBRARY)
       SET (
         PROJ4_LIBRARY 
         PROJ4_LIBRARY;odbc32;odbccp32 
         CACHE STRING INTERNAL)
    ENDIF (PROJ4_LIBRARY)
  ENDIF (MSVC)
  
ELSE(WIN32)

  IF(UNIX) 
    # try to use framework on mac
    # want clean framework path, not unix compatibility path
    IF (APPLE)
      IF (CMAKE_FIND_FRAMEWORK MATCHES "FIRST"
          OR CMAKE_FRAMEWORK_PATH MATCHES "ONLY"
          OR NOT CMAKE_FIND_FRAMEWORK)
        SET (CMAKE_FIND_FRAMEWORK_save ${CMAKE_FIND_FRAMEWORK} CACHE STRING "" FORCE)
        SET (CMAKE_FIND_FRAMEWORK "ONLY" CACHE STRING "" FORCE)
        FIND_LIBRARY(PROJ4_LIBRARY PROJ4)
        IF (PROJ4_LIBRARY)
          # they're all the same in a framework
          SET (PROJ4_INCLUDE_DIR ${PROJ4_LIBRARY}/Headers CACHE PATH "Path to a file.")
        ENDIF (PROJ4_LIBRARY)
        SET (CMAKE_FIND_FRAMEWORK ${CMAKE_FIND_FRAMEWORK_save} CACHE STRING "" FORCE)
      ENDIF ()
    ENDIF (APPLE)

    IF (NOT PROJ4_INCLUDE_DIR OR NOT PROJ4_LIBRARY)
    
      FIND_PATH(PROJ4_INCLUDE_DIR proj_api.h
        PATHS
        /usr/local/include
        /usr/include
        /sw/include # Fink
        /opt/local/include # DarwinPorts
        /opt/csw/include # Blastwave
        /opt/include
      )

      FIND_LIBRARY(PROJ4_LIBRARY 
        NAMES proj
        PATHS $ENV{PROJ4_HOME}
          NO_DEFAULT_PATH
          PATH_SUFFIXES lib64 lib
      )
      
      FIND_LIBRARY(PROJ4_LIBRARY 
        NAMES proj
        PATHS
          /usr/local
          /usr
          /sw
          /opt/local
          /opt/csw
          /opt
          /usr/freeware
        PATH_SUFFIXES lib64 lib
      )

    ENDIF ()

  ENDIF(UNIX)

ENDIF(WIN32)

IF (PROJ4_INCLUDE_DIR AND PROJ4_LIBRARY)
   SET(PROJ4_FOUND TRUE)
ENDIF (PROJ4_INCLUDE_DIR AND PROJ4_LIBRARY)

IF (PROJ4_FOUND)
   IF (NOT PROJ4_FIND_QUIETLY)
      MESSAGE(STATUS "Found PROJ4: ${PROJ4_LIBRARY}")
   ENDIF (NOT PROJ4_FIND_QUIETLY)
ELSE (PROJ4_FOUND)
   MESSAGE(STATUS "Could not find PROJ4")
ENDIF (PROJ4_FOUND)
