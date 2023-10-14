//
// Created by devyk on 2023/9/19.
//

#ifndef KKRTC_FILESYSTEM_GLOB_H
#define KKRTC_FILESYSTEM_GLOB_H

#include "filesystem.h"
#include "filesystem_private.h"
#include "log_helper.h"

// 宏定义
#if defined(_WIN32) || defined(WINCE)
const char dir_separators[] = "/\\";

namespace {
    struct dirent {
        const char* d_name;
    };

    struct DIR {
#if defined(WINRT) || defined(_WIN32_WCE)
        WIN32_FIND_DATAW data;
#else
        WIN32_FIND_DATAA data;
#endif
        HANDLE handle;
        dirent ent;
#ifdef WINRT
        DIR();
        ~DIR();
#endif
    };

    DIR* opendir(const char* path);
    dirent* readdir(DIR* dir);
    void closedir(DIR* dir);
}
#else
# include <dirent.h>
# include <sys/stat.h>
const char dir_separators[] = "/";
#endif
#if KKRTC_HAVE_FILESYSTEM_SUPPORT
static bool isDir(const std::string& path, DIR* dir);
bool isDirectory(const std::string &path);
#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT

#endif //KKRTC_FILESYSTEM_GLOB_H
