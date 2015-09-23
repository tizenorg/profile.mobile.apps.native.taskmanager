CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
PROJECT(task-mgr)
# FIND ALL SOURCE IN A SOURCE DIRECTORY
AUX_SOURCE_DIRECTORY(${CMAKE_SOURCE_DIR}/src SRCS)

#INCLUDE FILE
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR}/include)
SET(PACKAGE "${PROJECT_NAME}")
SET(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/res")
SET(EDJDIR "${RESOURCE_DIR}/edje/${PACKAGE}")

INCLUDE(FindPkgConfig)
pkg_check_modules(pkgs REQUIRED
	appcore-efl
	evas
	edje
	elementary
	ecore
	eina
	capi-appfw-application
	capi-system-info
	capi-appfw-package-manager
	capi-appfw-app-manager
	dlog
	pkgmgr-info
	rua
	feedback
)

SET(PREFIX ${CMAKE_INSTALL_PREFIX})
SET(RESDIR "${PREFIX}/res")
SET(EDJEDIR "${RESDIR}/edje")
SET(IMAGEDIR "${RESDIR}/images")
SET(LOCALEDIR "${RESDIR}/locale")

FOREACH(flag ${pkgs_CFLAGS})
	SET(EXTRA_CFLAGS "${EXTRA_CFLAGS} ${flag}")
ENDFOREACH(flag)

SET(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${EXTRA_CFLAGS} -fPIE")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EXTRA_CFLAGS} -fPIE")

SET(CMAKE_C_FLAGS_PROFILING    "-O0 -g -pg -fPIE")
SET(CMAKE_CXX_FLAGS_PROFILING  "-O0 -std=c++0x -g -pg -fPIE")
SET(CMAKE_C_FLAGS_DEBUG        "-O0 -g -fPIE")
SET(CMAKE_CXX_FLAGS_DEBUG      "-O0 -std=c++0x -g -fPIE")
SET(CMAKE_C_FLAGS_RELEASE      "-O2 -g -fPIE")
SET(CMAKE_CXX_FLAGS_RELEASE    "-O2 -std=c++0x -g -fPIE")
SET(CMAKE_C_FLAGS_CCOV         "-O2 -g --coverage -fPIE")
SET(CMAKE_CXX_FLAGS_CCOV       "-O2 -std=c++0x -g --coverage -fPIE")

SET(CMAKE_EXE_LINKER_FLAGS "-Wl,--as-needed -pie")

MESSAGE(${CMAKE_C_FLAGS})

ADD_DEFINITIONS("-DEDJEDIR=\"${EDJEDIR}\"")
ADD_DEFINITIONS("-DLOCALEDIR=\"${LOCALEDIR}\"")
ADD_DEFINITIONS("-DIMAGEDIR=\"${IMAGEDIR}\"")

ADD_EXECUTABLE(${PROJECT_NAME} ${SRCS})

TARGET_LINK_LIBRARIES(${PROJECT_NAME} ${pkgs_LDFLAGS})

# Install
INSTALL(TARGETS ${PROJECT_NAME} DESTINATION bin)

#Install images
INSTALL(DIRECTORY res/images/ DESTINATION res/images)

# Install Manifest File
INSTALL(FILES data/org.tizen.task-mgr.xml DESTINATION /usr/share/packages)
# End of a file
ADD_SUBDIRECTORY(res)
