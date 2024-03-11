cmake_minimum_required(VERSION 3.23)
project(gllearn)

set(CMAKE_CXX_STANDARD 17)

# set(CMAKE_EXE_LINKER_FLAGS "-static")

include_directories(${CMAKE_SOURCE_DIR}/third_party/include)
aux_source_directory(src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party)
target_link_libraries(${PROJECT_NAME} opengl32 glew32 glfw3)

if(CMAKE_SYSTEM_NAME MATCHES "Windows")
    message(STATUS "Copy dll to binary dir")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/third_party/glew32.dll" "${PROJECT_BINARY_DIR}/glew32.dll")
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy "${CMAKE_SOURCE_DIR}/third_party/glfw3.dll" "${PROJECT_BINARY_DIR}/glfw3.dll")
endif()