#ifndef FUNCTION_H
#define FUNCTION_H

#include <QVector>
#include <QMap>
#include "instruction.h"

struct Function{
    QVector<Instruction> m_instructions;
    QMap<QString, int>   m_labels;
    int                  m_arity;
};


#endif // FUNCTION_H
