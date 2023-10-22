if (HAVE_VIDEO_CODEC_FFMPEG)
    file(GLOB video_codec_ffmpeg_hdrs
            ${CMAKE_CURRENT_SOURCE_DIR}/*.h
            )
    file(GLOB video_codec_ffmpeg_sources
            ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
            )

     set(LIB_NAME "kkrtc_video_codec_ffmpeg")
    # 动态库
    add_library(${LIB_NAME} MODULE ${video_codec_ffmpeg_hdrs} ${video_codec_ffmpeg_sources})
    target_link_libraries(${LIB_NAME}   kkrtc_module_core kkrtc_video_coding_core)
    set_target_properties(${LIB_NAME} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)
endif ()
