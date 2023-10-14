
if(KKRTC_WIN)
    add_subdirectory(win_capture)
elseif(KKRTC_MAC OR KKRTC_IOS)
    add_subdirectory(objc_capture)
elseif(KKRTC_ANDROID)
    add_subdirectory(android_capture)
endif()