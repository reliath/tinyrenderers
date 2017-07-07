cmake_minimum_required(VERSION 3.0)

project(samples)

include_directories(${tinyrenders_include_dir})

include_directories(${glfw_INCLUDE_DIRS})
link_libraries(glfw)

include_directories(${VULKAN_INCLUDE_DIR})
link_libraries(${VULKAN_LIBRARY})

function(add_vk sample_name)
    set(target_name "${sample_name}_VK")
    add_executable(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/${sample_name}.cpp
                                  ${CMAKE_SOURCE_DIR}/tinyvk.h)
    target_compile_definitions(${target_name} PRIVATE -DTINY_RENDERER_VK)
    if(UNIX)
        target_link_libraries(${target_name} PRIVATE X11-xcb)
    elseif(WIN32)
        set_target_properties(${target_name} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:Windows /INCREMENTAL:NO")
        set_target_properties(${target_name} PROPERTIES FOLDER "hlsl-iovars/vk")
    endif()
endfunction()

add_vk(Ex01)
add_vk(Ex02)
add_vk(Ex03)
add_vk(Ex04)
add_vk(Ex05)

if(WIN32)
    function(add_dx sample_name)
        set(target_name "${sample_name}_DX")
        add_executable(${target_name} ${CMAKE_CURRENT_SOURCE_DIR}/src/${sample_name}.cpp
                                      ${CMAKE_CURRENT_SOURCE_DIR}/data/${sample_name}.vs.hlsl
                                      ${CMAKE_CURRENT_SOURCE_DIR}/data/${sample_name}.ps.hlsl
                                      ${CMAKE_SOURCE_DIR}/tinydx.h)
        set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/data/${sample_name}.vs.hlsl
                                    ${CMAKE_CURRENT_SOURCE_DIR}/data/${sample_name}.ps.hlsl
                                    PROPERTIES HEADER_FILE_ONLY TRUE)
        target_compile_definitions(${target_name} PRIVATE -DTINY_RENDERER_DX)
        set_target_properties(${target_name} PROPERTIES LINK_FLAGS "/ENTRY:mainCRTStartup /SUBSYSTEM:Windows /INCREMENTAL:NO")
        set_target_properties(${target_name} PROPERTIES FOLDER "hlsl-iovars/dx")
    endfunction()

    add_dx(Ex01)
    add_dx(Ex02)
    add_dx(Ex03)
    add_dx(Ex04)
    add_dx(Ex05)
endif()
