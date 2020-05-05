cmake_minimum_required (VERSION 3.8)

project ("VGE")
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE VGE_SRC
    "Include/*.hpp"
    "Sources/*.cpp"
)

set(SFML_STATIC_LIBRARIES TRUE)

find_package(SFML 2.5 COMPONENTS system graphics REQUIRED)
find_package(glm REQUIRED)

add_executable (${PROJECT_NAME} ${VGE_SRC})

include_directories("${CMAKE_SOURCE_DIR}/Include" ${GLM_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} sfml-graphics)
