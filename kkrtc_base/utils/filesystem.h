//
// Created by devyk on 2023/9/18.
//

#ifndef KKRTC_FILESYSTEM_H
#define KKRTC_FILESYSTEM_H

#include <string>
#include <vector>

namespace kkrtc {
    namespace utils {
        namespace fs {

            bool exists(const std::string &path);

            bool isDirectory(const std::string &path);

            void remove_all(const std::string &path);


            std::string getcwd();

/** @brief Converts path p to a canonical absolute path
 * Symlinks are processed if there is support for them on running platform.
 *
 * @param path input path. Target file/directory should exist.
 */
            std::string canonical(const std::string &path);

/** Join path components */
            std::string join(const std::string &base, const std::string &path);

/** Get parent directory */
            std::string getParent(const std::string &path);

            std::wstring getParent(const std::wstring &path);

/**
 * Generate a list of all files that match the globbing pattern.
 *
 * Result entries are prefixed by base directory path.
 *
 * @param directory base directory
 * @param pattern filter pattern (based on '*'/'?' symbols). Use empty string to disable filtering and return all results
 * @param[out] result result of globing.
 * @param recursive scan nested directories too
 * @param includeDirectories include directories into results list
 */
            void glob(const std::string &directory, const std::string &pattern,
                      std::vector<std::string> &result,
                      bool recursive = false, bool includeDirectories = false);

/**
 * Generate a list of all files that match the globbing pattern.
 *
 * @param directory base directory
 * @param pattern filter pattern (based on '*'/'?' symbols). Use empty string to disable filtering and return all results
 * @param[out] result globbing result with relative paths from base directory
 * @param recursive scan nested directories too
 * @param includeDirectories include directories into results list
 */
            void glob_relative(const std::string &directory, const std::string &pattern,
                               std::vector<std::string> &result,
                               bool recursive = false, bool includeDirectories = false);


            bool createDirectory(const std::string &path);

            bool createDirectories(const std::string &path);

#if defined(__KKRTC_BUILD) || defined(BUILD_PLUGIN)
            // TODO
//CV_EXPORTS std::string getTempDirectory();

/**
 * @brief Returns directory to store OpenCV cache files
 * Create sub-directory in common OpenCV cache directory if it doesn't exist.
 * @param sub_directory_name name of sub-directory. NULL or "" value asks to return root cache directory.
 * @param configuration_name optional name of configuration parameter name which overrides default behavior.
 * @return Path to cache directory. Returns empty string if cache directories support is not available. Returns "disabled" if cache disabled by user.
 */

 std::string getCacheDirectory(const char* sub_directory_name, const char* configuration_name = NULL);

#endif

/** @brief Retrieve location of OpenCV libraries or current executable
*/
            bool getBinLocation(std::string &dst);

#if defined(_WIN32)

/** @brief Retrieve location of OpenCV libraries or current executable

@note WIN32 only
 */
            bool getBinLocation(std::wstring &dst);

#endif
        }//fs
    }//utils
}//kkrtc



#endif //KKRTC_FILESYSTEM_H
