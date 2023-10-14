//
// Created by devyk on 2023/9/18.
//

#include "filesystem.h"
#include "log_helper.h"
#include "filesystem_glob.h"
#include "kkrtc_std.h"


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
            static const char native_separator = '\\';
#else
            static const char native_separator = '/';
#endif


            static inline
            bool isPathSeparator(char c) {
                return c == '/' || c == '\\';
            }

            bool exists(const std::string &path) {

#if defined _WIN32 || defined WINCE
                BOOL status = TRUE;
                {
                    WIN32_FILE_ATTRIBUTE_DATA all_attrs;
#ifdef WINRT
                    wchar_t wpath[MAX_PATH];
        size_t copied = mbstowcs(wpath, path.c_str(), MAX_PATH);
        CV_Assert((copied != MAX_PATH) && (copied != (size_t)-1));
        status = ::GetFileAttributesExW(wpath, GetFileExInfoStandard, &all_attrs);
#else
                    status = ::GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &all_attrs);
#endif
                }

                return !!status;
#else
                struct stat stat_buf;
    return (0 == stat(path.c_str(), &stat_buf));
#endif
            }


            void remove_all(const std::string &path) {
                if (!exists(path))
                    return;
                if (isDirectory(path))
                {
                    std::vector<std::string> entries;
                    utils::fs::glob(path, std::string(), entries, false, true);
                    for (size_t i = 0; i < entries.size(); i++)
                    {
                        const std::string& e = entries[i];
                        remove_all(e);
                    }
#ifdef _MSC_VER
                    bool result = _rmdir(path.c_str()) == 0;
#else
                    bool result = rmdir(path.c_str()) == 0;
#endif
                    if (!result)
                    {
                        KKLogErrorTag("filesystem") << "Can't remove directory: " << path;
                    }
                }
                else
                {
#ifdef _MSC_VER
                    bool result = _unlink(path.c_str()) == 0;
#else
                    bool result = unlink(path.c_str()) == 0;
#endif
                    if (!result)
                    {
                        KKLogErrorTag("filesystem") << "Can't remove file: " << path;
                    }
                }
            }

            std::string getcwd() {
                char buf[4096] ;
#if defined WIN32 || defined _WIN32 || defined WINCE
#ifdef WINRT
                return std::string();
#else
                DWORD sz = GetCurrentDirectoryA(0, NULL);

                sz = GetCurrentDirectoryA((DWORD)4096, buf);
                return std::string(buf, (size_t)sz);
#endif
#elif defined __linux__ || defined __APPLE__ || defined __HAIKU__ || defined __FreeBSD__
    for(;;)
    {
        char* p = ::getcwd(buf, 4096);
        if (p == NULL)
        {
            if (errno == ERANGE)
            {

                continue;
            }
            return std::string();
        }
        break;
    }
    return std::string(buf, (size_t)strlen(buf));
#else
    return std::string();
#endif
            }



            std::string canonical(const std::string &path) {
                std::string result;
#ifdef _WIN32
                const char* result_str = _fullpath(NULL, path.c_str(), 0);
#else
                const char* result_str = realpath(path.c_str(), NULL);
#endif
                if (result_str)
                {
                    result =  std::string(result_str);
                    free((void*)result_str);
                }
                return result.empty() ? path : result;
            }

            std::string join(const std::string &base, const std::string &path) {
                if (base.empty())
                    return path;
                if (path.empty())
                    return base;

                bool baseSep = isPathSeparator(base[base.size() - 1]);
                bool pathSep = isPathSeparator(path[0]);
                std::string result;
                if (baseSep && pathSep)
                {
                    result = base + path.substr(1);
                }
                else if (!baseSep && !pathSep)
                {
                    result = base + native_separator + path;
                }
                else
                {
                    result = base + path;
                }
                return result;
            }

            std::string getParent(const std::string &path) {
                std::string::size_type loc = path.find_last_of("/\\");
                if (loc == std::string::npos)
                    return std::string();
                return std::string(path, 0, loc);
            }

            std::wstring getParent(const std::wstring &path) {
                std::wstring::size_type loc = path.find_last_of(L"/\\");
                if (loc == std::wstring::npos)
                    return std::wstring();
                return std::wstring(path, 0, loc);
            }




            bool createDirectory(const std::string & path)
            {
#if defined WIN32 || defined _WIN32 || defined WINCE
#ifdef WINRT
                wchar_t wpath[MAX_PATH];
    size_t copied = mbstowcs(wpath, path.c_str(), MAX_PATH);
    CV_Assert((copied != MAX_PATH) && (copied != (size_t)-1));
    int result = CreateDirectoryA(wpath, NULL) ? 0 : -1;
#else
                int result = _mkdir(path.c_str());
#endif
#elif defined __linux__ || defined __APPLE__ || defined __HAIKU__ || defined __FreeBSD__
                int result = mkdir(path.c_str(), 0777);
#else
    int result = -1;
#endif

                if (result == -1)
                {
                    return isDirectory(path);
                }
                return true;
            }

            bool createDirectories(const std::string & path_)
            {
                std::string path = path_;
                for (;;)
                {
                    char last_char = path.empty() ? 0 : path[path.length() - 1];
                    if (isPathSeparator(last_char))
                    {
                        path = path.substr(0, path.length() - 1);
                        continue;
                    }
                    break;
                }

                if (path.empty() || path == "./" || path == ".\\" || path == ".")
                    return true;
                if (isDirectory(path))
                    return true;

                size_t pos = path.rfind('/');
                if (pos == std::string::npos)
                    pos = path.rfind('\\');
                if (pos != std::string::npos)
                {
                    std::string parent_directory = path.substr(0, pos);
                    if (!parent_directory.empty())
                    {
                        if (!createDirectories(parent_directory))
                            return false;
                    }
                }

                return createDirectory(path);
            }


            static String getModuleLocation(const void* addr)
            {

#ifdef _WIN32
                HMODULE m = 0;
#if _WIN32_WINNT >= 0x0501 && (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
                ::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                    reinterpret_cast<LPCTSTR>(addr),
                                    &m);
