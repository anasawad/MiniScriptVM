#ifndef MINISCRIPTVM_H
#define MINISCRIPTVM_H

#include <QMap>
#include <QStack>
#include <QStringList>

#include "structinfo.h"
#include "function.h"
#include "frame.h"
#include "allocator.h"

struct VMException
{
    QString message;
    VMException(QString message) : message(message)
    {
    }
};


//Marked not understand
typedef Value* (*ExternalMethodType)(QVector<Value *>, Allocator &);
class miniScriptVM
{

    QMap<QString, Value*>             m_constantPool;
    QMap<QString, Function*>          m_definedFunctions;
    QMap<QString, StructInfo>         m_definedStructs;
    QMap<QString, ExternalMethodType> m_definedExternalMethods;
    QMap<QString, int>                m_externalMethodArities;
    QStack<Frame>                     m_callStack;
    Allocator                         m_allocator;
    bool                              m_running;

public:
    miniScriptVM():m_allocator(m_callStack){}

    void Defined_StringConstant(QString p_key, QString p_string);
    void Defined_Function(QString p_name,Function *p_function);
    void Defined_Struct(QString p_name, QStringList p_fields);
    void Defined_ExternalMethod(QString, ExternalMethodType,int);

    void Load(QString p_assemblyCode);
    void Call_Main();
    void Execute();


private:
    void Do_lbl(Instruction &, Frame &);
    void Do_jmp(Instruction &, Frame &);
    void Do_push_int(Instruction &, Frame &);
    void Do_push_null(Instruction &, Frame &);
    void Do_push_string(Instruction &, Frame &);
    void Do_push_local(Instruction &, Frame &);
    void Do_pop_local(Instruction &, Frame &);
    void Do_new_obj(Instruction &, Frame &);
    void Do_set_field(Instruction &, Frame &);
    void Do_get_field(Instruction &, Frame &);
    void Do_add(Instruction &, Frame &);
    void Do_sub(Instruction &, Frame &);
    void Do_mul(Instruction &, Frame &);
    void Do_div(Instruction &, Frame &);
    void Do_And(Instruction &, Frame &);
    void Do_Or(Instruction &, Frame &);
    void Do_Not(Instruction &, Frame &);
    void Do_eql(Instruction &, Frame &);
    void Do_neq(Instruction &, Frame &);
    void Do_less(Instruction &, Frame &);
    void Do_greater(Instruction &, Frame &);
    void Do_geq(Instruction &, Frame &);
    void Do_leq(Instruction &, Frame &);
    void Do_jz(Instruction &, Frame &);
    void Do_jnz(Instruction &, Frame &);
    void Do_call(Instruction &, Frame &);
    void Do_ret(Instruction &, Frame &);
    void Do_call_external(Instruction &, Frame &);
};

#endif // MINISCRIPTVM_H
