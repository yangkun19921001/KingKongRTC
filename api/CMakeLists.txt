
add_subdirectory(video)
set(GLOB kkrtc_api_hards
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

add_library(kkrtc_api
        ${kkrtc_api_video_hdrs}
        ${kkrtc_api_video_srcs}
        ${kkrtc_api_hards}
        ${kkrtc_api_srcs}
        )
target_link_libraries(kkrtc_api kkrtc_base)
set_target_properties(kkrtc_api PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)


target_include_directories(kkrtc_api PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/video
        )
list(APPEND kkrtc_libs kkrtc_api)
set(kkrtc_libs "${kkrtc_libs}" CACHE INTERNAL "KKRTC Libraries")





