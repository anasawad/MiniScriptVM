#include "miniscriptvm.h"
#include "struct.h"
#include <iostream>
using namespace std;

void miniScriptVM::Defined_StringConstant(QString p_key, QString p_string)
{
    m_constantPool[p_key] = m_allocator.New_String(p_string);
}

void miniScriptVM::Defined_Function(QString p_name, Function *p_function)
{
    m_definedFunctions[p_name] = p_function;
}

void miniScriptVM::Defined_Struct(QString p_name, QStringList p_fields)
{
    StructInfo theStructInfo;
    theStructInfo.m_fieldNames = p_fields;
    theStructInfo.m_name       = p_name;
    m_definedStructs[p_name]   = theStructInfo;
}

void miniScriptVM::Defined_ExternalMethod(QString p_name, ExternalMethodType p_method, int p_arity)
{
    m_definedExternalMethods[p_name] = p_method;
    m_externalMethodArities[p_name]  = p_arity;
}

QString Hex_Decode(QString other)
{
    QByteArray array1 = other.toUtf8();
    QByteArray array2 = array1.toHex();
    return QString(array2);
}
QString Hex_Encode(QString source)
{
    QByteArray original = QByteArray::fromHex(source.toUtf8());
            QString ret = QString::fromUtf8(original.data());
    return ret;
}

void miniScriptVM::Call_Main()
{
    if(!m_definedFunctions.contains("main"))
        throw VMException("Ni Main Function Defined");
    Function *main = m_definedFunctions["main"];
    Frame frame;
    frame.m_iPointer = 0;
    frame.m_method = main;
    m_callStack.push(frame);
}

