file(GLOB video_module_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB video_module_srcs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
add_library(kkrtc_module_core ${video_module_hdrs} ${video_module_srcs})
target_link_libraries(kkrtc_module_core kkrtc_base kkrtc_api)
set_target_properties(kkrtc_module_core PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
target_include_directories(kkrtc_module_core PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
list(APPEND kkrtc_libs kkrtc_module_core)
set(kkrtc_libs "${kkrtc_libs}" CACHE INTERNAL "KKRTC Libraries")


