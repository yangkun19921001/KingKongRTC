add_subdirectory(ffmpeg)


if (KKRTC_WIN)
elseif (KKRTC_MAC OR KKRTC_IOS)
elseif (KKRTC_ANDROID)
endif ()