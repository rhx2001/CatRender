include_guard(GLOBAL)

include(CMakeParseArguments)

function(_catrender_find_file output_variable)
    cmake_parse_arguments(FIND_FILE "" "DOC" "NAMES;ROOTS;PATH_SUFFIXES" ${ARGN})
    find_file(${output_variable}
        NAMES ${FIND_FILE_NAMES}
        PATHS ${FIND_FILE_ROOTS}
        PATH_SUFFIXES ${FIND_FILE_PATH_SUFFIXES}
        DOC "${FIND_FILE_DOC}"
        NO_DEFAULT_PATH
    )
    set(${output_variable} "${${output_variable}}" PARENT_SCOPE)
endfunction()

function(_catrender_find_library output_variable)
    cmake_parse_arguments(FIND_LIBRARY "" "DOC" "NAMES;ROOTS;PATH_SUFFIXES" ${ARGN})
    find_library(${output_variable}
        NAMES ${FIND_LIBRARY_NAMES}
        PATHS ${FIND_LIBRARY_ROOTS}
        PATH_SUFFIXES ${FIND_LIBRARY_PATH_SUFFIXES}
        DOC "${FIND_LIBRARY_DOC}"
        NO_DEFAULT_PATH
    )
    set(${output_variable} "${${output_variable}}" PARENT_SCOPE)
endfunction()

function(_catrender_make_interface_target target_name include_directories)
    if(NOT TARGET ${target_name})
        add_library(${target_name} INTERFACE IMPORTED)
    endif()
    set_target_properties(${target_name} PROPERTIES
        INTERFACE_INCLUDE_DIRECTORIES "${include_directories}")
endfunction()

