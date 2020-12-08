#ifndef PARSERIMPL_H
#define PARSERIMPL_H

#include "../general/schema.h"
#include "fatalparseexception.h"

#include <QJsonValue>

class Parser;

class ParserImpl
{
private:
    static const QStringList _schema_fields;
    static const QStringList _block_fields;
    static const QStringList _connection_fields;

    static const QStringList _monophase_schema;
    static const QStringList _multiphase_schema;

public:
    ParserImpl(Parser*);
    ~ParserImpl();

    void parse(const QString&);

private:
    void parseJson(const QString&, const QByteArray&);

    void parseUsing(QJsonValue&, const QString&);
    QList<QPair<ID, QString>> parseGlobalIO(QJsonValue&);
    QStringList parseIO(QJsonValue&);
    QString parseTypeName(QJsonValue&);
    QMap<ID, SharedPtr<Block>> parseBlocks(QJsonValue&);
    QList<SharedPtr<Connection>> parseConnections(QJsonValue&);

private:
    Parser* _parser;
};

#endif // PARSERIMPL_H
