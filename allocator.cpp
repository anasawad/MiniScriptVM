#include "allocator.h"
#include "struct.h"


#include "value.h"
Allocator::Allocator(QStack<Frame> &p_callStack):m_callStack(p_callStack)
{
    m_currentHeapSize = 0;
    m_maxHeapSize = 1*MEGA_BYTE;
}

Value *Allocator::New_Int(int p_intValue)
{
    Check_Heap_Size(sizeof(Int));
    Value *val = new Int(p_intValue);
    m_heap.insert(val);
    return val;
}

Value *Allocator::New_String(QString p_stringValue)
{
    Check_Heap_Size(sizeof(String));
    Value *val = new String(p_stringValue);
    m_heap.insert(val);
    return val;
}

Value *Allocator::New_structInstacne(StructInfo p_srtuctInstance)
{

    Check_Heap_Size(sizeof(StructInstance));

    StructInstance *struct_instance = new StructInstance();
    struct_instance->m_type = p_srtuctInstance;
    for(int i =0 ; i < p_srtuctInstance.m_fieldNames.count();i++)
    {
        QString field_name = p_srtuctInstance.m_fieldNames[i];
        struct_instance->m_fields[field_name] = this->Null();
    }
    m_heap.insert(struct_instance);
    return struct_instance;
}

Value *Allocator::Null()
{
    return Null::m_nullVal;
}

void Allocator::Check_Heap_Size(int p_newObjectSize)
{
    if((m_currentHeapSize + p_newObjectSize) > m_maxHeapSize)
    {
        // first try to free memory by collecting garbage
        Garbage_Collector();

        if(m_currentHeapSize + p_newObjectSize > m_maxHeapSize)
        {
            // if we still need memory, expand the heap
            m_maxHeapSize *= HEAP_GROWTH_FACTOR;
        }
    }
    else
    {
        // ok, the current maxHeap allows allocating the object
                // but we'll check utilization; if it's too low we'll shrink the heap
        double utilization = m_currentHeapSize/m_maxHeapSize;
        if(utilization < MIN_HEAP_UTILIZATION)
        {
            m_maxHeapSize /= HEAP_GROWTH_FACTOR;
            if(m_maxHeapSize < MIN_HEAP)
                m_maxHeapSize = MIN_HEAP;
        }
    }
    m_currentHeapSize += p_newObjectSize;
}

void Allocator::Garbage_Collector()
{
    Mark();
    Sweep();
}

void Allocator::Mark()
{
    m_currentHeapSize = 0;
    QStack<Value*> reachable;

    for(int i =0 ; i < m_callStack.count();++i)
    {
       Frame &myFrame = m_callStack[i];
        // Add all locals to reachable
        for(QMap<QString, Value *>::const_iterator it = myFrame.m_localVariables.begin();
                  it != myFrame.m_localVariables.end(); ++it)
        {
            reachable.push(it.value());
        }

        // ...and add all values on operand stack
        for(int j = 0; j<myFrame.m_operandStack.count(); j++)
        {
            reachable.push(myFrame.m_operandStack[j]);
        }

    }

    while(!reachable.empty())
    {
        Value* val = reachable.pop();
        if(val->m_mark)
            continue;
        val->m_mark = 1;
        m_currentHeapSize += val->Size_In_Bytes();
        StructInstance *struct_instance = dynamic_cast<StructInstance*>(val);

        if(struct_instance)
        {
            //add fields to rachable
            for(QMap<QString, Value*>::const_iterator it = struct_instance->m_fields.begin(); it != struct_instance->m_fields.end();++it)
            {
                reachable.push(it.value());
            }
        }
    }
}

void Allocator::Sweep()
{
    QVector<Value*> toDelete;
    QSet<Value*>::const_iterator it;

    for(it = m_heap.begin(); it != m_heap.end();++it)
    {
        Value* val = *it;
        if(val->m_mark)
            val->m_mark = 0;
        else
            toDelete.append(val);
    }
    for(int i=0; i < toDelete.count();i++)
    {
        Value* val = toDelete[i];
        m_heap.remove(val);
        delete val;
    }
}


