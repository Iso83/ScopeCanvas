cmake_minimum_required(VERSION 3.16)
project(ScopeCanvas)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable IDE folders (Visual Studio)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(FetchContent)

# ---------- GLFW configuration ----------

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

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

# ---------- Organize third party targets ----------

if(TARGET glfw)
    set_property(TARGET glfw PROPERTY FOLDER "third_party/glfw")
endif()

if(TARGET update_mappings)
    set_property(TARGET update_mappings PROPERTY FOLDER "third_party/glfw")
endif()

if(TARGET glm)
    set_property(TARGET glm PROPERTY FOLDER "third_party/glm")
endif()

if(TARGET nlohmann_json)
    set_property(TARGET nlohmann_json PROPERTY FOLDER "third_party/json")
endif()

# ---------- GLAD (local loader) ----------

add_library(glad STATIC
    third_party/glad/src/glad.c
)

target_include_directories(glad PUBLIC
    third_party/glad/include
)

set_property(TARGET glad PROPERTY FOLDER "third_party/glad")

# ---------- Sources ----------

file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
    src/*.h
)

# IDE folder structure
source_group(TREE ${CMAKE_SOURCE_DIR}/src FILES ${SRC_FILES} ${HEADER_FILES})

# ---------- Assets (IDE only) ----------

file(GLOB_RECURSE ASSET_FILES CONFIGURE_DEPENDS
    assets/*
)

# Show assets in Visual Studio solution tree
source_group(TREE ${CMAKE_SOURCE_DIR}/assets FILES ${ASSET_FILES})

set_property(SOURCE ${ASSET_FILES} PROPERTY VS_SETTINGS "ExcludedFromBuild=true")

# ---------- Target ----------

add_executable(ScopeCanvas
    ${SRC_FILES}
    ${HEADER_FILES}
	${ASSET_FILES}
)

target_include_directories(ScopeCanvas PRIVATE
    src
    include
)

set_property(TARGET ScopeCanvas PROPERTY FOLDER "engine")

# ---------- Link ----------

target_link_libraries(ScopeCanvas
    glfw
    glm
    glad
    nlohmann_json::nlohmann_json
)