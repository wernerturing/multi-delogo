#ifndef FG_EXCEPTIONS_H
#define FG_EXCEPTIONS_H

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


#endif // FG_EXCEPTIONS_H
