function(sc_setup_fonts)
    set(SC_FONT_DIR
        "${CMAKE_SOURCE_DIR}/third_party/fonts"
        CACHE INTERNAL ""
    )

    set(SC_DEFAULT_FONT
        "${SC_FONT_DIR}/DejaVuSans.ttf"
        CACHE INTERNAL ""
    )
endfunction()

function(sc_setup_web_assets TARGET_NAME)
    if(NOT EMSCRIPTEN)
        return()
    endif()

    if(EXISTS "${SC_DEFAULT_FONT}")
        target_link_options(${TARGET_NAME} PRIVATE
            "--preload-file=${SC_DEFAULT_FONT}@/assets/DejaVuSans.ttf"
        )
    endif()
endfunction()