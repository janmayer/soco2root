#include "FSUtils.h"

#include <cassert>
#include <cstring>

#include <algorithm>
#include <iostream>
#include <memory>

#include <fcntl.h>
#include <pwd.h>
#include <sys/mman.h>
#include <sys/vfs.h>
#include <unistd.h>

#ifdef __linux__
#include <linux/limits.h>
#include <linux/magic.h>
#else
#define SMB_SUPER_MAGIC 0x517B
#define NFS_SUPER_MAGIC 0x6969
#define PATH_MAX 4096
#endif


namespace SOCO
{

class StatError : public std::runtime_error
{
    public:
    StatError() noexcept
        : std::runtime_error("stat failed")
    {
    }

    StatError(const StatError& e) noexcept
        : std::runtime_error(e)
    {
    }

    StatError(StatError&& e) noexcept
        : std::runtime_error(std::move(e))
    {
    }

    StatError(const std::string& function, const std::string& filename, const int& err_no) noexcept
        : std::runtime_error(function + " - stat failed for file \'" + filename + "\': " + strerror(err_no))
    {
    }

    StatError(const std::string& filename, const std::string& desc)
        : std::runtime_error("Failed to stat file " + filename + " (" + desc + ")")
    {
    }

    virtual ~StatError() noexcept {}
};

class MMAPError : public std::runtime_error
{
    public:
    MMAPError() noexcept
        : std::runtime_error("mmap failed")
    {
    }

    MMAPError(const MMAPError& e) noexcept
        : std::runtime_error(e)
    {
    }

    MMAPError(MMAPError&& e) noexcept
        : std::runtime_error(std::move(e))
    {
    }

    explicit MMAPError(const std::string& function, const std::string& filename, const int& err_no) noexcept
        : std::runtime_error(function + " - mmap failed for file \'" + filename + "\': " + strerror(err_no))
    {
    }

    virtual ~MMAPError() noexcept {}
};

class NotARegularFile : public std::runtime_error
{
    public:
    NotARegularFile() noexcept
        : std::runtime_error("not a regular file")
    {
    }

    NotARegularFile(const NotARegularFile& e) noexcept
        : std::runtime_error(e)
    {
    }

    NotARegularFile(NotARegularFile&& e) noexcept
        : std::runtime_error(std::move(e))
    {
    }

    explicit NotARegularFile(const std::string& function, const std::string& filename) noexcept
        : std::runtime_error(function + " - \'" + filename + "\' is not a regular file")
    {
    }

