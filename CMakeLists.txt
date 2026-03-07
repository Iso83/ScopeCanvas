cmake_minimum_required(VERSION 3.16)
project(NodeCanvasGL)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# ---------- Download dependencies ----------
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.3.9
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.1
)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.3
)

FetchContent_MakeAvailable(glfw glm nlohmann_json)

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    src/*.h
)

add_executable(NodeCanvasGL
    ${SRC_FILES}
    ${HEADER_FILES}
    glad/src/glad.c
)

target_include_directories(NodeCanvasGL PRIVATE
    src
    glad/include
)

target_link_libraries(NodeCanvasGL
    glfw
    glm
    nlohmann_json::nlohmann_json
)
