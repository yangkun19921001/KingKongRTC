//
// Created by devyk on 2023/9/18.
//

#ifndef KKRTC_FILESYSTEM_PRIVATE_H
#define KKRTC_FILESYSTEM_PRIVATE_H



// TODO Move to CMake?
#ifndef KKRTC_HAVE_FILESYSTEM_SUPPORT
#  if defined(__EMSCRIPTEN__) || defined(__native_client__)
/* no support */
#  elif defined WINRT || defined _WIN32_WCE
/* not supported */
#  elif defined __ANDROID__ || defined __linux__ || defined _WIN32 || \
        defined __FreeBSD__ || defined __bsdi__ || defined __HAIKU__
#      define KKRTC_HAVE_FILESYSTEM_SUPPORT 1
#  elif defined(__APPLE__)
#    include <TargetConditionals.h>
#    if (defined(TARGET_OS_OSX) && TARGET_OS_OSX) || (defined(TARGET_OS_IOS) && TARGET_OS_IOS)
#      define KKRTC_HAVE_FILESYSTEM_SUPPORT 1 // OSX, iOS only
#    endif
#  else
/* unknown */
#  endif
#  ifndef KKRTC_HAVE_FILESYSTEM_SUPPORT
#    define KKRTC_HAVE_FILESYSTEM_SUPPORT 0
#  endif
#endif

#if KKRTC_HAVE_FILESYSTEM_SUPPORT

namespace kkrtc {
    namespace utils {
        namespace fs {

            /**
             * File-based lock object.
             *
             * Provides interprocess synchronization mechanism.
             * Platform dependent.
             *
             * Supports multiple readers / single writer access pattern (RW / readers-writer / shared-exclusive lock).
             *
             * File must exist.
             * File can't be re-used (for example, I/O operations via std::fstream is not safe)
             */
            class FileLock {
            public:
                explicit FileLock(const char *fname);

                ~FileLock();

                void lock(); //< acquire exclusive (writer) lock
                void unlock(); //< release exclusive (writer) lock

                void lock_shared(); //< acquire shareable (reader) lock
                void unlock_shared(); //< release shareable (reader) lock

                struct Impl;
            protected:
                Impl *pImpl;

            private:
                FileLock(const FileLock &); // disabled
                FileLock &operator=(const FileLock &); // disabled
            };

        }//fs
    }//utils
}//kkrtc

#endif
#endif //KKRTC_FILESYSTEM_PRIVATE_H
