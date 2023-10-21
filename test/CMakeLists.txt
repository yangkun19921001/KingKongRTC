

#kkrtc_libs
#set(kkrtc_libs ${kkrtc_module_core})
#message("kkrtc_libs: ${kkrtc_libs}")

set(OUT_EXECUTABLE_CAP_NAME "")
set(OUT_EXECUTABLE_LOG_NAME "")
set(OUT_EXECUTABLE_CAP_PLUGIN_NAME "")
set(OUT_EXECUTABLE_VIDEO_BROADCASTER_NAME "")
set(OUT_EXECUTABLE_VIDEO_PROCESSOR_NAME "")
if (KKRTC_WIN)
    add_executable(${PROJECT_NAME}-win_camera_capture win_camera_capture.cpp)
    set(OUT_EXECUTABLE_CAP_NAME ${PROJECT_NAME}-win_camera_capture)

    add_executable(${PROJECT_NAME}-win_log test_log.cpp)
    set(OUT_EXECUTABLE_LOG_NAME ${PROJECT_NAME}-win_log)

    add_executable(${PROJECT_NAME}-test-cap-plugin test_vcap_plugin.cpp)
    set(OUT_EXECUTABLE_CAP_PLUGIN_NAME ${PROJECT_NAME}-test-cap-plugin)

    add_executable(${PROJECT_NAME}-test_video_broadcaster test_video_broadcaster.cpp)
    set(OUT_EXECUTABLE_VIDEO_BROADCASTER_NAME ${PROJECT_NAME}-test_video_broadcaster)

    add_executable(${PROJECT_NAME}-test_video_processor test_video_processor.cpp)
    set(OUT_EXECUTABLE_VIDEO_PROCESSOR_NAME ${PROJECT_NAME}-test_video_processor)
elseif (KKRTC_MAC)
    add_executable(${PROJECT_NAME}-mac_camera_capture mac_camera_capture.cpp)
    set(OUT_EXECUTABLE_CAP_NAME ${PROJECT_NAME}-mac_camera_capture)
elseif (KKRTC_LINUX)
    add_executable(${PROJECT_NAME}-linux_camera_capture linux_camera_capture.cpp)
    set(OUT_EXECUTABLE_CAP_NAME ${PROJECT_NAME}-linux_camera_capture)
endif ()

set(KKRTC_BASE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../kkrtc_base
        )
target_include_directories(${OUT_EXECUTABLE_CAP_NAME} PRIVATE ${KKRTC_BASE_DIRS}
        )
target_include_directories(${OUT_EXECUTABLE_LOG_NAME} PRIVATE ${KKRTC_BASE_DIRS}
        )
target_include_directories(${OUT_EXECUTABLE_CAP_PLUGIN_NAME} PRIVATE ${KKRTC_BASE_DIRS}
        )
message("${KKRTC_BASE_INCLUDE_DIRS}")
target_link_libraries(${OUT_EXECUTABLE_CAP_NAME} PRIVATE ${kkrtc_libs})
target_link_libraries(${OUT_EXECUTABLE_LOG_NAME} PRIVATE ${kkrtc_libs})
target_link_libraries(${OUT_EXECUTABLE_CAP_PLUGIN_NAME} PRIVATE ${kkrtc_libs})
target_link_libraries(${OUT_EXECUTABLE_VIDEO_BROADCASTER_NAME} PRIVATE ${kkrtc_libs})
target_link_libraries(${OUT_EXECUTABLE_VIDEO_PROCESSOR_NAME} PRIVATE ${kkrtc_libs})