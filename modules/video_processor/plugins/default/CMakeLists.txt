if (HAVE_VPROCESSOR_DEFAULT)
    file(GLOB vp_def_hdrs
            ${CMAKE_CURRENT_SOURCE_DIR}/*.h
            )
    file(GLOB vp_def_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
            )

    set(LIB_NAME "")
    if(KKRTC_WIN)
        set(LIB_NAME "kkrtc_win_video_processor_default")
    elseif(KKRTC_MAC)
        set(LIB_NAME "kkrtc_mac_video_processor_default")
    elseif(KKRTC_IOS)
        set(LIB_NAME "kkrtc_ios_video_processor_default")
    elseif(KKRTC_ANDROID)
        set(LIB_NAME "kkrtc_android_video_processor_default")
    endif()

    set(LIB_NAME "kkrtc_video_processor_default")
    # 动态库
    add_library(${LIB_NAME} MODULE ${vp_def_hdrs} ${vp_def_sources})
    target_link_libraries(${LIB_NAME}   kkrtc_module_core kkrtc_video_pro_core)
    set_target_properties(${LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)
endif ()