function(catrender_configure_thirdpart)
    # All search roots are relative to the current project. No machine-specific
    # include/library path is required or embedded in the source tree.
    set(_catrender_common_roots
        "${CATRENDER_THIRDPART_DIR}"
        "${CATRENDER_THIRDPART_DIR}/include"
        "${CATRENDER_THIRDPART_DIR}/Include"
    )

    set(_catrender_glfw_roots
        ${_catrender_common_roots}
        "${CATRENDER_THIRDPART_DIR}/glfw"
        "${CATRENDER_THIRDPART_DIR}/GLFW"
        "${CATRENDER_THIRDPART_DIR}/glfw/include"
        "${CATRENDER_THIRDPART_DIR}/glfw/Include"
    )
    set(_catrender_glm_roots
        ${_catrender_common_roots}
        "${CATRENDER_THIRDPART_DIR}/glm"
        "${CATRENDER_THIRDPART_DIR}/GLM"
        "${CATRENDER_THIRDPART_DIR}/glm/include"
        "${CATRENDER_THIRDPART_DIR}/GLM/Include"
    )
    set(_catrender_stb_roots
        ${_catrender_common_roots}
        "${CATRENDER_THIRDPART_DIR}/stb"
        "${CATRENDER_THIRDPART_DIR}/STB"
        "${CATRENDER_THIRDPART_DIR}/stb_image"
        "${CATRENDER_THIRDPART_DIR}/stb/include"
        "${CATRENDER_THIRDPART_DIR}/stb_image/include"
    )
    set(_catrender_tinyobj_roots
        ${_catrender_common_roots}
        "${CATRENDER_THIRDPART_DIR}/tinyobjloader"
        "${CATRENDER_THIRDPART_DIR}/TinyObjLoader"
        "${CATRENDER_THIRDPART_DIR}/tinyobjloader/include"
        "${CATRENDER_THIRDPART_DIR}/TinyObjLoader/Include"
    )
    set(_catrender_vulkan_roots
        ${_catrender_common_roots}
        "${CATRENDER_THIRDPART_DIR}/vulkan"
        "${CATRENDER_THIRDPART_DIR}/Vulkan"
        "${CATRENDER_THIRDPART_DIR}/vulkan/include"
        "${CATRENDER_THIRDPART_DIR}/Vulkan/Include"
    )

    _catrender_find_file(CATRENDER_GLFW_HEADER
        NAMES GLFW/glfw3.h glfw3.h
        ROOTS ${_catrender_glfw_roots}
        PATH_SUFFIXES include Include GLFW
        DOC "GLFW header")
    if(CATRENDER_GLFW_HEADER)
        get_filename_component(_catrender_glfw_header_dir "${CATRENDER_GLFW_HEADER}" DIRECTORY)
        if(_catrender_glfw_header_dir STREQUAL CATRENDER_THIRDPART_DIR)
            # A flat glfw3.h needs a generated GLFW/ wrapper because the
            # existing source includes <GLFW/glfw3.h>.
            set(CATRENDER_GLFW_INCLUDE_DIR "${CATRENDER_THIRDPART_DIR}")
            set(CATRENDER_GLFW_FLAT_HEADER ON)
        else()
            get_filename_component(CATRENDER_GLFW_INCLUDE_DIR "${_catrender_glfw_header_dir}" DIRECTORY)
        endif()
    endif()

    _catrender_find_file(CATRENDER_GLM_HEADER
        NAMES glm/glm.hpp glm.hpp
        ROOTS ${_catrender_glm_roots}
        PATH_SUFFIXES include Include glm
        DOC "GLM header")
    if(CATRENDER_GLM_HEADER)
        get_filename_component(_catrender_glm_header_dir "${CATRENDER_GLM_HEADER}" DIRECTORY)
        get_filename_component(CATRENDER_GLM_INCLUDE_DIR "${_catrender_glm_header_dir}" DIRECTORY)
    endif()

    _catrender_find_file(CATRENDER_STB_IMAGE_HEADER
        NAMES stb_image/stb_image.h stb/stb_image.h stb_image.h
        ROOTS ${_catrender_stb_roots}
        PATH_SUFFIXES include Include stb STB stb_image
        DOC "stb_image header")
    if(CATRENDER_STB_IMAGE_HEADER)
        get_filename_component(CATRENDER_STB_IMAGE_INCLUDE_DIR
            "${CATRENDER_STB_IMAGE_HEADER}" DIRECTORY)
    endif()

    _catrender_find_file(CATRENDER_TINYOBJLOADER_HEADER
        NAMES tinyobjloader/tiny_obj_loader.h tiny_obj_loader.h
        ROOTS ${_catrender_tinyobj_roots}
        PATH_SUFFIXES include Include tinyobjloader TinyObjLoader
        DOC "tinyobjloader header")
    if(CATRENDER_TINYOBJLOADER_HEADER)
        get_filename_component(CATRENDER_TINYOBJLOADER_INCLUDE_DIR
            "${CATRENDER_TINYOBJLOADER_HEADER}" DIRECTORY)
    endif()

    _catrender_find_file(CATRENDER_VULKAN_HEADER
        NAMES vulkan/vulkan.h vulkan.h
        ROOTS ${_catrender_vulkan_roots}
        PATH_SUFFIXES include Include vulkan
        DOC "Vulkan header")
    if(CATRENDER_VULKAN_HEADER)
        get_filename_component(_catrender_vulkan_header_dir "${CATRENDER_VULKAN_HEADER}" DIRECTORY)
        get_filename_component(CATRENDER_VULKAN_INCLUDE_DIR "${_catrender_vulkan_header_dir}" DIRECTORY)
    endif()

    set(_catrender_library_roots
        "${CATRENDER_THIRDPART_DIR}"
        "${CATRENDER_THIRDPART_DIR}/lib"
        "${CATRENDER_THIRDPART_DIR}/Lib"
        "${CATRENDER_THIRDPART_DIR}/lib64"
        "${CATRENDER_THIRDPART_DIR}/bin"
        "${CATRENDER_THIRDPART_DIR}/Bin"
        "${CATRENDER_THIRDPART_DIR}/glfw"
        "${CATRENDER_THIRDPART_DIR}/glfw/lib"
        "${CATRENDER_THIRDPART_DIR}/glfw/Lib"
        "${CATRENDER_THIRDPART_DIR}/glfw/lib-vc2022"
        "${CATRENDER_THIRDPART_DIR}/glfw/lib-vc2019"
        "${CATRENDER_THIRDPART_DIR}/glfw/lib-vc2017"
        "${CATRENDER_THIRDPART_DIR}/glfw/lib-static-ucrt"
        "${CATRENDER_THIRDPART_DIR}/glfw/lib-mingw-w64"
        "${CATRENDER_THIRDPART_DIR}/Vulkan"
        "${CATRENDER_THIRDPART_DIR}/Vulkan/lib"
        "${CATRENDER_THIRDPART_DIR}/Vulkan/Lib"
        "${CATRENDER_THIRDPART_DIR}/vulkan"
        "${CATRENDER_THIRDPART_DIR}/vulkan/lib"
        "${CATRENDER_THIRDPART_DIR}/vulkan/Lib")
    list(REMOVE_DUPLICATES _catrender_library_roots)

    _catrender_find_library(CATRENDER_GLFW_LIBRARY
        NAMES glfw3 glfw3_mt glfw3dll
        ROOTS ${_catrender_library_roots}
        DOC "GLFW library")
    _catrender_find_library(CATRENDER_VULKAN_LIBRARY
        NAMES vulkan-1 vulkan
        ROOTS ${_catrender_library_roots}
        DOC "Vulkan loader library")

    set(_catrender_missing)
    if(NOT CATRENDER_GLFW_HEADER)
        list(APPEND _catrender_missing "GLFW headers: thirdpart/include/GLFW/glfw3.h")
    endif()
    if(NOT CATRENDER_GLFW_LIBRARY)
        list(APPEND _catrender_missing "GLFW library: thirdpart/lib/glfw3.lib (or a platform equivalent)")
    endif()
    if(NOT CATRENDER_GLM_HEADER)
        list(APPEND _catrender_missing "GLM headers: thirdpart/include/glm/glm.hpp")
    endif()
    if(NOT CATRENDER_STB_IMAGE_HEADER)
        list(APPEND _catrender_missing "stb_image header: thirdpart/include/stb_image/stb_image.h")
    endif()
    if(NOT CATRENDER_TINYOBJLOADER_HEADER)
        list(APPEND _catrender_missing "tinyobjloader header: thirdpart/include/tinyobjloader/tiny_obj_loader.h")
    endif()
    if(NOT CATRENDER_VULKAN_HEADER)
        list(APPEND _catrender_missing "Vulkan headers: thirdpart/include/vulkan/vulkan.h")
    endif()
    if(NOT CATRENDER_VULKAN_LIBRARY)
        list(APPEND _catrender_missing "Vulkan loader library: thirdpart/lib/vulkan-1.lib (or a platform equivalent)")
    endif()

    if(_catrender_missing)
        string(JOIN "\n  - " _catrender_missing_text ${_catrender_missing})
        message(FATAL_ERROR
            "CatRender third-party dependencies are incomplete:\n"
            "  - ${_catrender_missing_text}\n\n"
            "Put the missing files under the project ${CATRENDER_THIRDPART_LABEL}/ directory. "
            "See thirdpart/README.md.")
    endif()

    # The old source tree uses stb_image/tinyobjloader prefixes. These tiny
    # generated compatibility headers let either a package-style download or a
    # flat single-header download work without absolute paths in source files.
    set(_catrender_compat_include_dir
        "${CMAKE_CURRENT_BINARY_DIR}/generated/thirdpart/include")
    file(MAKE_DIRECTORY
        "${_catrender_compat_include_dir}/stb_image"
        "${_catrender_compat_include_dir}/tinyobjloader")
    file(WRITE "${_catrender_compat_include_dir}/stb_image/stb_image.h"
        "#pragma once\n#include <stb_image.h>\n")
    file(WRITE "${_catrender_compat_include_dir}/tinyobjloader/tiny_obj_loader.h"
        "#pragma once\n#include <tiny_obj_loader.h>\n")
    if(CATRENDER_GLFW_FLAT_HEADER)
        file(MAKE_DIRECTORY "${_catrender_compat_include_dir}/GLFW")
        file(WRITE "${_catrender_compat_include_dir}/GLFW/glfw3.h"
            "#pragma once\n#include <glfw3.h>\n")
    endif()

    set(_catrender_glfw_interface_include_dirs "${CATRENDER_GLFW_INCLUDE_DIR}")
    if(CATRENDER_GLFW_FLAT_HEADER)
        list(PREPEND _catrender_glfw_interface_include_dirs "${_catrender_compat_include_dir}")
    endif()
    _catrender_make_interface_target(catrender::GLFW "${_catrender_glfw_interface_include_dirs}")
    set_property(TARGET catrender::GLFW APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "${CATRENDER_GLFW_LIBRARY}")
    if(WIN32)
        set_property(TARGET catrender::GLFW APPEND PROPERTY
            INTERFACE_LINK_LIBRARIES "user32;gdi32;shell32")
    endif()
    if(CATRENDER_GLFW_LIBRARY MATCHES "glfw3dll")
        set_property(TARGET catrender::GLFW APPEND PROPERTY
            INTERFACE_COMPILE_DEFINITIONS GLFW_DLL)
    endif()

    _catrender_make_interface_target(catrender::GLM "${CATRENDER_GLM_INCLUDE_DIR}")
    _catrender_make_interface_target(catrender::STBImage
        "${_catrender_compat_include_dir};${CATRENDER_STB_IMAGE_INCLUDE_DIR}")
    _catrender_make_interface_target(catrender::TinyObjLoader
        "${_catrender_compat_include_dir};${CATRENDER_TINYOBJLOADER_INCLUDE_DIR}")
    _catrender_make_interface_target(catrender::Vulkan "${CATRENDER_VULKAN_INCLUDE_DIR}")
    set_property(TARGET catrender::Vulkan APPEND PROPERTY
        INTERFACE_LINK_LIBRARIES "${CATRENDER_VULKAN_LIBRARY}")

    if(CATRENDER_GLFW_LIBRARY MATCHES "glfw3dll")
        _catrender_find_file(CATRENDER_GLFW_RUNTIME_DLL
            NAMES glfw3.dll
            ROOTS ${_catrender_library_roots}
            PATH_SUFFIXES bin Bin lib-vc2022 lib-vc2019 lib-vc2017
            DOC "GLFW runtime DLL")
        if(CATRENDER_GLFW_RUNTIME_DLL)
            set(CATRENDER_GLFW_RUNTIME_DLL
                "${CATRENDER_GLFW_RUNTIME_DLL}" PARENT_SCOPE)
        endif()
    endif()

    _catrender_find_file(CATRENDER_VULKAN_RUNTIME_DLL
        NAMES vulkan-1.dll
        ROOTS ${_catrender_library_roots}
        PATH_SUFFIXES bin Bin
        DOC "Vulkan loader runtime DLL")
    if(CATRENDER_VULKAN_RUNTIME_DLL)
        set(CATRENDER_VULKAN_RUNTIME_DLL
            "${CATRENDER_VULKAN_RUNTIME_DLL}" PARENT_SCOPE)
    endif()

    mark_as_advanced(
        CATRENDER_GLFW_HEADER
        CATRENDER_GLM_HEADER
        CATRENDER_STB_IMAGE_HEADER
        CATRENDER_TINYOBJLOADER_HEADER
        CATRENDER_VULKAN_HEADER
        CATRENDER_GLFW_LIBRARY
        CATRENDER_VULKAN_LIBRARY
        CATRENDER_GLFW_RUNTIME_DLL
        CATRENDER_VULKAN_RUNTIME_DLL)

    message(STATUS "CatRender thirdpart root: ${CATRENDER_THIRDPART_LABEL}/")
    message(STATUS "GLFW header: found")
    message(STATUS "GLFW library: found")
    message(STATUS "GLM header: found")
    message(STATUS "stb_image header: found")
    message(STATUS "tinyobjloader header: found")
    message(STATUS "Vulkan header: found")
    message(STATUS "Vulkan library: found")
endfunction()
