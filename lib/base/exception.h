// exception.h
// Base library
// 
// Copyright 2010 LibWebserver Authors. All rights reserved.

#ifndef BASE_EXCEPTION_H__
#define BASE_EXCEPTION_H__

#include <stdexcept>
#include <string>

/**
 * Macro to known insert values into exception arguments.
 */
#define base_throw(_class, args...) \
throw _class(__FILE__, __LINE__, args);

#define base_throw0(_class) \
throw _class(__FILE__, __LINE__);

class BaseException : public std::runtime_error {
public:
  /**
   * Constructor for a given type for exception, file name,
   * and file line number.
   *
   * @param init_what the type of exception.
   * @param file the file name where the exception was thrown.
   * @param line the line in @ref file where the exception was thrown.
   */
  BaseException(const char* init_what, const char* file, const size_t line);
  virtual ~BaseException() throw();

  /**
   * Get the type of this exception.
   *
   * @return the string with the type of this exception.
   */
  std::string message() const throw();

  /**
   * Get the location for throwing an exception.
   *
   * @return the string with the location (file name and file line number)
   * for throwing an exception.
   */
  const std::string where() const;

  /**
   * Get the reason for throwing an exception.
   *
   * @return the string with the reason for throwing an exception.
   */
  virtual const std::string why() const;

  /**
   * Convert this exception from binary form to presentation format.
   *
   * @return C++ string with the human-readable ASCII representation
   * of the exception.
   */
  const char* what() throw();

protected:
  BaseException(const BaseException& src);
  BaseException& operator=(const BaseException& src) ;

  std::string what_; // The type of exception
  std::string what_complex_; // The full info of exception
  const char*  file_; // The file name where exception occured
  size_t line_; // The line number where exception occured
};

/**
 * @short A base class for exceptions that keeps the reason for exception.
 */
class BaseReasonedException : public BaseException {
public:
  /**
   * Constructor for a given type for exception, file name,
   * file line number, and a reason.
   *
   * @param init_what the type of exception.
   * @param file the file name where the exception was thrown.
   * @param line the line in @ref file where the exception was thrown.
   * @param init_why the reason for the exception that was thrown.
   */
  BaseReasonedException(const char* init_what, const char* file, const size_t line, const std::string& init_why) throw();
  ~BaseReasonedException() throw();

  /**
   * Get the reason for throwing an exception.
   *
   * @return the string with the reason for throwing an exception.
   */
  const std::string why() const;

protected:
  // The reason for the exception.
  std::string why_;
};

/**
 * @short A standard exception that is thrown on non specified error.
 */
class GeneralException : public BaseReasonedException {
public:
  GeneralException(const char* file, const size_t line, const std::string& init_why = "");
};

/**
 * @short A standard exception that is thrown if a cast is invalid.
 */
class InvalidCast : public BaseReasonedException {
public:
  InvalidCast(const char* file, const size_t line, const std::string& init_why = "");
};

/**
 * @short A standard exception that is thrown if a range is invalid.
 */
class InvalidRange : public BaseReasonedException {
public:
  InvalidRange(const char* file, const size_t line, const std::string& init_why = "");
};


/**
 * @short A standard exception that is thrown if a string is invalid.
 */
class InvalidString : public BaseReasonedException {
public:
  InvalidString(const char* file, const size_t line, const std::string& init_why = "");
};


/**
 * @short A standard exception that is thrown on I/O error.
 */
class IOException : public BaseReasonedException {
public:
  IOException(const char* file, const size_t line, const std::string& init_why = "");
};

/**
 * @short A standard exception that is thrown on internal error.
 */
class InternalError : public BaseReasonedException {
public:
  InternalError(const char* file, const size_t line, const std::string& init_why = "");
};

/**
 * @short A standard exception that is thrown on logic error.
 */
class LogicError : public BaseReasonedException {
public:
  LogicError(const char* file, const size_t line, const std::string& init_why = "");
};

/**
 * @short A standard exception that is thrown on type error.
 */
class TypeError : public BaseReasonedException {
public:
  TypeError(const char* file, const size_t line, const std::string& init_why = "");
};

#endif // BASE_EXCEPTION_H__
