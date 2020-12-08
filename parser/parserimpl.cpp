#include "parserimpl.h"

#include "parser.h"

#include <QFile>
#include <QFileInfo>
#include <QDir>

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

const QStringList ParserImpl::_schema_fields =
{
    "using",
    "typename",
    "inputs",
    "outputs",
    "blocks",
    "connections"
};

const QStringList ParserImpl::_block_fields =
{
    "typename",
    "id"
};

const QStringList ParserImpl::_connection_fields =
{
    "input-id",
    "input-name",
    "output-id",
    "output-name"
};

const QStringList  ParserImpl::_monophase_schema =
{
    "Buffer",
    "Not"
};

const QStringList  ParserImpl::_multiphase_schema =
{
    "And",
    "AndNot",
    "Or",
    "OrNot"
};

ParserImpl::ParserImpl(Parser* parser):
    _parser(parser)
{
}

ParserImpl::~ParserImpl()
{
}

void ParserImpl::parse(const QString& path)
{
    QFile f(path);
    QFileInfo info(f);

    if (f.open(QIODevice::ReadOnly))
    {
        QByteArray data = f.readAll();
        f.close();
        parseJson(info.absoluteDir().absolutePath(), data);
    }
    else
    {
        _parser->error("File is not readable \"%1\"", { info.absoluteFilePath() });
        throw FatalParseException();
    }
}

void ParserImpl::parseJson(const QString& path, const QByteArray& json)
{
    QJsonParseError error;
    QJsonDocument document = QJsonDocument::fromJson(json, &error);

    if(error.error != QJsonParseError::NoError)
    {
        _parser->error("Invalid json: %1", { error.errorString() });
    }
    else if (!document.isObject())
    {
        _parser->error("Schema is not an object", {});
    }
    else
    {
        QJsonObject object = document.object();
        QStringList fields = object.keys();

        for(const QString& field : _schema_fields)
        {
            if(!fields.contains(field))
            {
                _parser->error("Schema does not contains property: \"%1\"", { field });
                throw FatalParseException();
            }
        }

        QJsonValue _using = object["using"];
        QJsonValue inputs = object["inputs"];
        QJsonValue outputs = object["outputs"];
        QJsonValue type_name = object["typename"];
        QJsonValue blocks = object["blocks"];
        QJsonValue connections = object["connections"];

        parseUsing(_using, path);

        SharedPtr<Schema> schema = std::make_shared<Schema>();

        schema->setInputs(parseGlobalIO(inputs));
        schema->setOutputs(parseGlobalIO(outputs));
        schema->setConnections(parseConnections(connections));
        schema->setBlocks(parseBlocks(blocks));
        schema->setTypeName(parseTypeName(type_name));

        for(const QPair<ID, QString>& p : schema->inputs())
        {
            ID id = p.first;
            QString name = p.second;

            if (!schema->blocks().contains(id))
            {
                _parser->error("Global input id = \"%1\" not found", {QString::number(id)});
            }
            else if (schema->blocks()[id]->inputs().indexOf(name) == -1)
            {
                _parser->error("Global input name = \"%1\" not found", {name});
            }
        }

        for(const QPair<ID, QString>& p : schema->outputs())
        {
            ID id = p.first;
            QString name = p.second;

            if (!schema->blocks().contains(id))
            {
                _parser->error("Global input id = \"%1\" not found", {QString::number(id)});
            }
            else if (schema->blocks()[id]->outputs().indexOf(name) == -1)
            {
                _parser->error("Global input name = \"%1\" not found", {name});
            }
        }

        for(const SharedPtr<Connection> & connection : schema->connections())
        {
            if (!schema->blocks().contains(connection->inputID()))
            {
                _parser->error("Block with id = %1 does not exists in \"%2\"", {QString::number(connection->inputID()), schema->typeName()});
            }
            else if (!schema->blocks()[connection->inputID()]->inputs().contains(connection->inputName()))
            {
                _parser->error("Block with id = %1 does not contains input with name = \"%2\"",
                               { QString::number(connection->inputID()),connection->inputName()});
            }

            if (!schema->blocks().contains(connection->outputID()))
            {
                _parser->error("Block with id = %1 does not exists in \"%2\"", {QString::number(connection->outputID()), schema->typeName()});
            }
            else if (!schema->blocks()[connection->outputID()]->outputs().contains(connection->outputName()))
            {
                _parser->error("Block with id = %1 does not contains input with name = \"%2\"",
                               { QString::number(connection->outputID()),connection->outputName()});
            }
        }

        _parser->insert(schema);
    }
}

