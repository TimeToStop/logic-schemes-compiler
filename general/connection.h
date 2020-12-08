#ifndef CONNECTION_H
#define CONNECTION_H

#include "block.h"

class Connection
{
public:
    Connection();
    ~Connection();

    void setInputId(const ID&);
    void setInputName(const QString&);
    void setOuputId(const ID&);
    void setOutputName(const QString&);

    ID inputID() const;
    const QString& inputName() const;
    ID outputID() const;
    const QString& outputName() const;

private:
    ID _input_id;
    QString _input_name;
    ID _output_id;
    QString _output_name;
};

#endif // CONNECTION_H
