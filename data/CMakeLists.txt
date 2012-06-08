ADD_CUSTOM_TARGET(taskmgr.edj
		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/images
		${CMAKE_SOURCE_DIR}/data/taskmgr.edc ${CMAKE_BINARY_DIR}/data/taskmgr.edj
		DEPENDS ${CMAKE_SOURCE_DIR}/data/taskmgr.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} taskmgr.edj)
INSTALL(FILES ${CMAKE_BINARY_DIR}/data/taskmgr.edj DESTINATION ${EDJDIR})

# install theme
#ADD_CUSTOM_TARGET(theme_taskmanager.edj
#		COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/themes
#		${CMAKE_SOURCE_DIR}/data/themes/theme_taskmanager.edc ${CMAKE_BINARY_DIR}/data/theme_taskmanager.edj
#		DEPENDS ${CMAKE_SOURCE_DIR}/data/themes/theme_taskmanager.edc
#)
#ADD_DEPENDENCIES(${PROJECT_NAME} theme_taskmanager.edj)
#INSTALL(FILES ${CMAKE_BINARY_DIR}/data/theme_taskmanager.edj DESTINATION ${EDJDIR})

# install images
INSTALL(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/icons/ DESTINATION ${IMAGEDIR} FILES_MATCHING PATTERN "*.png")

