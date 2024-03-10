cmake_minimum_required(VERSION 3.23)
project(gllearn)

set(CMAKE_CXX_STANDARD 17)

include_directories(${CMAKE_SOURCE_DIR}/third_party/include)
aux_source_directory(src SRC_LIST)
add_executable(${PROJECT_NAME} ${SRC_LIST})

target_link_directories(${PROJECT_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/third_party)
target_link_libraries(${PROJECT_NAME} opengl32 glew32 glfw3)