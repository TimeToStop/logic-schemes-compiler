#include "schema.h"

Schema::Schema():
    _type_name(),
    _inputs(),
    _outputs(),
    _blocks(),
    _connections()
{   
}

Schema::~Schema()
{
}

void Schema::setTypeName(const QString& type_name)
{
    _type_name = type_name;
}

void Schema::setInputs(const QList<QPair<ID, QString>>& inputs)
{
    _inputs = inputs;
}

void Schema::setOutputs(const QList<QPair<ID, QString>>& outputs)
{
    _outputs = outputs;
}

void Schema::setBlocks(const QMap<ID, SharedPtr<Block>>& blocks)
{
    _blocks = blocks;
}

void Schema::setConnections(const QList<SharedPtr<Connection>>& connections)
{
    _connections = connections;
}

const QString& Schema::typeName() const
{
    return _type_name;
}

const QList<QPair<ID, QString>>& Schema::inputs() const
{
    return _inputs;
}

const QList<QPair<ID, QString>>& Schema::outputs() const
{
    return _outputs;
}

const QMap<ID, SharedPtr<Block>>& Schema::blocks() const
{
    return _blocks;
}

const QList<SharedPtr<Connection>>& Schema::connections() const
{
    return _connections;
}
