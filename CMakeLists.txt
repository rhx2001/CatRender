cmake_minimum_required(VERSION 3.24)

project(CatRender VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(CATRENDER_COMPILE_SHADERS "Compile GLSL shaders with glslc when it is available" ON)

# Keep all non-system dependencies under the current project. The correctly
# spelled thirdpart directory is preferred; thridpart is accepted as a
# compatibility name because it has been used in local workspaces.
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thirdpart")
    set(CATRENDER_THIRDPART_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdpart")
    set(CATRENDER_THIRDPART_LABEL "thirdpart")
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/thridpart")
    set(CATRENDER_THIRDPART_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thridpart")
    set(CATRENDER_THIRDPART_LABEL "thridpart")
else()
    set(CATRENDER_THIRDPART_DIR "${CMAKE_CURRENT_SOURCE_DIR}/thirdpart")
    set(CATRENDER_THIRDPART_LABEL "thirdpart")
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(ThirdPart)
catrender_configure_thirdpart()

set(CATRENDER_PROJECT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/CatRenderer")

set(CATRENDER_ENGINE_SOURCES
    CatRenderer/main.cpp
    CatRenderer/Src/App/CatRender.cpp
    CatRenderer/Src/Gui/GUIManager.cpp
    CatRenderer/Src/Renderer/Camera.cpp
    CatRenderer/Src/Renderer/VulkanCore.cpp
    CatRenderer/Src/ResourceManager/BufferManager.cpp
    CatRenderer/Src/ResourceManager/Material.cpp
    CatRenderer/Src/ResourceManager/MaterialManager.cpp
    CatRenderer/Src/ResourceManager/Mesh.cpp
    CatRenderer/Src/ResourceManager/ModelInstance.cpp
    CatRenderer/Src/ResourceManager/ModelManager.cpp
    CatRenderer/Include/ResourceManager/MaterialViewer.cpp
    CatRenderer/Src/Window/Window.cpp
)

set(CATRENDER_ENGINE_HEADERS
    CatRenderer/Include/App/CatRender.h
    CatRenderer/Include/Gui/GUIManager.h
    CatRenderer/Include/Renderer/Camera.hpp
    CatRenderer/Include/Renderer/VulkanCore.h
    CatRenderer/Include/ResourceManager/BufferManager.h
    CatRenderer/Include/ResourceManager/Material.h
    CatRenderer/Include/ResourceManager/MaterialManager.h
    CatRenderer/Include/ResourceManager/MaterialViewer.h
    CatRenderer/Include/ResourceManager/Mesh.h
    CatRenderer/Include/ResourceManager/ModelInstance.h
    CatRenderer/Include/ResourceManager/ModelManager.h
    CatRenderer/Include/Window/window.h
)

# ImGui is already vendored in CatRenderer/Include/Gui. It is kept as a separate
# target so the application target only contains CatRender code.
set(CATRENDER_IMGUI_SOURCES
    CatRenderer/Include/Gui/imgui.cpp
    CatRenderer/Include/Gui/imgui_demo.cpp
    CatRenderer/Include/Gui/imgui_draw.cpp
    CatRenderer/Include/Gui/imgui_tables.cpp
    CatRenderer/Include/Gui/imgui_widgets.cpp
    CatRenderer/Include/Gui/imgui_impl_glfw.cpp
    CatRenderer/Include/Gui/imgui_impl_vulkan.cpp
)

add_library(catrender_imgui STATIC ${CATRENDER_IMGUI_SOURCES})
target_include_directories(catrender_imgui
    PUBLIC
        "${CATRENDER_PROJECT_DIR}/Include"
        "${CATRENDER_PROJECT_DIR}/Include/Gui"
)
target_link_libraries(catrender_imgui
    PUBLIC
        catrender::GLFW
        catrender::Vulkan
)

add_executable(CatRender
    ${CATRENDER_ENGINE_SOURCES}
    ${CATRENDER_ENGINE_HEADERS}
)

target_include_directories(CatRender
    PRIVATE
        "${CATRENDER_PROJECT_DIR}/Include"
)
target_link_libraries(CatRender
    PRIVATE
        catrender_imgui
        catrender::GLFW
        catrender::GLM
        catrender::STBImage
        catrender::TinyObjLoader
        catrender::Vulkan
)

if(MSVC)
    target_compile_options(CatRender PRIVATE /W4 /permissive-)
    target_compile_options(catrender_imgui PRIVATE /W3 /permissive-)
else()
    target_compile_options(CatRender PRIVATE -Wall -Wextra -Wpedantic)
    target_compile_options(catrender_imgui PRIVATE -Wall -Wextra -Wpedantic)
endif()

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/CatRenderer" FILES
    ${CATRENDER_ENGINE_SOURCES}
    ${CATRENDER_ENGINE_HEADERS}
    ${CATRENDER_IMGUI_SOURCES}
)

# The renderer reads resources relative to its working directory. Copy the
# runtime tree next to the executable so a CMake build does not depend on an
# old Visual Studio working path.
set(CATRENDER_SHADER_OUTPUT_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated/shaders")
set(CATRENDER_RUNTIME_SHADER_FILES)

set(_catrender_glslc_hints
    "${CATRENDER_THIRDPART_DIR}/bin"
    "${CATRENDER_THIRDPART_DIR}/Bin"
    "${CATRENDER_THIRDPART_DIR}/vulkan/bin"
    "${CATRENDER_THIRDPART_DIR}/vulkan/Bin"
    "${CATRENDER_THIRDPART_DIR}/Vulkan/bin"
    "${CATRENDER_THIRDPART_DIR}/Vulkan/Bin")
find_program(CATRENDER_GLSLC_EXECUTABLE
    NAMES glslc glslc.exe
    HINTS ${_catrender_glslc_hints}
    NO_DEFAULT_PATH)

if(CATRENDER_COMPILE_SHADERS AND CATRENDER_GLSLC_EXECUTABLE)
    set(_catrender_shader_outputs)
    foreach(_shader_name IN ITEMS vert frag)
        set(_shader_source "${CATRENDER_PROJECT_DIR}/shaders/Shader.${_shader_name}")
        set(_shader_output "${CATRENDER_SHADER_OUTPUT_DIR}/${_shader_name}.spv")
        add_custom_command(
            OUTPUT "${_shader_output}"
            COMMAND "${CMAKE_COMMAND}" -E make_directory "${CATRENDER_SHADER_OUTPUT_DIR}"
            COMMAND "${CATRENDER_GLSLC_EXECUTABLE}" "${_shader_source}" -o "${_shader_output}"
            DEPENDS "${_shader_source}"
            COMMENT "Compiling Shader.${_shader_name}"
            VERBATIM
        )
        list(APPEND _catrender_shader_outputs "${_shader_output}")
    endforeach()
    add_custom_target(CatRenderShaders DEPENDS ${_catrender_shader_outputs})
    add_dependencies(CatRender CatRenderShaders)
    set(CATRENDER_RUNTIME_SHADER_FILES ${_catrender_shader_outputs})
else()
    if(CATRENDER_COMPILE_SHADERS AND NOT CATRENDER_GLSLC_EXECUTABLE)
        message(WARNING
            "glslc was not found. Existing CatRenderer/shaders/*.spv files will be used. "
            "Put glslc in ${CATRENDER_THIRDPART_LABEL}/bin to compile shaders.")
    endif()
    set(CATRENDER_RUNTIME_SHADER_FILES
        "${CATRENDER_PROJECT_DIR}/shaders/vert.spv"
        "${CATRENDER_PROJECT_DIR}/shaders/frag.spv")
endif()

foreach(_shader_file IN LISTS CATRENDER_RUNTIME_SHADER_FILES)
    get_filename_component(_shader_file_name "${_shader_file}" NAME)
    add_custom_command(TARGET CatRender POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E make_directory "$<TARGET_FILE_DIR:CatRender>/shaders"
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${_shader_file}"
            "$<TARGET_FILE_DIR:CatRender>/shaders/${_shader_file_name}"
        VERBATIM
    )
endforeach()

if(EXISTS "${CATRENDER_PROJECT_DIR}/textures")
    add_custom_command(TARGET CatRender POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CATRENDER_PROJECT_DIR}/textures"
            "$<TARGET_FILE_DIR:CatRender>/textures"
        VERBATIM
    )
endif()

if(EXISTS "${CATRENDER_PROJECT_DIR}/models")
    add_custom_command(TARGET CatRender POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_directory
            "${CATRENDER_PROJECT_DIR}/models"
            "$<TARGET_FILE_DIR:CatRender>/models"
        VERBATIM
    )
else()
    message(STATUS
        "CatRenderer/models is not present. The executable still builds, but the default model files "
        "must be supplied before running the renderer.")
endif()

if(EXISTS "${CATRENDER_PROJECT_DIR}/imgui.ini")
    add_custom_command(TARGET CatRender POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CATRENDER_PROJECT_DIR}/imgui.ini"
            "$<TARGET_FILE_DIR:CatRender>/imgui.ini"
        VERBATIM
    )
endif()

if(CATRENDER_GLFW_RUNTIME_DLL)
    add_custom_command(TARGET CatRender POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CATRENDER_GLFW_RUNTIME_DLL}"
            "$<TARGET_FILE_DIR:CatRender>/glfw3.dll"
        VERBATIM
    )
endif()

if(CATRENDER_VULKAN_RUNTIME_DLL)
    add_custom_command(TARGET CatRender POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy_if_different
            "${CATRENDER_VULKAN_RUNTIME_DLL}"
            "$<TARGET_FILE_DIR:CatRender>/vulkan-1.dll"
        VERBATIM
    )
endif()

if(CMAKE_GENERATOR MATCHES "Visual Studio")
    set_property(TARGET CatRender PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "$<TARGET_FILE_DIR:CatRender>")
endif()

install(TARGETS CatRender RUNTIME DESTINATION .)
install(DIRECTORY "${CATRENDER_PROJECT_DIR}/textures" DESTINATION .)
if(EXISTS "${CATRENDER_PROJECT_DIR}/models")
    install(DIRECTORY "${CATRENDER_PROJECT_DIR}/models" DESTINATION .)
endif()