void ParserImpl::parseUsing(QJsonValue& values, const QString& path)
{
    if (!values.isArray())
    {
        _parser->error("Property \"using\" is not an array", {});
    }
    else
    {
        QJsonArray paths = values.toArray();

        for(const QJsonValue& value : paths)
        {
            if (!value.isString())
            {
                _parser->error("Path to file is not a string", {});
            }
            else
            {
                _parser->parse(path + "/" + value.toString());
            }
        }
    }
}

QList<QPair<ID, QString>> ParserImpl::parseGlobalIO(QJsonValue& value)
{
    if (!value.isArray())
    {
        _parser->error("Global Input/Output is not an array", {});
        throw FatalParseException();
    }
    else
    {
        QList<QPair<ID, QString>> ios;
        QJsonArray array = value.toArray();

        for(const QJsonValue& io : array)
        {
            if (!io.isObject())
            {
                _parser->error("Global Input/Output is not an object", {});
            }
            else
            {
                QJsonObject io_object = io.toObject();

                if (!io_object.contains("id") || !io_object.contains("name"))
                {
                    _parser->error("Global input/output does not contains id or name", { });
                    continue;
                }

                QJsonValue io_id = io_object["id"];
                QJsonValue io_name = io_object["name"];

                if (!io_id.isDouble() || io_id.toInt(-1) < 0)
                {
                    _parser->error("Invalid id on global input/output", { });
                    continue;
                }

                if(!io_name.isString())
                {
                    _parser->error("Global input/output name in not a string", { });
                    continue;
                }

                ios.append(QPair<ID, QString>(io_id.toInt(), io_name.toString()));
            }
        }

        return ios;
    }
}

QStringList ParserImpl::parseIO(QJsonValue& value)
{
    if (!value.isArray())
    {
        _parser->error("Input/Output is not an array", {});
        throw FatalParseException();
    }
    else
    {
        QStringList ios;
        QJsonArray array = value.toArray();

        for(const QJsonValue& io : array)
        {
            if (!io.isString())
            {
                _parser->error("Input/Output is not a string", {});
            }
            else
            {
                QString io_name = io.toString();

                if (ios.contains(io_name))
                {
                    _parser->error("Input/Output name is not unique: %1", { io_name });
                }
                else
                {
                    ios.append(io_name);
                }
            }
        }

        return ios;
    }
}

QString ParserImpl::parseTypeName(QJsonValue& type)
{
    if (!type.isString())
    {
        _parser->error("Property \"typename\" is not a string", {});
        throw FatalParseException();
    }
    else
    {
        QString name = type.toString();

        if(_parser->schemas().contains(name)
                || _monophase_schema.contains(name)
                || _multiphase_schema.contains(name))
        {
            _parser->error("Schema with type \"%1\" is already declared or default", { name });
        }

        return name;
    }
}

