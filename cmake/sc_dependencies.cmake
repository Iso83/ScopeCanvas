include(FetchContent)

function(sc_setup_freetype OUT_TARGET)
    # Emscripten's FreeType port is selected on the consuming render target.  A
    # host find_package result must never leak a native library into a Wasm link.
    if(EMSCRIPTEN)
        set(${OUT_TARGET} "" PARENT_SCOPE)
        return()
    endif()
    option(SC_FETCH_FREETYPE
        "Download and build FreeType when no system package is available"
        ON
    )

    find_package(Freetype QUIET)

    if(NOT Freetype_FOUND AND SC_FETCH_FREETYPE)
        set(FT_DISABLE_ZLIB ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_BZIP2 ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_PNG ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_HARFBUZZ ON CACHE BOOL "" FORCE)
        set(FT_DISABLE_BROTLI ON CACHE BOOL "" FORCE)

        FetchContent_Declare(
            sc_freetype
            GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
            GIT_TAG VER-2-13-3
        )

        FetchContent_MakeAvailable(sc_freetype)
    endif()

    set(FREETYPE_TARGET "")

    if(TARGET Freetype::Freetype)
        get_target_property(
            ALIAS_TARGET
            Freetype::Freetype
            ALIASED_TARGET
        )

        if(ALIAS_TARGET)
            set(FREETYPE_TARGET ${ALIAS_TARGET})
        else()
            set(FREETYPE_TARGET Freetype::Freetype)
        endif()

    elseif(TARGET freetype)
        set(FREETYPE_TARGET freetype)

    elseif(TARGET freetype_static)
        set(FREETYPE_TARGET freetype_static)
    endif()

    if(TARGET ${FREETYPE_TARGET}
       AND NOT FREETYPE_TARGET MATCHES "::")
        set_property(
            TARGET ${FREETYPE_TARGET}
            PROPERTY FOLDER "${SC_THIRDPARTY_FOLDER}/freetype"
        )

        set_target_properties(
            ${FREETYPE_TARGET}
            PROPERTIES
                DEBUG_POSTFIX ""
                RELWITHDEBINFO_POSTFIX ""
                MINSIZEREL_POSTFIX ""
        )
    endif()

    set(${OUT_TARGET} ${FREETYPE_TARGET} PARENT_SCOPE)
endfunction()
