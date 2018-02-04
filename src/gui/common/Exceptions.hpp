#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <exception>


namespace mdl {
  class Exception : public std::exception
  {
  public:
    virtual const char* what() const throw() = 0;
  };


  class VideoNotOpenedException : public Exception
  {
  public:
    virtual const char* what() const throw() {
      return "Failed to open video file";
    }
  };
}


#endif // EXCEPTIONS_H