#endif
                if (m)
                {
                    TCHAR path[MAX_PATH];
                    const size_t path_size = sizeof(path) / sizeof(*path);
                    size_t sz = GetModuleFileName(m, path, path_size);
                    if (sz > 0 && sz < path_size)
                    {
                        path[sz] = TCHAR('\0');
#ifdef _UNICODE
                        char char_path[MAX_PATH];
            size_t copied = wcstombs(char_path, path, MAX_PATH);
            CV_Assert((copied != MAX_PATH) && (copied != (size_t)-1));
            return cv::String(char_path);
#else
                        return String(path);
#endif
                    }
                }
#elif defined(__linux__)
                Dl_info info;
    if (0 != dladdr(addr, &info))
    {
        return cv::String(info.dli_fname);
    }
#elif defined(__APPLE__)
# if TARGET_OS_MAC
    Dl_info info;
    if (0 != dladdr(addr, &info))
    {
        return cv::String(info.dli_fname);
    }
# endif
#else
    // not supported, skip
#endif
                return String();
            }

            bool getBinLocation(std::string& dst)
            {
                dst = getModuleLocation((void*)getModuleLocation); // using code address, doesn't work with static linkage!
                return !dst.empty();
            }

#ifdef _WIN32
            bool getBinLocation(std::wstring& dst)
            {
                void* addr = (void*)getModuleLocation; // using code address, doesn't work with static linkage!
                HMODULE m = 0;
#if _WIN32_WINNT >= 0x0501 && (!defined(WINAPI_FAMILY) || (WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP))
                ::GetModuleHandleEx(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                                    reinterpret_cast<LPCTSTR>(addr),
                                    &m);
#endif
                if (m)
                {
                    wchar_t path[4096];
                    const size_t path_size = sizeof(path)/sizeof(*path);
                    size_t sz = GetModuleFileNameW(m, path, path_size);
                    if (sz > 0 && sz < path_size)
                    {
                        path[sz] = '\0';
                        dst.assign(path, sz);
                        return true;
                    }
                }
                return false;
            }
#endif


        }
    }
}