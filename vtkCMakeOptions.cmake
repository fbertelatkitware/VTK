#
# This file can be included by other projects that use or depend on VTK
# it sets up many default parameters and include paths.
# Please do not add any commands which creates/sets cache entries
# in this file since it is usually used in combination with the
# LOAD_CACHE() command. Otherwise, the same entry will be set
# more than once and the results will be unpredictable.
#

IF (VTK_USE_RENDERING)
  IF (VTK_USE_VOLUMEPRO)
    IF (VLI_INCLUDE_PATH_FOR_VG500)
      INCLUDE_DIRECTORIES( ${VLI_INCLUDE_PATH_FOR_VG500} )
    ENDIF (VLI_INCLUDE_PATH_FOR_VG500)
    IF (VLI_INCLUDE_PATH_FOR_VP1000)
      INCLUDE_DIRECTORIES( ${VLI_INCLUDE_PATH_FOR_VP1000} )
    ENDIF (VLI_INCLUDE_PATH_FOR_VP1000)
    IF (VLI_LIBRARY_FOR_VG500)
      LINK_LIBRARIES (${VLI_LIBRARY_FOR_VG500})
    ENDIF (VLI_LIBRARY_FOR_VG500)
    IF (VLI_LIBRARY_FOR_VP1000)
      LINK_LIBRARIES (${VLI_LIBRARY_FOR_VP1000})
    ENDIF (VLI_LIBRARY_FOR_VP1000)
  ENDIF (VTK_USE_VOLUMEPRO)
  INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Rendering)
  IF (NOT VTK_LIBRARY_PATH)
    LINK_DIRECTORIES(${VTK_BINARY_DIR}/Rendering) 
  ENDIF (NOT VTK_LIBRARY_PATH)
ENDIF (VTK_USE_RENDERING)

# Optional Stanford University PLY file format
IF(PLY_LIBRARY)
  IF(PLY_PATH)
    LINK_LIBRARIES( ${PLY_LIBRARY} )
  ENDIF(PLY_PATH)
ENDIF(PLY_LIBRARY)

IF (VTK_USE_HYBRID)
  # hybrid requires rendering
  IF (VTK_USE_RENDERING)
    INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Hybrid)
    IF (NOT VTK_LIBRARY_PATH)
      LINK_DIRECTORIES(${VTK_BINARY_DIR}/Hybrid) 
    ENDIF (NOT VTK_LIBRARY_PATH)
  ELSE (VTK_USE_RENDERING)
    MESSAGE("The Hybrid option requires Rendering, but you do not have Rendering selected." "Warning")
  ENDIF (VTK_USE_RENDERING)
ENDIF (VTK_USE_HYBRID)

IF (VTK_USE_PATENTED)
  INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Patented)
  IF (NOT VTK_LIBRARY_PATH)
    LINK_DIRECTORIES(${VTK_BINARY_DIR}/Patented) 
  ENDIF (NOT VTK_LIBRARY_PATH)
ENDIF (VTK_USE_PATENTED)

IF (VTK_USE_PARALLEL)
    INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Parallel)
  IF (NOT VTK_LIBRARY_PATH)
    LINK_DIRECTORIES(${VTK_BINARY_DIR}/Parallel) 
  ENDIF (NOT VTK_LIBRARY_PATH)
ENDIF (VTK_USE_PARALLEL)

INCLUDE_DIRECTORIES(
${VTK_BINARY_DIR} 
${VTK_SOURCE_DIR}/Common
${VTK_SOURCE_DIR}/Filtering
${VTK_SOURCE_DIR}/Imaging
${VTK_SOURCE_DIR}/Graphics
${VTK_SOURCE_DIR}/IO
)

IF (NOT VTK_LIBRARY_PATH)
  LINK_DIRECTORIES(
    ${VTK_BINARY_DIR}/Common 
    ${VTK_BINARY_DIR}/Filtering
    ${VTK_BINARY_DIR}/Imaging
    ${VTK_BINARY_DIR}/Graphics
    ${VTK_BINARY_DIR}/IO
    )
ENDIF (NOT VTK_LIBRARY_PATH)

IF (UNIX)
  LINK_LIBRARIES(${CMAKE_THREAD_LIBS} ${CMAKE_DL_LIBS} -lm)
ENDIF (UNIX)

#
# Look for the PNG, zlib, jpeg, and tiff libraries and header files
#
INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Utilities/zlib)
INCLUDE_DIRECTORIES(${VTK_BINARY_DIR}/Utilities/zlib)
INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Utilities/png)
INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Utilities/jpeg)
INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Utilities/tiff)
IF (NOT VTK_LIBRARY_PATH)
  LINK_DIRECTORIES(${VTK_BINARY_DIR}/Utilities/png)
  LINK_DIRECTORIES(${VTK_BINARY_DIR}/Utilities/zlib)
  LINK_DIRECTORIES(${VTK_BINARY_DIR}/Utilities/jpeg)
  LINK_DIRECTORIES(${VTK_BINARY_DIR}/Utilities/tiff)
ENDIF (NOT VTK_LIBRARY_PATH)

#
# add include path for general testing header file
#
INCLUDE_DIRECTORIES(${VTK_SOURCE_DIR}/Common/Testing/Cxx)

IF(NOT CMAKE_COMPILER_IS_GNUCXX)
  SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ANSI_CFLAGS}")
ENDIF(NOT CMAKE_COMPILER_IS_GNUCXX)

IF(APPLE)
  SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-coalesce-templates -fno-coalesce-static-vtables")
ENDIF(APPLE)

IF(CMAKE_COMPILER_IS_GNUCXX)
  IF(WIN32)
    IF(UNIX)
      SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mwin32")
      SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -mwin32")
    ENDIF(UNIX)
  ENDIF(WIN32)
ENDIF(CMAKE_COMPILER_IS_GNUCXX)

IF (OPENGL_INCLUDE_PATH)
  INCLUDE_DIRECTORIES(${OPENGL_INCLUDE_PATH})
ENDIF(OPENGL_INCLUDE_PATH)

#
# get information for Tcl wrapping 
#
IF (VTK_WRAP_TCL)
  # add in the Tcl values if found
  IF (TCL_INCLUDE_PATH)
    INCLUDE_DIRECTORIES(${TCL_INCLUDE_PATH})
  ENDIF (TCL_INCLUDE_PATH)
ENDIF (VTK_WRAP_TCL)

#
# get information for Python wrapping 
#
IF (VTK_WRAP_PYTHON)
  INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})
ENDIF (VTK_WRAP_PYTHON)

#
# get information for Java wrapping 
#
IF (VTK_WRAP_JAVA)
  IF (JAVA_INCLUDE_PATH)
    INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH})
  ENDIF (JAVA_INCLUDE_PATH)
  IF (JAVA_INCLUDE_PATH2)
    INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH2})
  ENDIF (JAVA_INCLUDE_PATH2)
  IF (JAVA_AWT_INCLUDE_PATH)
    INCLUDE_DIRECTORIES(${JAVA_AWT_INCLUDE_PATH})
  ENDIF (JAVA_AWT_INCLUDE_PATH)
ENDIF (VTK_WRAP_JAVA)

# for cmake version 1.1 and later
IF (CMAKE_MAJOR_VERSION MATCHES [1-9][0-9]*)
  IF (CMAKE_MINOR_VERSION MATCHES [1-9][0-9]*)
    SET(VTK_MAKE_INSTANTIATOR 1)
  ENDIF (CMAKE_MINOR_VERSION MATCHES [1-9][0-9]*)
ENDIF (CMAKE_MAJOR_VERSION MATCHES [1-9][0-9]*)


