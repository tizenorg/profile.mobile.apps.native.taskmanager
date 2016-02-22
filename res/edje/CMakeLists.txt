#install edj
ADD_CUSTOM_TARGET(layout.edj
                COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/images
                ${CMAKE_CURRENT_SOURCE_DIR}/layout.edc layout.edj
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/layout.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} layout.edj)
INSTALL(FILES layout.edj DESTINATION ${EDJEDIR})

ADD_CUSTOM_TARGET(item.edj
                COMMAND edje_cc -id ${CMAKE_CURRENT_SOURCE_DIR}/images
                ${CMAKE_CURRENT_SOURCE_DIR}/item.edc item.edj
                DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/item.edc
)
ADD_DEPENDENCIES(${PROJECT_NAME} item.edj)
INSTALL(FILES item.edj DESTINATION ${EDJEDIR})

