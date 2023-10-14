file(GLOB kkrtc_utils_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB kkrtc_utils_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
set(kkrtc_utils_hdrs "${kkrtc_utils_hdrs}" CACHE INTERNAL "KKRTC Libraries")
set(kkrtc_utils_srcs "${kkrtc_utils_srcs}" CACHE INTERNAL "KKRTC Libraries")