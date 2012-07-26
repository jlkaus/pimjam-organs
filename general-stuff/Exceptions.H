#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>
#include <string>
#include <stdint.h>

class OrganException: public std::exception {};

class OrganIntException: public OrganException
{
  public:
    OrganIntException(uint32_t ex) : xError(ex) {}
    OrganIntException(const OrganIntException& ex) : xError(ex.xError) {}
    OrganIntException& operator= (const OrganIntException& ex) { xError = ex.xError; return *this; }
    virtual ~OrganIntException() throw() {}

    virtual const char* what() const throw() {
      static char buffer[32];
      snprintf(buffer, sizeof(buffer), "%d", xError);
      return buffer;
    }

  private:
    uint32_t xError;
};

class OrganStringException: public OrganException
{
  public:
    OrganStringException(const char* ex) : xError(ex) {}
    OrganStringException(const std::string& ex) : xError(ex) {}
    OrganStringException(const OrganStringException& ex) : xError(ex.xError) {}
    OrganStringException& operator= (const OrganStringException& ex) { xError = ex.xError; return *this; }
    virtual ~OrganStringException() throw() {}

    virtual const char* what() const throw() { return xError.c_str(); }

  private:
    std::string xError;
};

#endif