void miniScriptVM::Load(QString p_assemblyCode)
{
    QStringList lines = p_assemblyCode.split("\n", QString::SkipEmptyParts, Qt::CaseSensitive);
    QString currFuncName = "";
    Function* currFunction = NULL;

    QString currStructName = "";
    QStringList currStructFields;

    for(int i =0; i<lines.count(); i++)
        {
            QString line = lines[i];
            line = line.replace("\t"," ",Qt::CaseSensitive)
                    .replace("\r","",Qt::CaseSensitive)
                    .trimmed();
            if(line == "" || line.startsWith("//"))
                continue;

            QStringList lineParts = line.split(" ", QString::SkipEmptyParts, Qt::CaseSensitive);

            QString opcode;
            QString arg ;

            opcode = lineParts[0];
            arg = arg = lineParts.count()>1? lineParts[1] : "";

            if(opcode == ".string")
            {
                if(lineParts.count() == 3)
                {
                    QString &sym = arg;
                    QString data = Hex_Decode(lineParts[2]);
                    Defined_StringConstant(sym, data);
                }
                else
                {
                    throw VMException("Missing value in .string directive");
                }
            }
            else if(opcode == ".function")
            {
                currFuncName = arg.trimmed();
                if(lineParts.count()==3)
                {
                    int arity = lineParts[2].trimmed().toInt();
                    currFunction = new Function();
                    currFunction->m_arity = arity;
                    m_definedFunctions[currFuncName] = currFunction;
                }
                else
                    throw VMException(QString("Function definition incomplete"));
            }
            else if(opcode == ".endfunction")
            {
                currFunction = NULL;
                currFuncName = "";
            }

            else if(opcode == ".struct")
            {
                currStructName= arg.trimmed();
                currStructFields.clear();
            }
            else if(opcode == ".field")
            {
                currStructFields.append(arg);
            }

            else if(opcode == ".endstruct")
            {
                Defined_Struct(currStructName, currStructFields);
                currStructName = "";
            }
            else if(opcode == "lbl")
            {
                int n = currFunction->m_instructions.count();
                currFunction->m_instructions.append(Instruction(lbl));

                currFunction->m_labels[arg] = n;
            }
            else if(opcode == "jmp")
            {
                currFunction->m_instructions.append(Instruction(jmp, arg));
            }
            else if(opcode == "push_int")
            {
                bool ok;
                int n = arg.toInt(&ok);
                currFunction->m_instructions.append(Instruction(push_int, n));
            }
            else if(opcode == "push_null")
            {
                currFunction->m_instructions.append(Instruction(push_null));
            }
            else if(opcode == "push_string")
            {
                currFunction->m_instructions.append(Instruction(push_string, arg));
            }
            else if(opcode == "push_local")
            {
                currFunction->m_instructions.append(Instruction(push_local, arg));
            }
            else if(opcode == "pop_local")
            {
                currFunction->m_instructions.append(Instruction(pop_local, arg));
            }
            else if(opcode == "new_obj")
            {
                currFunction->m_instructions.append(Instruction(new_obj, arg));
            }
            else if(opcode == "set_field")
            {
                currFunction->m_instructions.append(Instruction(set_field, arg));
            }
            else if(opcode == "get_field")
            {
                currFunction->m_instructions.append(Instruction(get_field, arg));
            }
            else if(opcode == "add")
            {
                currFunction->m_instructions.append(Instruction(add));
            }
            else if(opcode == "sub")
            {
                currFunction->m_instructions.append(Instruction(sub));
            }
            else if(opcode == "mul")
            {
                currFunction->m_instructions.append(Instruction(mul));
            }
            else if(opcode == "div")
            {
                currFunction->m_instructions.append(Instruction(divide));
            }
            else if(opcode == "and")
            {
                currFunction->m_instructions.append(Instruction(lAnd));
            }
            else if(opcode == "ar")
            {
                currFunction->m_instructions.append(Instruction(lOr));
            }
            else if(opcode == "not")
            {
                currFunction->m_instructions.append(Instruction(lNot));
            }
            else if(opcode == "eql")
            {
                currFunction->m_instructions.append(Instruction(eql));
            }
            else if(opcode == "neq")
            {
                currFunction->m_instructions.append(Instruction(neq));
            }
            else if(opcode == "less")
            {
                currFunction->m_instructions.append(Instruction(::less));
            }
            else if(opcode == "greater")
            {
                currFunction->m_instructions.append(Instruction(::greater));
            }
            else if(opcode == "geq")
            {
                currFunction->m_instructions.append(Instruction(geq));
            }
            else if(opcode == "leq")
            {
                currFunction->m_instructions.append(Instruction(leq));
            }
            else if(opcode == "jz")
            {
                currFunction->m_instructions.append(Instruction(jz, arg));
            }
            else if(opcode == "jnz")
            {
                currFunction->m_instructions.append(Instruction(jnz, arg));
            }
            else if(opcode == "call")
            {
                currFunction->m_instructions.append(Instruction(call, arg));
            }
            else if(opcode == "ret")
            {
                currFunction->m_instructions.append(Instruction(ret));
            }
            else if(opcode == "call_external")
            {
                currFunction->m_instructions.append(Instruction(call_external, arg));
            }
        } // for
    }

