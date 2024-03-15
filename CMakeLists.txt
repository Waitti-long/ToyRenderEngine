cmake_minimum_required(VERSION 3.23)
project(gllearn)

set(CMAKE_CXX_STANDARD 17)

set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(${CMAKE_SOURCE_DIR}/third_party/include)
include_directories(${CMAKE_SOURCE_DIR})
aux_source_directory(src SRC_LIST)

# SOIL2
set(SOIL2_SRC "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/image_DXT.c"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/image_DXT.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/image_helper.c"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/image_helper.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/SOIL2.c"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/SOIL2.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/wfETC.c"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/common/common.cpp"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/common/common.hpp"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/pkm_helper.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/pvr_helper.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stb_image.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stb_image_write.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_DDS.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_DDS_c.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_ext.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_ext_c.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_pkm.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_pkm_c.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_pvr.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_pvr_c.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_qoi.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/stbi_qoi_c.h"
        "${CMAKE_SOURCE_DIR}/third_party/SOIL2/src/SOIL2/wfETC.h")

add_executable(${PROJECT_NAME} ${SRC_LIST} ${SOIL2_SRC})

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party)
target_link_libraries(${PROJECT_NAME} opengl32 glew32 glfw3)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Copy dll to binary dir")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/third_party/glew32.dll" "${PROJECT_BINARY_DIR}/glew32.dll")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/third_party/glfw3.dll" "${PROJECT_BINARY_DIR}/glfw3.dll")
endif ()