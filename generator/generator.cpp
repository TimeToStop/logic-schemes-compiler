#include "generator.h"

#include <QFile>
#include <QTextStream>
#include <QDir>

const QString Generator::_schema_template = QStringLiteral(
    "#pragma once\n"
    "#include \"single_include.hpp\"\n"
    "\n"
    "class %1 : public Schema, public std::enable_shared_from_this<%1> {\n"
    "public:\n"
    "   %1(): Schema() {}\n"
    "   %1(const %1&) = delete;\n"
    "   %1(%1&&) = delete;\n"
    "   ~%1() {}\n"
    "   virtual void construct() override {\n"
    "       std::shared_ptr<%1> schema = shared_from_this();\n"
    "       %2\n"
    "   }\n"
    "};\n"
);

const QString Generator::_pro_template = QStringLiteral(
    "QT -= gui\n\n"
    "CONFIG -= app_bundle\n"
    "CONFIG += c++17 console\n\n"
    "# Default rules for deployment.\n"
    "qnx: target.path = /tmp/$${TARGET}/bin\n"
    "else: unix:!android: target.path = /opt/$${TARGET}/bin\n"
    "!isEmpty(target.path): INSTALLS += target\n\n"
    "# Lib\n"
    "INCLUDEPATH += %1 \n\n"
    "HEADERS = %2 \n\n"
    "SOURCES = main.cpp \n\n"
);

const QString Generator::_main_template = QStringLiteral(
    "#include \"single_include.hpp\"\n"
    "\n"
    "int main(int argc, char* argv[])\n"
    "{\n"
    "    UserInput input(argc, argv);\n"
    "    LifeSpan lifespan;\n"
    "    std::shared_ptr<%1> schema = std::make_shared<%1>();\n"
    "    schema->construct();\n"
    "    input.initIO(schema->inputs(), schema->outputs());\n"
    "    lifespan.execute();\n"
    "    return 0;\n"
    "}\n"
);

const QString Generator::_single_include_template = QStringLiteral(
    "#pragma once\n"
    "#include <lib/logic_schemes_lib.hpp>\n"
    "\n"
    "\n"
    "%1\n"
);

Generator::Generator()
{
}

Generator::~Generator()
{
}

void Generator::generate(const QString& path, const SharedPtr<Schema>& main_schema, const QMap<QString, SharedPtr<Schema>>& schemas)
{
    QFile pro(path + "/" + main_schema->typeName().toLower() + ".pro"),
          main(path + "/main.cpp"),
          single_include(path + "/single_include.hpp");

    if (pro.open(QIODevice::WriteOnly))
    {
        pro.write(generateProFile(schemas));
        pro.close();
    }

    if (main.open(QIODevice::WriteOnly))
    {
        main.write(generateMainFile(main_schema));
        main.close();
    }

    if (single_include.open(QIODevice::WriteOnly))
    {
        single_include.write(generateSingleInclude(schemas));
        single_include.close();
    }

    for(const SharedPtr<Schema>& schema : schemas)
    {
        QFile file(path + "/" + schema->typeName() + ".hpp");

        if(file.open(QIODevice::WriteOnly))
        {
            QByteArray data = generateSchemaClass(schema);
            file.write(data);
            file.close();
        }
    }
}

QByteArray Generator::generateSchemaClass(const SharedPtr<Schema>& schema)
{
    QString string;
    QTextStream stream(&string);

    for(const ID& id : schema->blocks().keys())
    {
        SharedPtr<Block> block = schema->blocks()[id];
        QString name = block->typeName() + QString::number(id);

        stream << "std::shared_ptr<" << block->typeName() << "> " << name
               << " = schema->add<" << block->typeName() << ">();";

        stream << name << "->setId(" << QString::number(id) << ");";

        if (block->type() != BlockType::CUSTOM)
        {
            for(const QString& input : block->inputs())
            {
                stream << name << "->addInput(\"" << input << "\");";
            }

            for(const QString& output : block->outputs())
            {
                stream << name << "->addOutput(\"" << output << "\");";
            }
        }
        else
        {
            stream << name << "->construct();";
        }
    }

    for(const SharedPtr<Connection>& connection : schema->connections())
    {
        SharedPtr<Block> input_block = schema->blocks()[connection->inputID()];
        SharedPtr<Block> output_block = schema->blocks()[connection->outputID()];
        QString input  = input_block->typeName() + QString::number(connection->inputID());
        QString output = output_block->typeName() + QString::number(connection->outputID());

        stream << "schema->connect(" << output << "->output(" << output_block->outputs().indexOf(connection->outputName()) << ")" << ","
                                    << input << "->input(" << input_block->inputs().indexOf(connection->inputName()) << ")" << ");";
    }

    for(const QPair<ID, QString>& p  : schema->inputs())
    {
        ID id = p.first;
        QString name = p.second;
        SharedPtr<Block> input_block = schema->blocks()[id];
        QString input  = input_block->typeName() + QString::number(id);

        stream << "schema->_inputs.push_back(" << input << "->input(" << input_block->inputs().indexOf(name) << "));";
    }

    for(const QPair<ID, QString>& p  : schema->outputs())
    {
        ID id = p.first;
        QString name = p.second;
        SharedPtr<Block> output_block = schema->blocks()[id];
        QString output  = output_block->typeName() + QString::number(id);

        stream << "schema->_outputs.push_back(" << output << "->output(" << output_block->outputs().indexOf(name) << "));";
    }

    return _schema_template.arg(schema->typeName(), string).toUtf8();
}

QByteArray Generator::generateProFile(const QMap<QString, SharedPtr<Schema>>& schemas)
{
    QString headers = "\\ ";

    for(const QString& name : schemas.keys())
    {
        headers += name + ".hpp \\ \n";
    }

    headers += "single_include.hpp \n";

    return _pro_template.arg(QDir::currentPath(), headers).toUtf8();
}

QByteArray Generator::generateMainFile(const SharedPtr<Schema>& schema)
{
    return _main_template.arg(schema->typeName()).toUtf8();
}

QByteArray Generator::generateSingleInclude(const QMap<QString, SharedPtr<Schema>>& schemas)
{
    QString includes = "";

    for(const QString& name : schemas.keys())
    {
        includes += "#include \"" + name + ".hpp\" \n";
    }

    return _single_include_template.arg(includes).toUtf8();
}
