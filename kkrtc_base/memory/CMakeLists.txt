file(GLOB kkrtc_mem_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB kkrtc_mem_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cc
        )
set(kkrtc_mem_hdrs "${kkrtc_mem_hdrs}" CACHE INTERNAL "KKRTC Libraries")
set(kkrtc_mem_srcs "${kkrtc_mem_srcs}" CACHE INTERNAL "KKRTC Libraries")