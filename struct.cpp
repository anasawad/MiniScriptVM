#include "struct.h"
StructInstance::StructInstance()
{
}

QString StructInstance::ToString()
{
    QStringList str;
    for(QMap<QString, Value *>::const_iterator i = m_fields.begin();
           i!= m_fields.end(); ++i)
    {
        QString fieldVal = QString("%1=%2").arg(i.key())
                      .arg(i.value()->ToString());
              str.append(fieldVal);
    }
    return QString("<a %1; %2>").arg(m_type.m_name).arg(str.join(", "));
}
