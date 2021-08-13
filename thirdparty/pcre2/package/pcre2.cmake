include(bde_utils)
include(bde_struct)
include(bde_uor)

bde_prefixed_override(pcre2 process_standalone_package)
function(pcre2_process_standalone_package retUOR listFile installOpts)
    bde_assert_no_extra_args()

    get_filename_component(listDir ${listFile} DIRECTORY)
    get_filename_component(rootDir ${listDir} DIRECTORY)

    set(TARGET pcre2)

    set(headers
        ${rootDir}/config.h
        ${rootDir}/pcre2.h
        ${rootDir}/pcre2_internal.h
        ${rootDir}/pcre2_intmodedep.h
        ${rootDir}/pcre2_ucp.h
    )

    set(sources
        ${rootDir}/pcre2_auto_possess.c
        ${rootDir}/pcre2_chartables.c
        ${rootDir}/pcre2_compile.c
        ${rootDir}/pcre2_config.c
        ${rootDir}/pcre2_context.c
        ${rootDir}/pcre2_convert.c
        ${rootDir}/pcre2_dfa_match.c
        ${rootDir}/pcre2_error.c
        ${rootDir}/pcre2_extuni.c
        ${rootDir}/pcre2_find_bracket.c
        ${rootDir}/pcre2_jit_compile.c
        ${rootDir}/pcre2_maketables.c
        ${rootDir}/pcre2_match.c
        ${rootDir}/pcre2_match_data.c
        ${rootDir}/pcre2_newline.c
        ${rootDir}/pcre2_ord2utf.c
        ${rootDir}/pcre2_pattern_info.c
        ${rootDir}/pcre2_serialize.c
        ${rootDir}/pcre2_script_run.c
        ${rootDir}/pcre2_string_utils.c
        ${rootDir}/pcre2_study.c
        ${rootDir}/pcre2_substitute.c
        ${rootDir}/pcre2_substring.c
        ${rootDir}/pcre2_tables.c
        ${rootDir}/pcre2_ucd.c
        ${rootDir}/pcre2_valid_utf.c
        ${rootDir}/pcre2_xclass.c
    )

    bde_ufid_add_library(${TARGET} ${sources} ${headers})

    # Set up PIC
    # This code does not work in 3.8, but will be fixed in later versions.
    # The -fPIC flag is set explicitely in the compile options for now.
    if(${bde_ufid_is_shr} OR ${bde_ufid_is_pic})
        set_target_properties(${TARGET} PROPERTIES POSITION_INDEPENDENT_CODE 1)
    endif()

    # Compile options and definitions.
    target_compile_definitions(
        ${TARGET}
        PRIVATE
            "PCRE2_CODE_UNIT_WIDTH=8"
            "HAVE_CONFIG_H"
            "SUPPORT_UNICODE"
    )

    # JIT support. Supported except 64-bit sparc.
    if(CMAKE_SYSTEM_NAME STREQUAL "SunOS" AND ${bde_ufid_is_64})
        message(STATUS "JIT support is disabled for " ${CMAKE_SYSTEM_NAME})
    else()
        target_compile_definitions(
            ${TARGET}
            PRIVATE
                "SUPPORT_JIT"
        )
        bde_log(VERBOSE "JIT support is enabled.")
    endif()

    target_compile_options(
        ${TARGET}
        PRIVATE
            # Compiler specific compile options.
            $<$<C_COMPILER_ID:AppleClang>:
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:Clang>:
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:GNU>:
                -fdiagnostics-show-option
                -fno-strict-aliasing
                -std=gnu99
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -fPIC>
            >
            $<$<C_COMPILER_ID:SunPro>:
                -temp=/bb/data/tmp
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -xcode=pic32>
            >
            $<$<C_COMPILER_ID:XL>:
                $<$<OR:${bde_ufid_is_shr},${bde_ufid_is_pic}>: -qpic>
                $<${bde_ufid_is_mt}: -qthreaded>
            >
    )

    target_compile_definitions(
        ${TARGET}
        PRIVATE
            $<${bde_ufid_is_mt}: _REENTRANT>
            $<$<C_COMPILER_ID:GNU>:
                USE_REAL_MALLOC
            >
    )

    get_filename_component(
        EXTERNAL_INCLUDE_DIR
        ${rootDir} DIRECTORY
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<BUILD_INTERFACE:${EXTERNAL_INCLUDE_DIR}>
    )

    bde_struct_get_field(component ${installOpts} COMPONENT)
    bde_struct_get_field(includeInstallDir ${installOpts} INCLUDE_DIR)
    install(
        FILES ${headers}
        COMPONENT "${component}-headers"
        DESTINATION "${includeInstallDir}/${TARGET}"
    )

    target_include_directories(
        ${TARGET}
        PUBLIC
            $<INSTALL_INTERFACE:${includeInstallDir}>
    )

    # Don't create interfaces to only use our own build/usage reqiurements
    bde_struct_create(
        uor
        BDE_UOR_TYPE
        NAME "${TARGET}"
        TARGET "${TARGET}"
    )
    standalone_package_install(${uor} ${listFile} ${installOpts})

    # Meta data install
    install(
        DIRECTORY ${listDir}
        COMPONENT "${component}-meta"
        DESTINATION "share/bdemeta/thirdparty/${component}"
        EXCLUDE_FROM_ALL
    )

    bde_return(${uor})
endfunction()
