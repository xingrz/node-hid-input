project(hid-input)

set(CMAKE_CXX_STANDARD 11)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

set(SRCS "src/addon.cc")

if(LINUX)
    list(APPEND SRCS "src/input_linux.cc")
endif()

add_library(${PROJECT_NAME} SHARED
    ${SRCS}
    ${CMAKE_JS_SRC}
)

set_target_properties(${PROJECT_NAME} PROPERTIES
    PREFIX ""
    SUFFIX ".node"
)

target_link_libraries(${PROJECT_NAME} ${CMAKE_JS_LIB})
target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_JS_INC})

target_compile_definitions(${PROJECT_NAME} PRIVATE -DNAPI_VERSION=7)
