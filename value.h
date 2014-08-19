#ifndef VALUE_H
#define VALUE_H

#include <QString>

class Value
{
public:
    int m_mark;
    Value();
    virtual ~Value(){}
    virtual  QString ToString() =0 ;
    virtual int Size_In_Bytes() =0;
};

class Int: public Value
{
public:
    int m_value;

public:
    Int(int p_value):m_value(p_value){}
    QString ToString()
    {
        return QString("%1").arg(m_value);
    }
    Value* Add(Value *p_value);
    int ComparTo(Value *p_value);
    int Size_In_Bytes(){return sizeof(Int);}
};


class String: public Value
{
public:
    QString m_strValue;
public:
    String(QString p_strVal):m_strValue(p_strVal){}
    QString ToString()
    {return m_strValue;}
    int Size_In_Bytes(){return sizeof(String);}
};

class Null: public Value
{
public:
    static Null *m_nullVal;
    QString ToString(){return "null";}
    int Size_In_Bytes(){return 0;}
};

#endif // VALUE_H
