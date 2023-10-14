

if(CMAKE_COMPILER_IS_GNUCC
        OR CMAKE_COMPILER_IS_GNUCXX
        OR CMAKE_COMPILER_IS_CLANG)
    set(CMAKE_CXX_FLAGS
            "-Wall -Wextra -Wno-unused-function -Werror-implicit-function-declaration -Wno-missing-field-initializers ${CMAKE_CXX_FLAGS} -fno-strict-aliasing"
            )
    set(CMAKE_C_FLAGS
            "-Wall -Wextra -Wno-unused-function -Werror-implicit-function-declaration -Wno-missing-braces -Wno-missing-field-initializers ${CMAKE_C_FLAGS} -std=gnu99 -fno-strict-aliasing"
            )

    option(USE_LIBC++ "Use libc++ instead of libstdc++" ${APPLE})
    if(USE_LIBC++)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    endif()
elseif(MSVC)
    if(CMAKE_CXX_FLAGS MATCHES "/W[0-4]")
        string(REGEX REPLACE "/W[0-4]" "/W4" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    endif()

    # Disable pointless constant condition warnings
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4127 /wd4201")
endif()

if(WIN32)
    add_definitions(-DUNICODE -D_UNICODE)
    if(BUILD_SHARED_LIBS)
        add_definitions(-DDSHOWCAPTURE_EXPORTS)
    endif()
endif()

if(MSVC)
    set(CMAKE_C_FLAGS_DEBUG "/DDEBUG=1 /D_DEBUG=1 ${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG "/DDEBUG=1 /D_DEBUG=1 ${CMAKE_C_FLAGS_DEBUG}")

    if(NOT CMAKE_SIZEOF_VOID_P EQUAL 8)
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SAFESEH:NO")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} /SAFESEH:NO")
        set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} /SAFESEH:NO")
    endif()
else()
    if(MINGW)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -D_WIN32_WINNT=0x0600 -DWINVER=0x0600")
        set(CMAKE_CXX_FLAGS
                "${CMAKE_CXX_FLAGS} -D_WIN32_WINNT=0x0600 -DWINVER=0x0600")
    endif()
    set(CMAKE_C_FLAGS_DEBUG "-DDEBUG=1 -D_DEBUG=1 ${CMAKE_C_FLAGS_DEBUG}")
    set(CMAKE_CXX_FLAGS_DEBUG "-DDEBUG=1 -D_DEBUG=1 ${CMAKE_CXX_FLAGS_DEBUG}")
endif()

if(MINGW)
    include(CheckSymbolExists)
    check_symbol_exists(MINGW_HAS_SECURE_API "_mingw.h" HAVE_MINGW_HAS_SECURE_API)
    if(NOT HAVE_MINGW_HAS_SECURE_API)
        message(FATAL_ERROR "mingw must be compiled with --enable-secure-api")
    endif()
endif()

set(libdshowcapture_SOURCES
        ${3RDPARTY_PATH}/libdshowcapture/external/capture-device-support/Library/EGAVResult.cpp
        ${3RDPARTY_PATH}/libdshowcapture/external/capture-device-support/Library/ElgatoUVCDevice.cpp
        ${3RDPARTY_PATH}/libdshowcapture/external/capture-device-support/Library/win/EGAVHIDImplementation.cpp
        ${3RDPARTY_PATH}/libdshowcapture/external/capture-device-support/SampleCode/DriverInterface.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/capture-filter.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/output-filter.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshowcapture.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshowencode.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/device.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/device-vendor.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/encoder.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-base.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-demux.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-enum.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-formats.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-media-type.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-encoded-device.cpp
        ${3RDPARTY_PATH}/libdshowcapture/source/log.cpp)

set(libdshowcapture_HEADERS
        ${3RDPARTY_PATH}/libdshowcapture/dshowcapture.h
        ${3RDPARTY_PATH}/libdshowcapture/source/external/IVideoCaptureFilter.h
        ${3RDPARTY_PATH}/libdshowcapture/source/capture-filter.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/output-filter.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/device.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/encoder.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-base.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-demux.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-device-defs.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-enum.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-formats.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/dshow-media-type.hpp
        ${3RDPARTY_PATH}/libdshowcapture/source/log.hpp)

add_library(libdshowcapture ${libdshowcapture_SOURCES} ${libdshowcapture_HEADERS})
set_target_properties(libdshowcapture PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin/plugins)

target_include_directories(
        libdshowcapture
        PRIVATE
        ${3RDPARTY_PATH}/libdshowcapture/
        ${3RDPARTY_PATH}/libdshowcapture/external/capture-device-support/Library
)

target_compile_definitions(libdshowcapture PRIVATE _UP_WINDOWS=1)

target_link_libraries(libdshowcapture PRIVATE setupapi strmiids ksuser winmm
        wmcodecdspuuid)

