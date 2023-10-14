//
// Created by devyk on 2023/9/19.
//

#include "filesystem_glob.h"

#if KKRTC_HAVE_FILESYSTEM_SUPPORT
#if defined _WIN32 || defined WINCE
# include <windows.h>



namespace
{
#ifdef WINRT
    DIR::DIR() { }
    ~DIR::DIR()
    {
        if (ent.d_name)
            delete[] ent.d_name;
    }
#endif


    DIR* opendir(const char* path)
    {
        DIR* dir = new DIR;
        dir->ent.d_name = 0;
#if defined(WINRT) || defined(_WIN32_WCE)
        std::string full_path = std::string(path) + "\\*";
        wchar_t wfull_path[MAX_PATH];
        size_t copied = mbstowcs(wfull_path, full_path.c_str(), MAX_PATH);
        CV_Assert((copied != MAX_PATH) && (copied != (size_t)-1));
        dir->handle = ::FindFirstFileExW(wfull_path, FindExInfoStandard,
                        &dir->data, FindExSearchNameMatch, NULL, 0);
#else
        dir->handle = ::FindFirstFileExA((std::string(path) + "\\*").c_str(),
                                         FindExInfoStandard, &dir->data, FindExSearchNameMatch, NULL, 0);
#endif
        if(dir->handle == INVALID_HANDLE_VALUE)
        {
            /*closedir will do all cleanup*/
            delete dir;
            return 0;
        }
        return dir;
    }

    dirent* readdir(DIR* dir)
    {
#if defined(WINRT) || defined(_WIN32_WCE)
        if (dir->ent.d_name != 0)
        {
            if (::FindNextFileW(dir->handle, &dir->data) != TRUE)
                return 0;
        }
        size_t asize = wcstombs(NULL, dir->data.cFileName, 0);
        CV_Assert((asize != 0) && (asize != (size_t)-1));
        char* aname = new char[asize+1];
        aname[asize] = 0;
        wcstombs(aname, dir->data.cFileName, asize);
        dir->ent.d_name = aname;
#else
        if (dir->ent.d_name != 0)
        {
            if (::FindNextFileA(dir->handle, &dir->data) != TRUE)
                return 0;
        }
        dir->ent.d_name = dir->data.cFileName;
#endif
        return &dir->ent;
    }

    void closedir(DIR* dir)
    {
        ::FindClose(dir->handle);
        delete dir;
    }


}
#else // defined _WIN32 || defined WINCE
# include <dirent.h>
# include <sys/stat.h>
const char dir_separators[] = "/";
#endif // defined _WIN32 || defined WINCE
#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT


