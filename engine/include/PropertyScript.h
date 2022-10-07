/*********************************************************************************

  *FileName: PropertyScript.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/11

**********************************************************************************/

#ifndef _PROPERTY_SCRIPT_H_
#define _PROPERTY_SCRIPT_H_

#include "structs.h"

enum enVarType
{
    NULL_VAR = 0,
    INT_VAR,
    FLOAT_VAR,
    STRING_VAR,
    VECTOR_VAR
};

class CVariable
{
public:
    CVariable() : type(0), floatVal(0), intVal(0), stringVal(0)
    {
        name[0] = '\0';
        vecVal.x = vecVal.y = vecVal.z = 0;
    }
    ~CVariable()
    {
        if (stringVal)
            delete[] stringVal;
        stringVal = 0;
    }

    void SetData(int t, char* n, void* data);
    void SetData(int t, void* data);

    char* GetName() { return name; }
    int GetType() { return type; }
    int GetDataAsInt() { return intVal; }
    float GetDataAsFloat() { return floatVal; }
    char* GetDataAsString() { return stringVal; }
    stVector GetDataAsVector() { return vecVal; }

private:
    char name[128];
    int type;

    int intVal;
    float floatVal;
    char* stringVal;
    stVector vecVal;
};

class CPropertyScript
{
public:
    CPropertyScript();
    ~CPropertyScript();

    bool IncreaseVariableList();
    bool LoadScriptFile(char* filename);

private:
    void ParseNext(char* tempLine, char* varName);

public:
    bool AddVariable(char* name, int t, void* val);
    bool SetVariable(char* name, int t, void* val);
    int GetVariableAsInt(char* name);
    float GetVariableAsFloat(char* name);
    char* GetVariableAsString(char* name);
    stVector GetVariableAsVector(char* name);

    void Shutdown();

private:
    CVariable* variableList;
    int m_totalVars;
    int currentLineChar;
};

#endif _PROPERTY_SCRIPT_H_