QMap<ID, SharedPtr<Block>> ParserImpl::parseBlocks(QJsonValue& value)
{
    if (!value.isArray())
    {
        _parser->error("Property \"blocks\" is not an array", {});
        throw FatalParseException();
    }
    else
    {
        QMap<ID, SharedPtr<Block>> blocks;

        for(const QJsonValue& block : value.toArray())
        {
            if (!block.isObject())
            {
                _parser->error("Block is not an object", {});
            }
            else
            {
                QJsonObject object = block.toObject();
                QStringList fields = object.keys();

                bool valid = true;

                for(const QString& field : _block_fields)
                {
                    if(!fields.contains(field))
                    {
                        _parser->error("Block does not contains property: \"%1\"", {field});
                        valid = false;
                        break;
                    }
                }

                if (!valid)
                {
                    continue;
                }

                SharedPtr<Block> block = std::make_shared<Block>();

                QJsonValue id = object["id"];
                QJsonValue type = object["typename"];

                if(!id.isDouble())
                {
                    _parser->error("Property \"id\" of block is not a number", {});
                    continue;
                }
                else if (blocks.contains(id.toInt()))
                {
                    _parser->error("Property \"id\" of block is not a non-negative int", {});
                    continue;
                }

                // To Do verify Type
                if(!type.isString())
                {
                    _parser->error("Property \"typename\" is not a string", { });
                    continue;
                }
                else
                {
                    QString type_val = type.toString();
                    block->setTypeName(type_val);

                    if (_monophase_schema.contains(type_val))
                    {
                        QJsonValue inputs =  object["inputs"];
                        QJsonValue outputs = object["outputs"];

                        block->setInputs(parseIO(inputs));
                        block->setOutputs(parseIO(outputs));

                        if (block->inputs().size() != 1
                            || block->outputs().size() != 1)
                        {
                            _parser->error("Monophase schema does not containts 1 input and 1 output", {});
                            throw FatalParseException();
                        }

                        block->setType(BlockType::MONOPHASE);
                    }
                    else if (_multiphase_schema.contains(type_val))
                    {
                        QJsonValue inputs =  object["inputs"];
                        QJsonValue outputs = object["outputs"];

                        block->setInputs(parseIO(inputs));
                        block->setOutputs(parseIO(outputs));

                        if (block->inputs().size() == 0 || block->outputs().size() != 1)
                        {
                            _parser->error("Multiphase schema does not containts any input or 1 output", {});
                            throw FatalParseException();
                        }

                        block->setType(BlockType::MULTIPHASE);
                    }
                    else if (_parser->schemas().contains(type_val))
                    {
                        QStringList inputs, outputs;

                        for(const QPair<ID, QString>& input : _parser->schemas()[type_val]->inputs())
                        {
                            inputs.append(input.second);
                        }

                        for(const QPair<ID, QString>& output : _parser->schemas()[type_val]->outputs())
                        {
                            outputs.append(output.second);
                        }

                        block->setInputs(inputs);
                        block->setOutputs(outputs);
                        block->setType(BlockType::CUSTOM);
                    }
                    else
                    {
                        _parser->error("Unknown type: \"%1\"", {type_val});
                        throw FatalParseException();
                    }
                }

                blocks.insert(id.toInt(), block);
            }
        }

        return blocks;
    }
}

QList<SharedPtr<Connection>> ParserImpl::parseConnections(QJsonValue& value)
{
    if (!value.isArray())
    {
        _parser->error("Property \"blocks\" is not an array", {});
        throw FatalParseException();
    }
    else
    {
        QList<SharedPtr<Connection>> connections;
        QJsonArray array = value.toArray();

        for(const QJsonValue& connection : array)
        {
            if (!connection.isObject())
            {
                _parser->error("Property \"connections\" is not an object", {});
            }
            else
            {
                QJsonObject object = connection.toObject();
                QStringList fields = object.keys();

                bool valid = true;

                for(const QString& field : _connection_fields)
                {
                    if (!fields.contains(field))
                    {
                        _parser->error("Connection does not contains property: \"%1\"", {field});
                        valid = false;
                        break;
                    }
                }

                if (!valid)
                {
                    continue;
                }

                QJsonValue input_id = object["input-id"];
                QJsonValue input_name = object["input-name"];
                QJsonValue output_id = object["output-id"];
                QJsonValue output_name = object["output-name"];

                if (!input_id.isDouble() || input_id.toInt() < 0)
                {
                    _parser->error("Property in connection: \"input-id\" is not a number", {});
                    continue;
                }

                if (!input_name.isString())
                {
                    _parser->error("Property in connection: \"input-name\" is not a string", {});
                    continue;
                }

                if (!output_id.isDouble() || output_id.toInt() < 0)
                {
                    _parser->error("Property in connection: \"input-id\" is not a number", {});
                    continue;
                }

                if (!output_name.isString())
                {
                    _parser->error("Property in connection: \"output-name\" is not a string", {});
                    continue;
                }

                SharedPtr<Connection> conn = std::make_shared<Connection>();

                conn->setInputId(input_id.toInt());
                conn->setInputName(input_name.toString());
                conn->setOuputId(output_id.toInt());
                conn->setOutputName(output_name.toString());

                connections.append(conn);
            }
        }

        return connections;
    }
}
