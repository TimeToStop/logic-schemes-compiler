#include "block.h"

Block::Block():
    _type(BlockType::CUSTOM),
    _type_name(),
    _inputs(),
    _outputs()
{  
}

Block::~Block()
{
}

void Block::setType(BlockType type)
{
    _type = type;
}

void Block::setTypeName(const QString& type_name)
{
    _type_name = type_name;
}

void Block::setInputs(const QStringList& inputs)
{
    _inputs = inputs;
}

void Block::setOutputs(const QStringList& outputs)
{
    _outputs = outputs;
}

BlockType Block::type() const
{
    return _type;
}

const QString& Block::typeName() const
{
    return _type_name;
}

const QStringList &Block::inputs() const
{
    return _inputs;
}

const QStringList &Block::outputs() const
{
    return _outputs;
}
