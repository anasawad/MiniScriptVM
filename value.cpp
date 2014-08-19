#include "value.h"
Null *Null::m_nullVal = new Null();


Value::Value()
{
    m_mark = 0;
}
