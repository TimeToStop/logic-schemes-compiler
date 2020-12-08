#ifndef BLOCK_H
#define BLOCK_H

using ID = unsigned long long int;

#include <QMap>
#include <QString>

enum class BlockType
{
    MONOPHASE,
    MULTIPHASE,
    CUSTOM
};

class Block
{
public:
    Block();
    ~Block();

    void setType(BlockType);
    void setTypeName(const QString&);
    void setInputs(const QStringList&);
    void setOutputs(const QStringList&);

    BlockType type() const;
    const QString& typeName() const;
    const QStringList& inputs() const;
    const QStringList& outputs() const;

private:
    BlockType _type;
    QString _type_name;
    QStringList _inputs;
    QStringList _outputs;
};

#endif // BLOCK_H