    virtual ~NotARegularFile() noexcept {}
};


std::string FSUtils::collapseDuplicateSlashes(std::string str)
{
    size_t pos;
    while ((pos = str.find("//")) != std::string::npos)
    {
        str.replace(pos, 2, 1, '/');
    }
    return str;
}

void FSUtils::collapseDuplicateSlashesInplace(std::string& str)
{
    size_t pos;
    while ((pos = str.find("//")) != std::string::npos)
    {
        str.replace(pos, 2, 1, '/');
    }
}

// Implementation of the POSIX dirname() function for std::string
std::string FSUtils::dirname(std::string path)
{
    cleanupPath(path);

    if (path.empty())
    {
        return std::string(".");
    }
    else if (path == "/")
    {
        return path;
    }

    auto last_slash = path.find_last_of('/');
    if (last_slash == std::string::npos)
    {
        path = ".";
    }
    else if (!last_slash)
    {
        path.erase(1, std::string::npos);
    }
    else
    {
        path.erase(last_slash);
    }
    return path;
}

// Implementation of the POSIX basename() function for std::string
std::string FSUtils::basename(std::string path)
{
    cleanupPath(path);

    if (path.empty())
    {
        return std::string(".");
    }
    if (path == "/")
    {
        return path;
    }

    auto last_slash = path.find_last_of('/');
    if (last_slash != std::string::npos)
    {
        path.erase(0, last_slash + 1);
    }
    return path;
}

std::string FSUtils::buildFilename(const std::string& base, const std::string& path, const std::string& extension)
{
    assert(base.size());
    std::string result = (path.size()) ? path : FSUtils::dirname(path);
    if (result.back() != '/')
    {
        result.push_back('/');
    }

    std::string base_name = FSUtils::basename(base);
    auto last_dot         = base_name.find_last_of('.');
    if (!last_dot || last_dot == base_name.npos || isNumericPostfix(base_name))
    {
        result += base_name;
    }
    else
    {
        result.append(base_name.begin(), base_name.begin() + last_dot);
    }

    if (!extension.empty())
    {
        if (extension.front() != '.' && result.back() != '.')
        {
            result.push_back('.');
        }
        result += extension;
    }
    return result;
}

std::string FSUtils::buildFilename(const std::string& base,
                                   const std::string& path,
                                   const std::string& extension,
                                   const std::string& infix)
{
    std::string name = FSUtils::buildFilename(base, path, extension);
    addInfix(name, infix);
    return name;
}

std::string FSUtils::buildDirname(std::string base, const std::string& sub)
{
    if (base.empty())
    {
        base = "./";
    }
    else if (base.back() != '/')
    {
        base.push_back('/');
    }
    base += sub;
    return base;
}

std::string FSUtils::stripExtension(const std::string& path)
{
    std::string base = FSUtils::basename(path);
    auto last_dot    = base.find_last_of('.');
    if (last_dot && last_dot != std::string::npos && !isNumericPostfix(base))
    {
        base.erase(last_dot);
    }
    return (FSUtils::dirname(path) + "/" + base);
}

std::string& FSUtils::addInfix(std::string& path, const std::string& infix)
{
    assert(!path.empty());
    assert(!infix.empty());

    std::string base = FSUtils::basename(path);
    size_t last_dot  = base.find_last_of('.');
    if (!last_dot || last_dot == std::string::npos || isNumericPostfix(base))
    {
        base += infix;
    }
    else
    {
        base.insert(last_dot, infix);
    }

    return (path = FSUtils::dirname(path) + "/" + base);
}

bool FSUtils::isNumericPostfix(const std::string& path)
{
    std::string base = FSUtils::basename(path);
    auto last_dot    = base.find_last_of('.');
    if (!last_dot || last_dot == std::string::npos || (last_dot + 1) == base.size())
    {
        return false;
    }
    ++last_dot;

    return (std::find_if(base.begin() + last_dot, base.end(), [](const int c) -> bool {
                return (!std::isdigit(c));
            }) == base.end());
}

void FSUtils::createDirectory(const std::string& path, const mode_t mode)
{
    if (directoryExists(path))
    {
        return;
    }

    // try to create the output dir
    errno = 0;
    if (mkdir(path.c_str(), mode) == -1)
    {
        const int errnum = errno;
        throw std::runtime_error("Failed to create directory " + path + " (" +
                                 FSUtils::getErrorDescription(errnum) + ")");
    }
}

// check if output dir exists
bool FSUtils::directoryExists(const std::string& path)
{
    struct stat sb;
    errno  = 0;
    int rc = ::stat(path.c_str(), &sb);
    if (rc == -1)
    {
        if (errno != ENOENT)
        {
            throw StatError(path, getErrorDescription(errno));
        }
        return false;
    }
    else if (!S_ISDIR(sb.st_mode))
    {
        throw std::runtime_error(path + " exists and is not a directory!");
    }
    return true;
}

bool FSUtils::fileExists(const std::string& path)
{
    struct stat sb;
    errno = 0;
    if (::stat(path.c_str(), &sb) == -1)
    {
        if (errno != ENOENT)
        {
            throw StatError(path, getErrorDescription(errno));
        }
        return false;
    }
    return (!S_ISDIR(sb.st_mode));
}

bool FSUtils::isRegularFile(const std::string& path)
{
    struct stat sb;
    errno = 0;
    if (::stat(path.c_str(), &sb) == -1)
    {
        if (errno != ENOENT)
        {
            throw StatError(path, getErrorDescription(errno));
        }
        return false;
    }
    return S_ISREG(sb.st_mode);
}

std::string& FSUtils::cleanupPath(std::string& path)
{
    collapseDuplicateSlashesInplace(path);
    if (path.size() > 1 && path.back() == '/')
    {
        path.pop_back();
    }
    return path;
}

std::string FSUtils::getErrorDescription(const int errnum)
{
    char buf[1024];
    errno = 0;
#if (_POSIX_C_SOURCE >= 200112L || _XOPEN_SOURCE >= 600) && !_GNU_SOURCE
    // XGI-compliant strerror
    int rc = strerror_r(errnum, buf, 1024);
    if (rc == -1)
    {
        // glib >= 2.13
        rc = errno;
    }

    if (rc == EINVAL)
    {
        return ("Unknown error " + std::to_string(errnum));
    }
    else if (rc == ERANGE)
    {
        return ("Error " + std::to_string(errnum));
    }
    else
    {
        return std::string(buf);
    }
#else
    /* GNU-specific */
    const char* desc = strerror_r(errnum, buf, 1024);
    if (!errno)
    {
        return std::string(desc);
    }

    // try again with bigger buffer
    char buf2[2048];
    errno = 0;
    desc  = strerror_r(errnum, buf2, 2048);
    if (!errno)
    {
        return std::string(desc);
    }
    return ("Error " + std::to_string(errnum));
#endif
}

void FSUtils::stat(const std::string& name, struct stat* sb)
{
    assert(!name.empty());
    assert(sb != nullptr);
    errno = 0;
    if (::stat(name.c_str(), sb) == -1)
    {
        int errnum = errno;
        throw StatError(name, getErrorDescription(errnum));
    }
}

void FSUtils::stat(const std::string& name, int fd, struct stat* sb)
{
    assert(!name.empty());
    assert(fd != -1);
    assert(sb != nullptr);
    errno = 0;
    if (fstat(fd, sb) == -1)
    {
        int errnum = errno;
        throw StatError(name, getErrorDescription(errnum));
    }
}

void* FSUtils::mmap(const std::string& name, struct stat* sb, bool exclude_from_coredump)
{
    assert(!name.empty());
    assert(sb != nullptr);

    int errnum;
    errno  = 0;
    int fd = open(name.c_str(), O_RDONLY);
    if (fd == -1)
    {
        errnum = errno;
        throw std::runtime_error("Failed to open " + name + " (" + getErrorDescription(errnum) +
                                 ")");
    }

    errno = 0;
    FSUtils::stat(name, fd, sb);
    if (!S_ISREG(sb->st_mode))
    {
        throw NotARegularFile("FSUtils::mmap()", name);
    }

    errno      = 0;
    void* data = ::mmap(nullptr, sb->st_size, PROT_READ, MAP_PRIVATE, fd, 0);
    errnum     = errno;
    do
    {
        errno = 0;
    } while (close(fd) == -1 && errno == EINTR);
    if (errno && errno != EINTR)
    {
        throw std::runtime_error("Failed to close file descriptor to " + name + " (" +
                                 getErrorDescription(errno) + ")");
    }

    if (data == MAP_FAILED)
    {
        throw MMAPError("FSUtils::mmapFile()", name, errnum);
    }

#if defined(MADV_DONTDUMP)
    errno = 0;
    if (exclude_from_coredump && madvise(data, sb->st_size, MADV_DONTDUMP) == -1)
    {
        errnum = errno;
        std::cout << "[W] Failed to exclude mmap-ed data from " << name << " from core-dumps ("
                  << getErrorDescription(errnum) << ")\n";
    }
#else
    // avoid unused parameter warning
    (void)exclude_from_coredump;
#endif /* defined(MADV_DONTDUMP) */

    return data;
}

std::string FSUtils::getHomeDirectory()
{
    char* home = getenv("HOME");
    if (home)
    {
        return std::string(home);
    }

    size_t size = sysconf(_SC_GETPW_R_SIZE_MAX);
    if (size == static_cast<size_t>(-1))
    {
        size = 16384;
    }

    auto buf = std::unique_ptr<char[]>(new char[size]);
    struct passwd pwd;
    struct passwd* result = nullptr;
    uid_t uid             = getuid();
    int rc                = getpwuid_r(uid, &pwd, buf.get(), size, &result);
    if (!result)
    {
        if (!rc)
        {
            throw std::runtime_error("No passwd entry for UID " + std::to_string(uid));
        }
        throw std::runtime_error("Failed to get passwd entry for UID " + std::to_string(uid) +
                                 " (" + getErrorDescription(rc) + ")");
    }
    return std::string(pwd.pw_dir);
}

std::string FSUtils::getcwd()
{
    char path[PATH_MAX];
    errno = 0;
    if (!::getcwd(path, PATH_MAX))
    {
        if (errno == ERANGE)
        {
            char path2[PATH_MAX * 2];
            errno = 0;
            if (!::getcwd(path2, PATH_MAX * 2))
            {
                throw std::runtime_error(
                    "Failed to get current working directory "
                    "(" +
                    getErrorDescription(errno) + ")");
            }
            return std::string(path2);
        }
        throw std::runtime_error("Failed to get current working directory (" +
                                 getErrorDescription(errno) + ")");
    }
    return std::string(path);
}

bool FSUtils::isRemoteOrSharedFS(const std::string& path)
{
    if (path.empty())
    {
        throw std::runtime_error("Invalid path");
    }

    struct statfs sb;
    errno  = 0;
    int rc = statfs(path.c_str(), &sb);
    if (rc == -1)
    {
        rc = errno;
        throw std::runtime_error("Can't statfs " + path + ": " + ::strerror(rc));
    }

    return (sb.f_type == SMB_SUPER_MAGIC || sb.f_type == NFS_SUPER_MAGIC || sb.f_type == 0x47504653 /* GPFS */);
}

} // namespace SOCO
