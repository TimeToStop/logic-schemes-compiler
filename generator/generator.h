#ifndef GENERATOR_H
#define GENERATOR_H

#include "../general/schema.h"

class Generator
{
private:
    static const QString _schema_template;
    static const QString _pro_template;
    static const QString _main_template;
    static const QString _single_include_template;

public:
    Generator();
    ~Generator();

    void generate(const QString&, const SharedPtr<Schema>&, const QMap<QString, SharedPtr<Schema>>&);

private:
    QByteArray generateSchemaClass(const SharedPtr<Schema>&);
    QByteArray generateProFile(const QMap<QString, SharedPtr<Schema>>&);
    QByteArray generateMainFile(const SharedPtr<Schema>&);
    QByteArray generateSingleInclude(const QMap<QString, SharedPtr<Schema>>&);
};

#endif // GENERATOR_H
