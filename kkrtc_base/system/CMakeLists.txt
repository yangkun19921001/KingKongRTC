file(GLOB kkrtc_system_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB kkrtc_system_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
set(kkrtc_system_hdrs "${kkrtc_system_hdrs}" CACHE INTERNAL "KKRTC Libraries")
set(kkrtc_system_srcs "${kkrtc_system_srcs}" CACHE INTERNAL "KKRTC Libraries")