#include "parser.h"

#include <QFile>
#include <QDir>
#include <QFileInfo>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonParseError>

#include <iostream>

Parser::Parser():
    _has_error(false)
{   
}

Parser::~Parser()
{
}

bool Parser::parse(const QString& path)
{
    try
    {
        QFileInfo info(path);
        QString absolute_path = info.absoluteFilePath();

        if (!_stack.contains(absolute_path))
        {
            ParserImpl impl(this);
            _stack.push(absolute_path);
            impl.parse(absolute_path);
            _stack.pop();
        }
    }
    catch (FatalParseException& e)
    {
        error("Compilation aborted:\nFatal error", {});
    }

    return !_has_error;
}

void Parser::error(QString error_template, const QStringList& args)
{
    _has_error = true;

    for(const QString& argument : args)
    {
        error_template = error_template.arg(argument);
    }

    for(int i = _stack.size() - 2; i >= 0; i--)
    {
        error_template = "In file included from \"" + _stack[i] + "\":\n" + error_template;
    }

    error_template = "Error in file \"" + _stack.top() + "\":\n" + error_template;
    std::cerr << error_template.toStdString() << std::endl;
}

bool Parser::insert(const SharedPtr<Schema>& schema)
{
    if (!_declared_schemas.contains(schema->typeName()))
    {
        _main_schema = schema;
        _declared_schemas.insert(schema->typeName(), schema);
        return true;
    }

    return false;
}

const SharedPtr<Schema> Parser::mainSchema() const
{
    return _main_schema;
}

const QMap<QString, SharedPtr<Schema>>& Parser::schemas() const
{
    return _declared_schemas;
}

