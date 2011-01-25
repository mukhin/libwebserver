// io.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_IO_H__
#define BASE_IO_H__

#include <string>
#include <vector>


namespace base {

  bool CheckFileExists(const char* path);
  void CreateDirectoryIfNotExists(const char* path);
  void DeleteFileOrDirectory(const std::vector<std::string>& path);

} // namespace base

#endif // BASE_IO_H__
