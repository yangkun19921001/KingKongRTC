//
// Created by devyk on 2023/9/17.
//

#ifndef KKRTC_KKRTC_PLUGIN_LOADER_H
#define KKRTC_KKRTC_PLUGIN_LOADER_H

#include "filesystem.h"
#include "filesystem_private.h"

#if defined(_WIN32)

#include <windows.h>

#elif defined(__linux__) || defined(__APPLE__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__HAIKU__) || defined(__GLIBC__)
#include <dlfcn.h>
#endif

#include "log_helper.h"
#include "version.h"

namespace kkrtc {
    namespace plugin {


#if defined(_WIN32)
        typedef HMODULE LibHandle_t;
        typedef wchar_t FileSystemChar_t;
        typedef std::wstring FileSystemPath_t;

// TODO wchar_t <=> UTF-8
        static inline
        FileSystemPath_t toFileSystemPath(const std::string &p) {
            FileSystemPath_t result;
            result.resize(p.size());
            for (size_t i = 0; i < p.size(); i++)
                result[i] = (wchar_t) p[i];
            return result;
        }

// TODO wchar_t <=> UTF-8
        static inline
        std::string toPrintablePath(const FileSystemPath_t &p) {
            std::string result;
            result.resize(p.size());
            for (size_t i = 0; i < p.size(); i++) {
                wchar_t ch = p[i];
                if ((int) ch >= ' ' && (int) ch < 128)
                    result[i] = (char) ch;
                else
                    result[i] = '?';
            }
            return result;
        }

#else  // !_WIN32
        typedef void* LibHandle_t;
        typedef char FileSystemChar_t;
        typedef std::string FileSystemPath_t;

        static inline FileSystemPath_t toFileSystemPath(const std::string& p) { return p; }
        static inline std::string toPrintablePath(const FileSystemPath_t& p) { return p; }
#endif

        static inline
        void *getSymbol_(LibHandle_t h, const char *symbolName) {
#if defined(_WIN32)
            return (void *) GetProcAddress(h, symbolName);
#elif defined(__linux__) || defined(__APPLE__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__HAIKU__) || defined(__GLIBC__)
            return dlsym(h, symbolName);
#endif
        }

        static inline
        LibHandle_t libraryLoad_(const FileSystemPath_t &filename) {
#if defined(_WIN32)
# ifdef WINRT
            return LoadPackagedLibrary(filename.c_str(), 0);
# else
            return LoadLibraryW(filename.c_str());
#endif
#elif defined(__linux__) || defined(__APPLE__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__HAIKU__) || defined(__GLIBC__)
            void* handle = dlopen(filename.c_str(), RTLD_NOW);
    CV_LOG_IF_DEBUG(NULL, !handle, "dlopen() error: " << dlerror());
    return handle;
#endif
        }

        static inline
        void libraryRelease_(LibHandle_t h) {
#if defined(_WIN32)
            FreeLibrary(h);
#elif defined(__linux__) || defined(__APPLE__) || defined(__OpenBSD__) || defined(__FreeBSD__) || defined(__HAIKU__) || defined(__GLIBC__)
            dlclose(h);
#endif
        }

        static inline
        std::string libraryPrefix() {
#if defined(_WIN32)
            return "";
#else
            return "lib";
#endif
        }

        static inline
        std::string librarySuffix() {
#if defined(_WIN32)
            const char *suffix = ""
                                 /*                                 KKAUX_STR(KKRTC_MAJOR_VERSION)
                                                                  KKAUX_STR(KKRTC_MINOR_VERSION)
                                                                  KKAUX_STR(KKRTC_PATCH_VERSION)
                                                                  #if (defined _MSC_VER && defined _M_X64) || (defined __GNUC__ && defined __x86_64__)
                                                                  "_64"
                                                                  #endif
                                                                  #if defined(_DEBUG) && defined(DEBUG_POSTFIX)
                                                                  CVAUX_STR(DEBUG_POSTFIX)
                                                                  #endif*/
                                 ".dll";
            return suffix;
#else
            return ".so";
#endif
        }

        class DynamicLib {
        private:
            LibHandle_t handle;
            const FileSystemPath_t fname;
            bool disableAutoUnloading_;
        public:
            DynamicLib(const FileSystemPath_t &filename);

            ~DynamicLib();

            /** Do not automatically unload library in destructor */
            inline void disableAutomaticLibraryUnloading() {
                disableAutoUnloading_ = true;
            }

            inline bool isLoaded() const {
                return handle != NULL;
            }

            void *getSymbol(const char *symbolName) const;

            std::string getName() const;

        private:
            void libraryLoad(const FileSystemPath_t &filename);

            void libraryRelease();

        private:
            DynamicLib(const DynamicLib &) = delete;

            DynamicLib &operator=(const DynamicLib &) = delete;
        };
        std::vector<plugin::FileSystemPath_t> getPluginCandidates(const std::string &baseName);
    }//plugins
}//kkrtc



#endif //KKRTC_KKRTC_PLUGIN_LOADER_H
