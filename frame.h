#ifndef FRAME_H
#define FRAME_H

#include <QStack>
#include <QMap>
#include "function.h"
#include "value.h"

struct Frame{
    QMap<QString, Value*> m_localVariables;
    QStack<Value*>        m_operandStack;
    int                   m_iPointer;
    Function*             m_method;

};

#endif // FRAME_H
