#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>

namespace fg {
  class Exception : public std::exception
  {
    virtual const char* what() const throw() = 0;
  };


  class InvalidParametersException : public Exception
  {
  public:
    virtual const char* what() const throw()
    {
        return "Invalid parameters for filter";
    }
  };
}


#endif // EXCEPTIONS_H
