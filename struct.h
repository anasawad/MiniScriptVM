#ifndef STRUCT_H
#define STRUCT_H

#include <QMap>
#include <QString>
#include"value.h"
#include "miniscriptvm.h"
#include "structInfo.h"

class StructInstance: public Value
{
public:
    StructInstance();
    QString ToString();
    int Size_In_Bytes(){return sizeof(StructInstance);}

    QMap<QString, Value*> m_fields;
    StructInfo            m_type;

};

#endif // STRUCT_H
