#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <QString>

enum OpCode{
    lbl,
        jmp,
        push_int,
        push_null,
        push_string,
        push_local,
        pop_local,
        new_obj,
        set_field,
        get_field,
        add,
        sub,
        mul,
        divide,
        lAnd,
        lOr ,
        lNot,
        eql ,
        neq ,
        less,
        greater,
        geq,
        leq,
        jz,
        jnz,
        call,
        ret,
        call_external
};

class Instruction
{
public:
    //Declarations
    OpCode  m_opCode;
    int     m_intVal;
    QString m_symbol;

    //Methods
    Instruction();
    Instruction(OpCode p_opCode);
    Instruction(OpCode p_opCode, int p_intVal);
    Instruction(OpCode p_opCode,QString p_strSymbol);


    void Set_IntVal(int p_intVal);
    int Get_IntVal();

    void Set_StrSymbol(QString p_StrSymbol);
    QString Get_StrSymbol();


};

#endif // INSTRUCTION_H
