# - Try to find the Urg includes and library
# which defines
#
# URG_FOUND - system has Urg
# URG_INCLUDE_DIR - where to find UrgDevice.h
# URG_LIBRARIES - the libraries to link against to use Urg

SET(URG_FOUND FALSE)

FIND_PATH(URG_INCLUDE_DIR NAMES urg/UrgDevice.h HINTS $ENV{URGROOT}/include)

FIND_LIBRARY(URG_LIBRARY NAMES c_urg HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_SYSTEM_LIBRARY NAMES c_urg_system HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_CONNECTION_LIBRARY NAMES c_urg_connection HINTS $ENV{URGROOT}/lib)

FIND_LIBRARY(URG_CPP_LIBRARY NAMES urg HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_CPP_CONNECTION_LIBRARY NAMES urg_connection HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_CPP_SYSTEM_LIBRARY NAMES urg_system HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_CPP_COMMON_LIBRARY NAMES urg_common HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_CPP_COORDINATE_LIBRARY NAMES urg_coordinate HINTS $ENV{URGROOT}/lib)
FIND_LIBRARY(URG_CPP_GEOMETRY_LIBRARY NAMES urg_geometry HINTS $ENV{URGROOT}/lib)


SET(URG_LIBRARIES
        ${URG_SYSTEM_LIBRARY}
        ${URG_LIBRARY}
        ${URG_CONNECTION_LIBRARY}

        ${URG_CPP_LIBRARY}
        ${URG_CPP_CONNECTION_LIBRARY}
        ${URG_CPP_SYSTEM_LIBRARY}
        ${URG_CPP_COMMON_LIBRARY}
        ${URG_CPP_COORDINATE_LIBRARY}
        ${URG_CPP_GEOMETRY_LIBRARY}
        )

IF(URG_LIBRARIES)
SET(URG_FOUND TRUE)
ENDIF(URG_LIBRARIES)

IF(URG_FOUND)
    MESSAGE(STATUS "Found c_urg libraries. " ${URG_LIBRARIES})
ELSE(URG_FOUND)
    IF(URG_FIND_REQUIRED)
        MESSAGE(FATAL_ERROR "Could'not find URG library")
    ENDIF(URG_FIND_REQUIRED)
ENDIF(URG_FOUND)
