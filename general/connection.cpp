#include "connection.h"

Connection::Connection():
    _input_id(0),
    _input_name(),
    _output_id(0),
    _output_name()
{
}

Connection::~Connection()
{
}

void Connection::setInputId(const ID& id)
{
    _input_id = id;
}

void Connection::setInputName(const QString& name)
{
    _input_name = name;
}

void Connection::setOuputId(const ID& id)
{
    _output_id = id;
}

void Connection::setOutputName(const QString& name)
{
    _output_name = name;
}

ID Connection::inputID() const
{
    return _input_id;
}

const QString& Connection::inputName() const
{
    return _input_name;
}

ID Connection::outputID() const
{
    return _output_id;
}

const QString& Connection::outputName() const
{
    return _output_name;
}