void miniScriptVM::Execute()
{
    while(!m_callStack.empty())
    {
        Frame &frame = m_callStack.top();
        Instruction instruction = frame.m_method->m_instructions[frame.m_iPointer++];
        switch(instruction.m_opCode)
        {
            case lbl:
                Do_lbl(instruction,frame);
                break;
            case jmp:
                Do_jmp(instruction,frame);
                break;
            case push_int:
                Do_push_int(instruction,frame);
                break;
            case push_null:
                Do_push_null(instruction,frame);
                break;
            case push_string:
                Do_push_string(instruction,frame);
                break;
            case push_local:
                Do_push_local(instruction,frame);
                break;
            case pop_local:
                Do_pop_local(instruction,frame);
                break;
            case new_obj:
                Do_new_obj(instruction,frame);
                break;
            case set_field:
                Do_set_field(instruction,frame);
                break;
            case get_field:
                Do_get_field(instruction,frame);
                break;
            case add:
                Do_add(instruction,frame);
                break;
            case sub:
                Do_sub(instruction,frame);
                break;
            case mul:
                Do_mul(instruction,frame);
                break;
            case divide:
                Do_div(instruction,frame);
                break;
            case lAnd:
                Do_And(instruction,frame);
                break;
            case lOr:
                Do_Or(instruction,frame);
                break;
            case lNot:
                Do_Not(instruction,frame);
                break;
            case eql:
                Do_eql(instruction,frame);
                break;
            case neq:
                Do_neq(instruction,frame);
                break;
            case ::less:
                Do_less(instruction,frame);
                break;
            case ::greater:
                Do_greater(instruction,frame);
                break;
            case geq:
                Do_geq(instruction,frame);
                break;
            case leq:
                Do_leq(instruction,frame);
                break;
            case jz:
                Do_jz(instruction,frame);
                break;
            case jnz:
                Do_jnz(instruction,frame);
                break;
            case call:
                Do_call(instruction,frame);
                break;
            case ret:
                Do_ret(instruction,frame);
                break;
            case call_external:
                Do_call_external(instruction,frame);
                break;
            }

     }
}

void miniScriptVM::Do_lbl(Instruction &, Frame &)
{
    //nothing
}

void miniScriptVM::Do_jmp(Instruction &p_instruction, Frame &p_frame)
{
    p_frame.m_iPointer = p_frame.m_method->m_labels[p_instruction.m_symbol];
}

void miniScriptVM::Do_push_int(Instruction &p_instruction, Frame &p_frame)
{
    Value *val = m_allocator.New_Int(p_instruction.m_intVal);
    p_frame.m_operandStack.push(val);
}

void miniScriptVM::Do_push_null(Instruction &p_instruction, Frame &p_frame)
{
    p_frame.m_operandStack.push(Null::m_nullVal);
}

void miniScriptVM::Do_push_string(Instruction &p_instruction, Frame &p_frame)
{
    Value *val = m_constantPool[p_instruction.m_symbol];
    p_frame.m_operandStack.push(val);
}

void miniScriptVM::Do_push_local(Instruction &p_instruction, Frame &p_frame)
{
    Value *val = m_constantPool[p_instruction.m_symbol];
    p_frame.m_operandStack.push(val);
}

void miniScriptVM::Do_pop_local(Instruction &p_instruction, Frame &p_frame)
{
    Value *val = p_frame.m_localVariables[p_instruction.m_symbol];
    p_frame.m_operandStack.push(val);
}

void miniScriptVM::Do_new_obj(Instruction &p_instruction, Frame &p_frame)
{
    if(!m_definedStructs.contains(p_instruction.m_symbol))
         throw VMException(QString("Undefined struct %1").arg(p_instruction.m_symbol));

    StructInfo structInfo = m_definedStructs[p_instruction.m_symbol];
    Value *val = m_allocator.New_structInstacne(structInfo);
    p_frame.m_operandStack.push(val);
}

void miniScriptVM::Do_set_field(Instruction &p_instruction, Frame &p_frame)
{
    Value *val = p_frame.m_operandStack.pop();
    Value *obj = p_frame.m_operandStack.pop();

    StructInstance *structInstance = dynamic_cast<StructInstance *>(obj);
    if(structInstance == NULL)
    {
         throw VMException("setting field of non-struct");
    }
    if(!structInstance->m_fields.contains(p_instruction.m_symbol))
    {
        throw VMException(QString("Setting undefined field '%1' in type '%2").arg(p_instruction.m_symbol).arg(structInstance->m_type.m_name));
    }
    structInstance->m_fields[p_instruction.m_symbol] = val;
}

