// io.cpp
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#include "io.h"
#include <base/exception.h>
#include <base/c_format.h>
#include <cerrno>
#include <iostream>

extern "C"  {
#include <fts.h>
#include <string.h>
#include <sys/param.h>
#include <sys/stat.h>
}


namespace base {

  bool CheckFileExists(const char* path) {
    struct stat dirstat;
    const bool success = ::stat(path, &dirstat) == 0;

    if (success) {
      return true;
    }
    else if (errno == ENOENT) {
      return false;
    }
    else {
      base_throw(IOException, ::strerror(errno));
    }
  }


  void CreateDirectoryIfNotExists(const char* path) {
    struct stat dirstat;
    if (::stat(path, &dirstat) == -1) {
      if (errno == ENOENT) {
        if (::mkdir(path, 0755) == -1) {
          base_throw(IOException, ::strerror(errno));
        }
      }
      else {
        base_throw(IOException, ::strerror(errno));
      }
    }
    else if ((dirstat.st_mode & S_IFDIR) == 0) {
      base_throw(IOException, c_format("File with name '%s' prevents directory creation", path));
    }
  }


  void DeleteFileOrDirectory(const std::vector<std::string>& files) {
    if (files.empty()) {
      return;
    }

    FTS* fts;
    FTSENT* p;
    int flags = FTS_PHYSICAL | FTS_NOSTAT;

    size_t n = 0;
    char** const entries = new char*[files.size() + 1];
    entries[files.size()] = 0;

    for (std::vector<std::string>::const_iterator i = files.begin(); i != files.end(); ++i) {
      char* path = new char[i->length() + 1];
      ::strncpy(path, i->c_str(), i->length() + 1);
      entries[n++] = path;
    }

    fts = ::fts_open(entries, flags, 0);
    if (!fts) {
      base_throw(IOException, c_format("fts_open failed: %s", ::strerror(errno)));
    }

    while ((p = ::fts_read(fts)) != 0) {
      if (p->fts_info == FTS_ERR) {
        base_throw(IOException, c_format("%s: %s", p->fts_path, ::strerror(p->fts_errno)));
      }

      if (p->fts_info == FTS_D) {
        continue;
      }
      else if (p->fts_info == FTS_DP || p->fts_info == FTS_DNR) {
        if (::rmdir(p->fts_accpath) == -1) {
          if (errno != ENOENT) {
            base_throw(IOException, c_format("%s: %s", p->fts_path, ::strerror(errno)));
          }
          errno = 0;
        }
      }
      else {
        if (::unlink(p->fts_accpath) == -1) {
          if (errno != ENOENT) {
            base_throw(IOException, c_format("%s: %s", p->fts_path, ::strerror(errno)));
          }
          errno = 0;
        }
      }
    }

    for (size_t i = 0; i != files.size(); ++i) {
      delete[] entries[i];
    }

    delete[] entries;
  }

} // namespace base
