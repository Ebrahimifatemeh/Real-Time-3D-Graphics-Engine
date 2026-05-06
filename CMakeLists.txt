cmake_minimum_required(VERSION 3.28)
project(asr)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SDL3WEBGPU_SOURCE
    "vendor/sdl3webgpu/sdl3webgpu.c"
)
set(ASR_SOURCES
    "include/asr.h"
    ${SDL3WEBGPU_SOURCE}
)
set(ASR_LIBRARIES
    dawn::webgpu_dawn
    SDL3::SDL3
    glm::glm
)

find_package(Dawn CONFIG REQUIRED)
find_package(SDL3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

include_directories(
    "./include"
    "./vendor/sdl3webgpu"
    "${Stb_INCLUDE_DIR}"
)
if(WIN32)
    find_package(directx-dxc CONFIG REQUIRED)
endif()

if(APPLE)
    set_source_files_properties(${SDL3WEBGPU_SOURCE} PROPERTIES
        COMPILE_FLAGS "-x objective-c"
    )
    find_library(COCOA_LIBRARY Cocoa REQUIRED)
    find_library(QUARTZCORE_LIBRARY QuartzCore REQUIRED)
    find_library(METAL_LIBRARY Metal REQUIRED)
    find_library(IOKIT_LIBRARY IOKit REQUIRED)
    find_library(COREVIDEO_LIBRARY CoreVideo REQUIRED)
    list(APPEND ASR_LIBRARIES
        ${COCOA_LIBRARY}
        ${QUARTZCORE_LIBRARY}
        ${METAL_LIBRARY}
        ${IOKIT_LIBRARY}
        ${COREVIDEO_LIBRARY}
    )
endif()

function(copy_dlls_for_target TARGET_NAME)
    if(WIN32)
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:dawn::webgpu_dawn>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:SDL3::SDL3>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                $<TARGET_FILE:Microsoft::DirectXShaderCompiler>
                $<TARGET_FILE_DIR:${TARGET_NAME}>
        )
        get_target_property(DXC_DLL_LOCATION Microsoft::DirectXShaderCompiler IMPORTED_LOCATION_RELEASE)
        if(NOT DXC_DLL_LOCATION)
            get_target_property(DXC_DLL_LOCATION Microsoft::DirectXShaderCompiler IMPORTED_LOCATION_DEBUG)
        endif()
        if(NOT DXC_DLL_LOCATION)
            get_target_property(DXC_DLL_LOCATION Microsoft::DirectXShaderCompiler IMPORTED_LOCATION)
        endif()
        if(DXC_DLL_LOCATION)
            get_filename_component(DXC_DLL_DIR "${DXC_DLL_LOCATION}" DIRECTORY)
            if(EXISTS "${DXC_DLL_DIR}/dxil.dll")
                add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_if_different
                        "${DXC_DLL_DIR}/dxil.dll"
                        $<TARGET_FILE_DIR:${TARGET_NAME}>
                )
            endif()
        endif()
        set(SYSTEM32_DIR "$ENV{SystemRoot}/System32")
        if(EXISTS "${SYSTEM32_DIR}/d3dcompiler_47.dll")
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SYSTEM32_DIR}/d3dcompiler_47.dll"
                    $<TARGET_FILE_DIR:${TARGET_NAME}>
            )
        endif()
        if(EXISTS "${SYSTEM32_DIR}/vulkan-1.dll")
            add_custom_command(TARGET ${TARGET_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy_if_different
                    "${SYSTEM32_DIR}/vulkan-1.dll"
                    $<TARGET_FILE_DIR:${TARGET_NAME}>
            )
        endif()
    endif()
endfunction()

add_executable(hello_world_test ${ASR_SOURCES} "tests/hello_world_test.cpp")
target_link_libraries(hello_world_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(hello_world_test)

add_executable(circle_test ${ASR_SOURCES} "tests/circle_test.cpp")
target_link_libraries(circle_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(circle_test)

add_executable(rectangle_test ${ASR_SOURCES} "tests/rectangle_test.cpp")
target_link_libraries(rectangle_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(rectangle_test)

add_executable(triangle_test ${ASR_SOURCES} "tests/triangle_test.cpp")
target_link_libraries(triangle_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(triangle_test)

add_executable(sphere_test ${ASR_SOURCES} "tests/sphere_test.cpp")
target_link_libraries(sphere_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(sphere_test)

add_executable(box_test ${ASR_SOURCES} "tests/box_test.cpp")
target_link_libraries(box_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(box_test)

add_executable(ring_test ${ASR_SOURCES} "tests/ring_test.cpp")
target_link_libraries(ring_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(ring_test)

add_executable(transform1_test ${ASR_SOURCES} "tests/transform1_test.cpp")
target_link_libraries(transform1_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(transform1_test)

add_executable(transform2_test ${ASR_SOURCES} "tests/transform2_test.cpp")
target_link_libraries(transform2_test PRIVATE ${ASR_LIBRARIES})
copy_dlls_for_target(transform2_test)