void miniScriptVM::Do_get_field(Instruction &p_instruction, Frame &p_frame)
{
    Value *obj = p_frame.m_operandStack.pop();

    StructInstance *structInstance = dynamic_cast<StructInstance *>(obj);

    if(structInstance == NULL)
    {
        throw VMException("Getting field of non-struct");
    }
    if(!structInstance->m_fields.contains(p_instruction.m_symbol))
    {
        throw VMException(QString("Getting undefined field '%1' in type '%2").arg(p_instruction.m_symbol).arg(structInstance->m_type.m_name));
    }

    Value *val = structInstance->m_fields[p_instruction.m_symbol];
    p_frame.m_operandStack.push(val);
}

void miniScriptVM::Do_add(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    Int *intVal1 = dynamic_cast<Int *> (val1);
    Int *intVal2 = dynamic_cast<Int *> (val2);

    if(intVal1 && intVal2)
    {
        int result = intVal1->m_value + intVal2->m_value;
        Value *val3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(val3);
    }

    String *s1 = dynamic_cast<String *>(val1);
    String *s2 = dynamic_cast<String *>(val2);

    if(s1 && s2)
    {
        QString result = s1->m_strValue + s2->m_strValue;
        Value *val3 = m_allocator.New_String(result);
        p_frame.m_operandStack.push(val3);
        return;
    }
    throw VMException("Can add only two ints or two strings");
}

void miniScriptVM::Do_sub(Instruction &p_instruction, Frame &p_frame)
{
    Value *val2 = p_frame.m_operandStack.pop();
    Value *val1 = p_frame.m_operandStack.pop();

    Int *intVal1 = dynamic_cast<Int *>(val1);
    Int *intVal2 = dynamic_cast<Int *>(val2);

    if(intVal1 && intVal2)
    {
        int result = intVal1->m_value - intVal2->m_value;
        Value *val3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(val3);
        return;
    }
    throw VMException("Can subtract only two ints");
}

void miniScriptVM::Do_mul(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    Int *intVal1 = dynamic_cast<Int *>(val1);
    Int *intVal2 = dynamic_cast<Int *>(val2);

    if(intVal1 && intVal2)
    {
        int result = intVal1->m_value * intVal2->m_value;
        Value *val3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(val3);
        return;
    }
    throw VMException("Can multiply only two ints");
}

void miniScriptVM::Do_div(Instruction &p_instruction, Frame &p_frame)
{
    Value *val2 = p_frame.m_operandStack.pop();
    Value *val1 = p_frame.m_operandStack.pop();

    Int *intVal1 = dynamic_cast<Int *>(val1);
    Int *intVal2 = dynamic_cast<Int *>(val2);

    if(intVal1 && intVal2)
    {
        if(intVal2->m_value == 0)
            throw VMException("Division by zero");
        int result = intVal1->m_value/intVal2->m_value;
        Value *val3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(val3);
        return;
    }
    throw VMException("Can divide only two ints");
}

void miniScriptVM::Do_And(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    Int *i1 = dynamic_cast<Int *>(val1);
    Int *i2 = dynamic_cast<Int *>(val1);

    if(i1 && i2)
    {
        int result = i1->m_value && i2->m_value;
        Value *v3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(v3);
        return;
    }
    throw VMException("Can apply logical operation only two ints");
}

void miniScriptVM::Do_Or(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    Int *i1 = dynamic_cast<Int *>(val1);
    Int *i2 = dynamic_cast<Int *>(val2);

    if(i1 && i2)
    {
        int result = i1->m_value || i2->m_value;
        Value *v3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(v3);
        return;
    }
     throw VMException("Can apply logical operation only two ints");
}

void miniScriptVM::Do_Not(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();

    Int *i1 = dynamic_cast<Int *>(val1);

    if(i1)
    {
        int result = !i1->m_value;
        Value *v3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(v3);
        return;
    }
    throw VMException("Can apply logical operation only to an int");
}

