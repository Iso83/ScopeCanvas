include_guard(GLOBAL)

function(assets_setup_fonts)
    # =========================================================
    # Summary
    #
    # Configures the project's font asset paths.
    # =========================================================
    set(ASSETS_FONT_DIR
        "${CMAKE_SOURCE_DIR}/assets/fonts"
        CACHE INTERNAL ""
    )

    set(ASSETS_DEFAULT_FONT
        "${ASSETS_FONT_DIR}/DejaVuSans.ttf"
        CACHE INTERNAL ""
    )
endfunction()

function(assets_setup_emscripten TARGET)
    # =========================================================
    # Summary
    #
    # Configures Emscripten asset preloading for a target.
    #
    # Parameters:
    #   [in] TARGET - Target receiving the asset preload options.
    # =========================================================
    if(NOT EMSCRIPTEN)
        return()
    endif()

    if(EXISTS "${ASSETS_DEFAULT_FONT}")
        target_link_options(${TARGET}
            PRIVATE
                "--preload-file=${ASSETS_DEFAULT_FONT}@/assets/DejaVuSans.ttf"
        )
    endif()
endfunction()