#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "value.h"
#include"structInfo.h"
#include "frame.h"
#include <QSet>
#include <QStack>

const int MEGA_BYTE               = 1024*1024;
const double HEAP_GROWTH_FACTOR   = 2.0;
const double MIN_HEAP_UTILIZATION = 0.35;
const int MIN_HEAP                = 1* MEGA_BYTE;

class Allocator
{
public:
    Allocator();
    Allocator(QStack<Frame> &p_callStack);
    Value *New_Int(int p_intValue);
    Value *New_String(QString p_stringValue);
    Value *New_structInstacne(StructInfo p_srtuctInstance);
    Value *Null();

private:
    void Check_Heap_Size(int p_newObjectSize);
    void Garbage_Collector();
    void Mark();
    void Sweep();

private:
    QSet<Value*>   m_heap;
    int            m_currentHeapSize;
    int            m_maxHeapSize;
    QStack<Frame>& m_callStack;


};

#endif // ALLOCATOR_H