void miniScriptVM::Do_eql(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    Int *intVal1 = dynamic_cast<Int *>(val1);
    Int *intVal2 = dynamic_cast<Int *>(val2);

    if(intVal1 && intVal2)
    {
        int result = intVal1->m_value == intVal2->m_value;
        Value *v3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(v3);
        return;
    }

    String *s1 = dynamic_cast<String *>(val1);
    String *s2 = dynamic_cast<String *>(val2);
    if(s1 && s2)
    {
        int result = s1->m_strValue == s2->m_strValue;
        Value *v3 = m_allocator.New_Int(result);
        p_frame.m_operandStack.push(v3);
        return;
    }

    Null *n1 = dynamic_cast<Null *>(val1);
    Null *n2 = dynamic_cast<Null *>(val2);

    if(n1 && n2)
      {
          int result = 1;
          Value *v3 = m_allocator.New_Int(result);
          p_frame.m_operandStack.push(v3);
          return;
      }
      // Otherwise: Not equal
      Value *False = m_allocator.New_Int(0);
      p_frame.m_operandStack    .push(False);
}

void miniScriptVM::Do_neq(Instruction &p_instruction, Frame &p_frame)
{
    Do_eql(p_instruction,p_frame);
    Do_Not(p_instruction,p_frame);
}

void miniScriptVM::Do_less(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    // Are they two Int's?
   Int *intVal1 = dynamic_cast<Int *>(val1);
   Int *intVal2 = dynamic_cast<Int *>(val2);
   if(intVal1 && intVal2)
   {
       int result = intVal1->m_value < intVal2->m_value;
       Value *val3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(val3);
       return;
   }

   // Are they two String's?
   String *s1 = dynamic_cast<String *>(val1);
   String *s2 = dynamic_cast<String *>(val2);
   if(s1 && s2)
   {
       int result = s1->m_strValue < s2->m_strValue;
       Value *v3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(v3);
       return;
   }

   // Otherwise: Type error!
   throw VMException("Can compare only two ints or two strings");
}

void miniScriptVM::Do_greater(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    // Are they two Int's?
   Int *intVal1 = dynamic_cast<Int *>(val1);
   Int *intVal2 = dynamic_cast<Int *>(val2);
   if(intVal1 && intVal2)
   {
       int result = intVal1->m_value > intVal2->m_value;
       Value *val3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(val3);
       return;
   }

   // Are they two String's?
   String *s1 = dynamic_cast<String *>(val1);
   String *s2 = dynamic_cast<String *>(val2);
   if(s1 && s2)
   {
       int result = s1->m_strValue > s2->m_strValue;
       Value *v3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(v3);
       return;
   }

   // Otherwise: Type error!
   throw VMException("Can compare only two ints or two strings");
}

void miniScriptVM::Do_geq(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    // Are they two Int's?
   Int *intVal1 = dynamic_cast<Int *>(val1);
   Int *intVal2 = dynamic_cast<Int *>(val2);
   if(intVal1 && intVal2)
   {
       int result = intVal1->m_value >= intVal2->m_value;
       Value *val3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(val3);
       return;
   }

   // Are they two String's?
   String *s1 = dynamic_cast<String *>(val1);
   String *s2 = dynamic_cast<String *>(val2);
   if(s1 && s2)
   {
       int result = s1->m_strValue >= s2->m_strValue;
       Value *v3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(v3);
       return;
   }

   // Otherwise: Type error!
   throw VMException("Can compare only two ints or two strings");
}

void miniScriptVM::Do_leq(Instruction &p_instruction, Frame &p_frame)
{
    Value *val1 = p_frame.m_operandStack.pop();
    Value *val2 = p_frame.m_operandStack.pop();

    // Are they two Int's?
   Int *intVal1 = dynamic_cast<Int *>(val1);
   Int *intVal2 = dynamic_cast<Int *>(val2);
   if(intVal1 && intVal2)
   {
       int result = intVal1->m_value <= intVal2->m_value;
       Value *val3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(val3);
       return;
   }

   // Are they two String's?
   String *s1 = dynamic_cast<String *>(val1);
   String *s2 = dynamic_cast<String *>(val2);
   if(s1 && s2)
   {
       int result = s1->m_strValue <= s2->m_strValue;
       Value *v3 = m_allocator.New_Int(result);
       p_frame.m_operandStack.push(v3);
       return;
   }

   // Otherwise: Type error!
   throw VMException("Can compare only two ints or two strings");
}