#if KKRTC_HAVE_FILESYSTEM_SUPPORT
bool isDirectory(const std::string &path) {
#if KKRTC_HAVE_FILESYSTEM_SUPPORT
    return isDir(path, NULL);
#else
    KKlogInfo << "File system support is disabled in this OpenCV build!";
#endif
}
static bool isDir(const std::string& path, DIR* dir)
{
#if defined _WIN32 || defined _WIN32_WCE
    DWORD attributes;
    BOOL status = TRUE;
    if (dir)
        attributes = dir->data.dwFileAttributes;
    else
    {
        WIN32_FILE_ATTRIBUTE_DATA all_attrs;
#if defined WINRT || defined _WIN32_WCE
        wchar_t wpath[MAX_PATH];
        size_t copied = mbstowcs(wpath, path.c_str(), MAX_PATH);
        CV_Assert((copied != MAX_PATH) && (copied != (size_t)-1));
        status = ::GetFileAttributesExW(wpath, GetFileExInfoStandard, &all_attrs);
#else
        status = ::GetFileAttributesExA(path.c_str(), GetFileExInfoStandard, &all_attrs);
#endif
        attributes = all_attrs.dwFileAttributes;
    }

    return status && ((attributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
#else
    CV_UNUSED(dir);
    struct stat stat_buf;
    if (0 != stat( path.c_str(), &stat_buf))
        return false;
    int is_dir = S_ISDIR( stat_buf.st_mode);
    return is_dir != 0;
#endif
}
#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT

bool kkrtc::utils::fs::isDirectory(const std::string& path)
{
#if KKRTC_HAVE_FILESYSTEM_SUPPORT
    return isDir(path, NULL);
#else
    KKLogErrorTag("filesystem_glob") << "File system support is disabled in this KKRTC build!";
#endif
}

#if KKRTC_HAVE_FILESYSTEM_SUPPORT
static bool wildcmp(const char *string, const char *wild)
{
    // Based on wildcmp written by Jack Handy - <A href="mailto:jakkhandy@hotmail.com">jakkhandy@hotmail.com</A>
    const char *cp = 0, *mp = 0;

    while ((*string) && (*wild != '*'))
    {
        if ((*wild != *string) && (*wild != '?'))
        {
            return false;
        }

        wild++;
        string++;
    }

    while (*string)
    {
        if (*wild == '*')
        {
            if (!*++wild)
            {
                return true;
            }

            mp = wild;
            cp = string + 1;
        }
        else if ((*wild == *string) || (*wild == '?'))
        {
            wild++;
            string++;
        }
        else
        {
            wild = mp;
            string = cp++;
        }
    }

    while (*wild == '*')
    {
        wild++;
    }

    return *wild == 0;
}

static void glob_rec(const std::string& directory, const std::string& wildchart, std::vector<std::string>& result,
        bool recursive, bool includeDirectories, const std::string& pathPrefix)
{
    DIR *dir;

    if ((dir = opendir (directory.c_str())) != 0)
    {
        /* find all the files and directories within directory */
        try
        {
            struct dirent *ent;
            while ((ent = readdir (dir)) != 0)
            {
                const char* name = ent->d_name;
                if((name[0] == 0) || (name[0] == '.' && name[1] == 0) || (name[0] == '.' && name[1] == '.' && name[2] == 0))
                    continue;

                std::string path = kkrtc::utils::fs::join(directory, name);
                std::string entry = kkrtc::utils::fs::join(pathPrefix, name);

                if (isDir(path, dir))
                {
                    if (recursive)
                        glob_rec(path, wildchart, result, recursive, includeDirectories, entry);
                    if (!includeDirectories)
                        continue;
                }

                if (wildchart.empty() || wildcmp(name, wildchart.c_str()))
                    result.push_back(entry);
            }
        }
        catch (...)
        {
            closedir(dir);
            throw;
        }
        closedir(dir);
    }
    else
    {
        KKLogErrorTag("filesystem_glob") << "could not open directory:"<<directory.c_str();
    }
}
#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT

void kkrtc::utils::fs::glob(const std::string& directory, const std::string& pattern,
                         std::vector<std::string>& result,
                         bool recursive, bool includeDirectories)
{
#if KKRTC_HAVE_FILESYSTEM_SUPPORT
    glob_rec(directory, pattern, result, recursive, includeDirectories, directory);
    std::sort(result.begin(), result.end());
#else // KKRTC_HAVE_FILESYSTEM_SUPPORT
     KKLogErrorTag("filesystem_glob") <<"File system support is disabled in this KKRTC build!";
#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT
}

void kkrtc::utils::fs::glob_relative(const std::string& directory, const std::string& pattern,
                                  std::vector<std::string>& result,
                                  bool recursive, bool includeDirectories)
{
#if KKRTC_HAVE_FILESYSTEM_SUPPORT
    glob_rec(directory, pattern, result, recursive, includeDirectories, std::string());
    std::sort(result.begin(), result.end());
#else // KKRTC_HAVE_FILESYSTEM_SUPPORT
    KKLogErrorTag("filesystem_glob") <<"File system support is disabled in this KKRTC build!";
#endif // KKRTC_HAVE_FILESYSTEM_SUPPORT
}
