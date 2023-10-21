file(GLOB kkrtc_video_cap_core_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include/*.h
        )
file(GLOB kkrtc_video_cap_core_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/core/src/*.cpp
        )

add_library(kkrtc_video_cap_core ${kkrtc_video_cap_core_hdrs} ${kkrtc_video_cap_core_srcs})
target_link_libraries(kkrtc_video_cap_core PRIVATE kkrtc_module_core)
set_target_properties(kkrtc_video_cap_core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


target_include_directories(kkrtc_video_cap_core PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/core/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../core
        )
list(APPEND kkrtc_libs kkrtc_video_cap_core)
set(kkrtc_libs "${kkrtc_libs}" CACHE INTERNAL "KKRTC Libraries")

add_subdirectory(plugins)