void miniScriptVM::Do_jz(Instruction &p_instruction, Frame &p_frame)
{
    Value *test = p_frame.m_operandStack.pop();
    Int *testI = dynamic_cast<Int *>(test);
    if(!testI)
        throw VMException("Excpected boolean on the stack");
    int ii = testI->m_value;
    if(ii == 0)
    {
        p_frame.m_iPointer = p_frame.m_method->m_labels[p_instruction.m_symbol];
    }
}

void miniScriptVM::Do_jnz(Instruction &p_instruction, Frame &p_frame)
{
    Value *test = p_frame.m_operandStack.pop();
      Int *testI = dynamic_cast<Int *>(test);
      if(!testI)
          throw VMException("Excpected boolean on the stack");
      int ii = testI->m_value;
      if(ii != 0)
      {
          p_frame.m_iPointer = p_frame.m_method->m_labels[p_instruction.m_symbol];
      }
}

void miniScriptVM::Do_call(Instruction &p_instruction, Frame &p_frame)
{
    if(m_definedFunctions.contains(p_instruction.m_symbol))
        throw VMException(QString("undefined function: %1").arg(p_instruction.m_symbol));

    Function *func = m_definedFunctions[p_instruction.m_symbol];

    //cout << "Calling: " << i.symbol.toStdString() << " with (";
        // Args are pushed in normal order:
        // the stack is like this:
        // ...., arg1, arg2, arg3
        // and so the following vector will be in reverse order
    QVector<Value *>args;
    for(int i=0; i< func->m_arity; i++)
        {
            Value * arg = p_frame.m_operandStack.pop();
            args.append(arg);
          //  cout << arg->toString().toStdString() <<" ";
        }
    Frame frame2;
    frame2.m_method = func;
    frame2.m_iPointer = 0;

    // Now we push arguments
    // so the stack in the new func is:
    // ..., arg3, arg2, arg1
    // and we can thus pop them in normal order
    for(int i=0; i<func->m_arity; i++)
            frame2.m_operandStack.push(args[i]);
        // and...call the function!
        m_callStack.push(frame2);
}

void miniScriptVM::Do_ret(Instruction &p_instruction, Frame &p_frame)
{
    Value *retVal = p_frame.m_operandStack.top();
    m_callStack.pop();
    if(!m_callStack.empty())
    {
        m_callStack.top().m_operandStack.push(retVal);
    }
}

QVector<Value *>Reverse(QVector<Value *>vect)
{
    QVector<Value *> ret;
    for(int i = vect.count()-1 ; i >= 0; i++)
    {
        ret.append(vect[i]);
    }
    return ret;
}

void miniScriptVM::Do_call_external(Instruction &p_instruction, Frame &p_frame)
{
    if(!m_definedExternalMethods.contains(p_instruction.m_symbol))
        throw VMException(QString("Unknown external method:%1").arg(p_instruction.m_symbol));

    int arity = m_externalMethodArities[p_instruction.m_symbol];
       ExternalMethodType m = m_definedExternalMethods[p_instruction.m_symbol];

       // Args are pushed in normal order:
       // the stack is like this:
       // ...., arg1, arg2, arg3
       // and so the following vector will be in reverse order
       QVector<Value*> args;
       for(int i=0; i< arity; i++)
           args.append(p_frame.m_operandStack.pop());
       // Now make them normal order
       Reverse(args);

       Value *retVal = m(args, m_allocator);
       p_frame.m_operandStack.push(retVal);
}

