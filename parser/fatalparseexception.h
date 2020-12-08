#ifndef PARSEEXCEPTION_H
#define PARSEEXCEPTION_H

#include <stdexcept>
#include <QString>

class FatalParseException : public std::exception
{
public:
    FatalParseException();
    virtual ~FatalParseException();
};

#endif // PARSEEXCEPTION_H
