

add_subdirectory(utils)
add_subdirectory(system)
add_subdirectory(video)
add_subdirectory(memory)
message("3RDPARTY_PATH:${3RDPARTY_PATH}")
file(GLOB kkrtc_rtc_base_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB kkrtc_rtc_base_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
add_library(kkrtc_base
        ${kkrtc_utils_hdrs}
        ${kkrtc_utils_srcs}
        ${kkrtc_system_hdrs}
        ${kkrtc_system_srcs}
        ${kkrtc_video_hdrs}
        ${kkrtc_video_srcs}
        ${kkrtc_rtc_base_hdrs}
        ${kkrtc_rtc_base_srcs}
        ${kkrtc_mem_srcs}
        ${kkrtc_mem_hdrs}
        )
#target_link_libraries(kkrtc_base_utils PRIVATE spdlog::spdlog_header_only)
set_target_properties(kkrtc_base PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_definitions(-DSPDLOG_HEADER_ONLY)
add_definitions(-DSPDLOG_NO_EXCEPTIONS)
target_include_directories(kkrtc_base PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/system
        ${CMAKE_CURRENT_SOURCE_DIR}/utils
        ${CMAKE_CURRENT_SOURCE_DIR}/video
        ${CMAKE_CURRENT_SOURCE_DIR}/memory
        ${3RDPARTY_PATH}/libyuv/include
        ${3RDPARTY_PATH}/abseil-cpp
        ${3RDPARTY_PATH}/spdlog/include
        ${3RDPARTY_PATH}
        ${CMAKE_CURRENT_SOURCE_DIR}/../include
        )
target_link_libraries(kkrtc_base  libyuv)
list(APPEND kkrtc_libs kkrtc_base)
set(kkrtc_libs "${kkrtc_libs}" CACHE INTERNAL "KKRTC Libraries")