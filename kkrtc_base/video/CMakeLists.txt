
add_subdirectory(libyuv)
file(GLOB kkrtc_video_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/libyuv/*.h
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB kkrtc_video_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
        ${CMAKE_CURRENT_SOURCE_DIR}/libyuv/*.cpp
        )
set(kkrtc_video_hdrs "${kkrtc_video_hdrs}" CACHE INTERNAL "KKRTC Libraries")
set(kkrtc_video_srcs "${kkrtc_video_srcs}" CACHE INTERNAL "KKRTC Libraries")