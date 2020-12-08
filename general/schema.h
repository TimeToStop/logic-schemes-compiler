#ifndef SCHEMA_H
#define SCHEMA_H

#include "block.h"
#include "connection.h"

#include <memory>
#include <QList>
#include <QSet>


// TODO: From QMap to QList QPair

template<typename T>
using SharedPtr = std::shared_ptr<T>;

class Schema
{
public:
    Schema();
    ~Schema();

    void setTypeName(const QString&);
    void setInputs(const QList<QPair<ID, QString>>&);
    void setOutputs(const QList<QPair<ID, QString>>&);
    void setBlocks(const QMap<ID, SharedPtr<Block>>&);
    void setConnections(const QList<SharedPtr<Connection>>&);

    const QString& typeName() const;
    const QList<QPair<ID, QString>>& inputs() const;
    const QList<QPair<ID, QString>>& outputs() const;
    const QMap<ID, SharedPtr<Block>>& blocks() const;
    const QList<SharedPtr<Connection>>& connections() const;

private:
    QString _type_name;
    QList<QPair<ID, QString>> _inputs;
    QList<QPair<ID, QString>> _outputs;
    QMap<ID, SharedPtr<Block>> _blocks;
    QList<SharedPtr<Connection>> _connections;
};

#endif // SCHEMA_H
