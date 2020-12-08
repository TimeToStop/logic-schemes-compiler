#ifndef PARSER_H
#define PARSER_H

#include "parserimpl.h"

#include <QStack>

class Parser
{
public:
    Parser();
    ~Parser();

    bool parse(const QString&);
    void error(QString error_template, const QStringList&);

    bool insert(const SharedPtr<Schema>&);

    const SharedPtr<Schema> mainSchema() const;
    const QMap<QString, SharedPtr<Schema>>& schemas() const;

private:
    bool _has_error;
    QStack<QString> _stack;
    SharedPtr<Schema> _main_schema;
    QMap<QString, SharedPtr<Schema>> _declared_schemas;
};

#endif // PARSER_H
