//
// Created by devyk on 2023/9/18.
//


#include "filesystem_private.h"
#include "log_helper.h"

#if KKRTC_HAVE_FILESYSTEM_SUPPORT

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#undef NOMINMAX
#define NOMINMAX

#include <windows.h>
#include <direct.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <io.h>
#include <stdio.h>

#elif defined __linux__ || defined __APPLE__ || defined __HAIKU__ || defined __FreeBSD__
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#endif

#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT

namespace kkrtc {
    namespace utils {
        namespace fs {
#ifdef _WIN32

            struct FileLock::Impl {
                Impl(const char *fname) {
                    // http://support.microsoft.com/kb/316609
                    int numRetries = 5;
                    do {
                        handle = ::CreateFileA(fname, GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
                                               OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
                        if (INVALID_HANDLE_VALUE == handle) {
                            if (ERROR_SHARING_VIOLATION == GetLastError()) {
                                numRetries--;
                                Sleep(250);
                                continue;
                            } else {
                                KKLogErrorTag("FileLock") << "Can't open lock file: " << fname;
                            }
                        }
                        break;
                    } while (numRetries > 0);
                }

                ~Impl() {
                    if (INVALID_HANDLE_VALUE != handle) {
                        ::CloseHandle(handle);
                    }
                }

                bool lock() {
                    OVERLAPPED overlapped;
                    std::memset(&overlapped, 0, sizeof(overlapped));
                    return !!::LockFileEx(handle, LOCKFILE_EXCLUSIVE_LOCK, 0, MAXDWORD, MAXDWORD, &overlapped);
                }

                bool unlock() {
                    OVERLAPPED overlapped;
                    std::memset(&overlapped, 0, sizeof(overlapped));
                    return !!::UnlockFileEx(handle, 0, MAXDWORD, MAXDWORD, &overlapped);
                }

                bool lock_shared() {
                    OVERLAPPED overlapped;
                    std::memset(&overlapped, 0, sizeof(overlapped));
                    return !!::LockFileEx(handle, 0, 0, MAXDWORD, MAXDWORD, &overlapped);
                }

                bool unlock_shared() {
                    return unlock();
                }

                HANDLE handle;

            private:
                Impl(const Impl &); // disabled
                Impl &operator=(const Impl &); // disabled
            };

#elif defined __linux__ || defined __APPLE__ || defined __HAIKU__ || defined __FreeBSD__

            struct FileLock::Impl
{
    Impl(const char* fname)
    {
        handle = ::open(fname, O_RDWR);
        CV_Assert(handle != -1);
    }
    ~Impl()
    {
        if (handle >= 0)
            ::close(handle);
    }

    bool lock()
    {
        struct ::flock l;
        std::memset(&l, 0, sizeof(l));
        l.l_type = F_WRLCK;
        l.l_whence = SEEK_SET;
        l.l_start = 0;
        l.l_len = 0;
        DBG(std::cout << "Lock..." << std::endl);
        bool res = -1 != ::fcntl(handle, F_SETLKW, &l);
        return res;
    }
    bool unlock()
    {
        struct ::flock l;
        std::memset(&l, 0, sizeof(l));
        l.l_type = F_UNLCK;
        l.l_whence = SEEK_SET;
        l.l_start = 0;
        l.l_len = 0;
        DBG(std::cout << "Unlock..." << std::endl);
        bool res = -1 != ::fcntl(handle, F_SETLK, &l);
        return res;
    }

    bool lock_shared()
    {
        struct ::flock l;
        std::memset(&l, 0, sizeof(l));
        l.l_type = F_RDLCK;
        l.l_whence = SEEK_SET;
        l.l_start = 0;
        l.l_len = 0;
        DBG(std::cout << "Lock read..." << std::endl);
        bool res = -1 != ::fcntl(handle, F_SETLKW, &l);
        return res;
    }
    bool unlock_shared()
    {
        return unlock();
    }

    int handle;

private:
    Impl(const Impl&); // disabled
    Impl& operator=(const Impl&); // disabled
};

#endif


            FileLock::FileLock(const char *fname)
                    : pImpl(new Impl(fname)) {
                // nothing
            }

            FileLock::~FileLock() {
                delete pImpl;
                pImpl = NULL;
            }

            void FileLock::lock() {}

            void FileLock::unlock() {}

            void FileLock::lock_shared() {}

            void FileLock::unlock_shared() {}
        }
    }
}