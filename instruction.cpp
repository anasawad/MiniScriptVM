#include "instruction.h"

Instruction::Instruction()
{

}

Instruction::Instruction(OpCode p_opCode)
{
    this->m_opCode= p_opCode;
}

Instruction::Instruction(OpCode p_opCode, int p_intVal)
{
    this->m_opCode = p_opCode;
    this->m_intVal = p_intVal;
}

Instruction::Instruction(OpCode p_opCode, QString p_strSymbol)
{
    this->m_opCode = p_opCode;
    this->m_symbol = p_strSymbol;
}
