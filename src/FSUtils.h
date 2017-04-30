#ifndef SOCO_FS_UTILS_HH
#define SOCO_FS_UTILS_HH

/*
soco2root - Convert soco2 event files to root
https://gitlab.ikp.uni-koeln.de/jmayer/soco2root
Copyright (C) 2017  Jan Mayer

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

// This file is based on SOCOv2, https://gitlab.ikp.uni-koeln.de/nima/soco-v2

#include <string>

struct stat;

namespace SOCO
{

class FSUtils
{
    public:
    static std::string collapseDuplicateSlashes(std::string str);

    static void collapseDuplicateSlashesInplace(std::string& str);

    static std::string dirname(std::string path);

    static std::string basename(std::string path);

    static std::string
    buildFilename(const std::string& basename, const std::string& path, const std::string& extension);

    static std::string buildFilename(const std::string& basename,
                                     const std::string& sub_dir,
                                     const std::string& extension,
                                     const std::string& infix);

    static std::string buildDirname(std::string base, const std::string& sub);

    static std::string stripExtension(const std::string& path);

    static std::string& addInfix(std::string& str, const std::string& infix);

    static bool isNumericPostfix(const std::string& path);

    static bool directoryExists(const std::string& path);

    static void createDirectory(const std::string& path, const mode_t mode = 0755);

    static bool fileExists(const std::string& path);

    static bool isRegularFile(const std::string& path);

    static std::string& cleanupPath(std::string& path);

    static std::string getErrorDescription(const int errnum);

    static void stat(const std::string& name, struct stat* sb);

    static void stat(const std::string& name, int fd, struct stat* sb);

    static void* mmap(const std::string& name, struct stat* sb, bool exclude_from_coredump = true);

    static std::string getHomeDirectory();

    static std::string getcwd();

    static bool isRemoteOrSharedFS(const std::string& path);
};

} // namespace SOCO

#endif // SOCO_FS_UTILS_HH
