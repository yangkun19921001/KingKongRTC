add_subdirectory(libdshowcapture)
file(GLOB win_cap_hdrs
        ${CMAKE_CURRENT_SOURCE_DIR}/*.h
        )
file(GLOB win_cap_sources
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )
message("libds->${libdshowcapture}")
# 动态库
add_library(kkrtc_win_dshow MODULE ${win_cap_hdrs} ${win_cap_sources})
target_link_libraries(kkrtc_win_dshow  libdshowcapture kkrtc_module_core kkrtc_video_cap_core)
set_target_properties(kkrtc_win_dshow PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)
target_include_directories(
        kkrtc_win_dshow
        PRIVATE
        ${3RDPARTY_PATH}/libdshowcapture/
)