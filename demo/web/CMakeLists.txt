# ============================================================================
# Source files
# ============================================================================

file(GLOB_RECURSE MODULE_SRC CONFIGURE_DEPENDS
    src/*.cpp
)

file(GLOB_RECURSE MODULE_PRIVATE_HEADERS CONFIGURE_DEPENDS
    src/*.h
)

file(GLOB_RECURSE MODULE_PUBLIC_HEADERS CONFIGURE_DEPENDS
    include/*.h
)

# ============================================================================
# Target
# ============================================================================

string(REPLACE "/" "_" MODULE_NAME "demo/web")

set(MODULE_TARGET "${PROJECT_NAME}_${MODULE_NAME}")

add_executable(${MODULE_TARGET}
    ${MODULE_SRC}
    ${MODULE_PRIVATE_HEADERS}
    ${MODULE_PUBLIC_HEADERS}
)

# ============================================================================
# Build configuration
# ============================================================================

cppcmake_version_target(${MODULE_TARGET})
cppcmake_helper_include_target(${MODULE_TARGET})

target_include_directories(${MODULE_TARGET}
    PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}/include"
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/src"
)

target_compile_features(${MODULE_TARGET}
    PUBLIC
        cxx_std_20
)

target_link_options(${MODULE_TARGET}
    PRIVATE
        "--shell-file=${CMAKE_CURRENT_SOURCE_DIR}/assets/shell.html"
        "-sUSE_GLFW=3"
        "-sMIN_WEBGL_VERSION=2"
        "-sMAX_WEBGL_VERSION=2"
        "-sALLOW_MEMORY_GROWTH=1"
        "-sNO_DISABLE_EXCEPTION_CATCHING=1"
)

set(SC_WEB_DIST_DIR "${CMAKE_BINARY_DIR}/dist")
file(MAKE_DIRECTORY "${SC_WEB_DIST_DIR}")

add_custom_command(TARGET ${MODULE_TARGET} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:${MODULE_TARGET}>"
        "${SC_WEB_DIST_DIR}/index.html"
    COMMENT "Creating dist/index.html"
)

# ============================================================================
# IDE organization
# ============================================================================

cppcmake_project_set_ide_folder(${MODULE_TARGET} "demo/web")

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/include/${PROJECT_NAME}/demo/web"
    PREFIX "Include"
    FILES
        ${MODULE_PUBLIC_HEADERS}
)

source_group(
    TREE "${CMAKE_CURRENT_SOURCE_DIR}/src"
    PREFIX "Source"
    FILES
        ${MODULE_SRC}
        ${MODULE_PRIVATE_HEADERS}
)

# ============================================================================
# Dependencies
# ============================================================================

target_link_libraries(${MODULE_TARGET}
    PRIVATE
        ${PROJECT_NAME}_demo_common
        ${PROJECT_NAME}_integration_web
)

# ============================================================================
# Assets
# ============================================================================

set_target_properties(${MODULE_TARGET} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${SC_WEB_DIST_DIR}"
    OUTPUT_NAME ${MODULE_TARGET}
    SUFFIX ".html"
)

# Default font
assets_setup_emscripten(${MODULE_TARGET})