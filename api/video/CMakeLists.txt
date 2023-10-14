file(GLOB kkrtc_api_video_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB kkrtc_api_video_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
set(kkrtc_api_video_hdrs "${kkrtc_api_video_hdrs}" CACHE INTERNAL "KKRTC Libraries")
set(kkrtc_api_video_srcs "${kkrtc_api_video_srcs}" CACHE INTERNAL "KKRTC Libraries")