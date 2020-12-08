#include <iostream>

#include "parser/parser.h"
#include "generator/generator.h"

int main(int argc, char *argv[])
{
    Parser p;

    if (p.parse("../test/test.json"))
    {
        Generator g;
        g.generate("../test/out", p.mainSchema(), p.schemas());
        std::cout << "Compilation finished" << std::endl;
    }
    else
    {
         std::cout << "Compilation aborted due to errors" << std::endl;
    }

    Q_UNUSED(argc);
    Q_UNUSED(argv);
    return 0;
}
