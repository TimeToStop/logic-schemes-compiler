QT -= gui

CONFIG += c++17 console
CONFIG -= app_bundle

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


SOURCES += \
    general/block.cpp \
    general/connection.cpp \
    general/schema.cpp \
    generator/generator.cpp \
    main.cpp \
    parser/fatalparseexception.cpp \
    parser/parser.cpp \
    parser/parserimpl.cpp

HEADERS += \
    build/lib/logic_schemes_lib.hpp \
    general/block.h \
    general/connection.h \
    general/schema.h \
    generator/generator.h \
    parser/fatalparseexception.h \
    parser/parser.h \
    parser/parserimpl.h \
    test/out/single_include.h

DISTFILES += \
    .gitignore \
    readme.md \
    test/test.json \
    test/use.json
