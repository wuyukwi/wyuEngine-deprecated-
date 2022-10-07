/*********************************************************************************

  *FileName: PropertyScript.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/11

**********************************************************************************/

#include"PropertyScript.h"
#include<fstream>

using std::ifstream;

int DetermineType(int startIndex, char* buffer)
{
    int numComponents = 0;
    int type = NULL_VAR;
    bool decimalFound = false;
    bool charFound = false;
    int index = startIndex;

    // ループして、文字列を取得します。
    while (index < (int)strlen(buffer))
    {
        // 新しい行がないため、このループ内に入ると、これを自動的に追加します。
        if (numComponents == 0)
            numComponents++;

        if (buffer[index] == ' ')
            numComponents++;

        if (buffer[index] == '.')
            decimalFound = true;

        if ((buffer[index] >= 'a' && buffer[index] <= 'z') ||
            (buffer[index] >= 'A' && buffer[index] <= 'Z') ||
            buffer[index] == '_')
            charFound = true;

        index++;
    }

    // 名前の後に変数が1つだけ表示されている場合、vectorは3である必要があるため、vector以外の任意のタイプにすることができます。
    switch (numComponents)
    {
    case 1:
        // 文字がある場合、それは文字列です。
        if (charFound)
            type = STRING_VAR;
        else
            type = INT_VAR;

        // 小数があり、文字がない場合は、floatになります。
        if (decimalFound == true && charFound == false)
            type = FLOAT_VAR;
        break;

    case 3:
        // ベクトルは単なるfloatであるため、グループ内に文字が含まれている場合は、文字列である。
        if (charFound)
            type = STRING_VAR;
        else
            type = VECTOR_VAR;
        break;

    default:
        // 名前の後に複数の単語がある場合、文字列にはスペースを含めることができるため、文字列である。
        if (numComponents > 0)
            type = STRING_VAR;
        break;
    }

    return type;
}

void CVariable::SetData(int t, char* n, void* data)
{
    //if (name!=nullptr) return;

    // 変数名を設定してから、タイプとデータを設定します。
    memcpy(name, n, strlen(n));
    name[strlen(n)] = '\0';
    SetData(t, data);
}

void CVariable::SetData(int t, void* data)
{
    stVector* vec = NULL;
    int len = 0;

    // タイプに応じて、値が格納されている場所によって異なります。
    switch (t)
    {
    case INT_VAR:
        intVal = (int)data;
        break;

    case FLOAT_VAR:
        floatVal = *(float*)data;
        break;

    case STRING_VAR:
        len = strlen((char*)data);
        stringVal = new char[len + 1];
        memcpy(stringVal, (char*)data, len);
        stringVal[len] = '\0';
        break;

    case VECTOR_VAR:
        vec = (stVector*)data;
        vecVal.x = vec->x;
        vecVal.y = vec->y;
        vecVal.z = vec->z;
        break;

    default:
        // ここに到達すると、それはNULL変数です。
        return;
        break;
    };

    type = t;
}

CPropertyScript::CPropertyScript() : variableList(NULL),
m_totalVars(0), currentLineChar(0)
{
}

CPropertyScript::~CPropertyScript()
{
    // すべてのリソースを解放します。
    Shutdown();
}

bool CPropertyScript::IncreaseVariableList()
{
    if (!variableList)
    {
        variableList = new CVariable[1];

        if (!variableList)
            return false;
    }
    else
    {
        CVariable* temp;
        temp = new CVariable[m_totalVars + 1];
        if (!temp)
            return false;

        memcpy(temp, variableList,
            sizeof(CVariable) * m_totalVars);

        delete[] variableList;
        variableList = temp;
    }

    return true;
}

bool CPropertyScript::LoadScriptFile(char* filename)
{
    int totalScriptLines = 0;
    ifstream input, input2;
    char tempLine[256];
    char varName[128];
    char param[3072];
    int type = 0;

    // ファイルを開いて、そこから行数を取得します。
    input.open(filename);
    if (!input.is_open())
        return false;

    // 以前のデータをすべてクリアします。
    Shutdown();

    // 行数を取得します。
    while (!input.eof())
    {
        input.getline(tempLine, 256, '\n');
        totalScriptLines++;
    }

    input.close();

    // 今回は変数を取り出します。
    input2.open(filename);
    if (!input2.is_open())
        return false;

    // スクリプトの各行をループして、すべての変数を取得します。
    for (int i = 0; i < totalScriptLines; i++)
    {
        // ラインカウンターを最初にリセットします。
        currentLineChar = 0;

        // ファイルから1行を読み取ります。
        input2.getline(tempLine, 256, '\n');
        tempLine[strlen(tempLine)] = '\0';

        // これがコメントかどうかを確認する、ない場合継続します。
        if (tempLine[0] != '#')
        {
            // 名前を読み、タイプを判別します。
            ParseNext(tempLine, varName);
            type = DetermineType(currentLineChar, tempLine);

            // タイプによっては、名前の後に読む必要のある単語の数によって異なります。 intの場合は1、ベクトル3、文字列1などが必要です。
            // データを取得したら、それを必要なタイプに変換し、変数に設定します。
            switch (type)
            {
            case INT_VAR:
                if (IncreaseVariableList())
                {
                    ParseNext(tempLine, param);
                    variableList[m_totalVars].SetData(INT_VAR, varName, (void*)atoi(param));
                    m_totalVars++;
                }
                break;
            case FLOAT_VAR:
                if (IncreaseVariableList())
                {
                    float fVal = 0;
                    ParseNext(tempLine, param);
                    fVal = (float)atof(param);
                    variableList[m_totalVars].SetData(FLOAT_VAR, varName, &fVal);
                    m_totalVars++;
                }
                break;
            case STRING_VAR:
                if (IncreaseVariableList())
                {
                    ParseNext(tempLine, param);
                    variableList[m_totalVars].SetData(STRING_VAR, varName, (void*)param);
                    m_totalVars++;
                }
                break;
            case VECTOR_VAR:
                if (IncreaseVariableList())
                {
                    stVector vecVal;

                    ParseNext(tempLine, param);
                    vecVal.x = (float)atof(param);
                    ParseNext(tempLine, param);
                    vecVal.y = (float)atof(param);
                    ParseNext(tempLine, param);
                    vecVal.z = (float)atof(param);

                    variableList[m_totalVars].SetData(VECTOR_VAR, varName, &vecVal);
                    m_totalVars++;
                }
                break;
            default:
                break;
            }
        }
    }

    // ファイルを閉じます.
    input2.close();
    return true;
}

void CPropertyScript::ParseNext(char* tempLine, char* outData)
{
    int commandSize = 0;
    int paramSize = 0;

    // エラーチェックします。
    if (!tempLine || !outData)
        return;

    // 配列を初期化します。
    outData[0] = '\0';

    // スペースまたは改行が見つかるまで、すべての文字を読み込みます。
    while (currentLineChar < (int)strlen(tempLine))
    {
        if (tempLine[currentLineChar] == ' ' || tempLine[currentLineChar] == '\n')
            break;

        // データを保存
        outData[paramSize] = tempLine[currentLineChar];
        paramSize++;
        currentLineChar++;
    }

    // '\0'出力して、次のスペースまで移動します。スペースがない場合、次の行に移動します。
    outData[paramSize] = '\0';
    currentLineChar++;
}

bool CPropertyScript::AddVariable(char* name, int t, void* val)
{
    // 変数がすでに存在するかどうかを確認します。
    if (!SetVariable(name, t, val))
    {
        if (!IncreaseVariableList())
            return false;

        // 変数データを設定します。
        variableList[m_totalVars].SetData(t, name, val);
        m_totalVars++;
    }

    return true;
}

bool CPropertyScript::SetVariable(char* name, int t, void* val)
{
    // リストをループして名前を比較します。変数が見つかった場合は、そのデータを設定します。
    for (int i = 0; i < m_totalVars; i++)
    {
        if (_stricmp(variableList[i].GetName(), name) == 0)
        {
            variableList[i].SetData(t, val);
            return true;
        }
    }

    return false;
}

int CPropertyScript::GetVariableAsInt(char* name)
{
    // リストをループして名前を比較します。変数が見つかった場合は、そのデータを設定します。
    for (int i = 0; i < m_totalVars; i++)
    {
        if (_stricmp(variableList[i].GetName(), name) == 0)
            return variableList[i].GetDataAsInt();
    }

    return 0;
}

float CPropertyScript::GetVariableAsFloat(char* name)
{
    // リストをループして名前を比較します。変数が見つかった場合は、そのデータを設定します。
    for (int i = 0; i < m_totalVars; i++)
    {
        if (_stricmp(variableList[i].GetName(), name) == 0)
            return variableList[i].GetDataAsFloat();
    }

    return 0;
}

char* CPropertyScript::GetVariableAsString(char* name)
{
    // リストをループして名前を比較します。変数が見つかった場合は、そのデータを設定します。
    for (int i = 0; i < m_totalVars; i++)
    {
        if (_stricmp(variableList[i].GetName(), name) == 0)
            return variableList[i].GetDataAsString();
    }

    return NULL;
}

stVector CPropertyScript::GetVariableAsVector(char* name)
{
    stVector null;

    //リストをループして、名前を比較します。変数が見つかった場合、そのデータを返します。
    for (int i = 0; i < m_totalVars; i++)
    {
        if (_stricmp(variableList[i].GetName(), name) == 0)
            return variableList[i].GetDataAsVector();
    }

    return null;
}

void CPropertyScript::Shutdown()
{
    // リスト削除します。
    if (variableList)
        delete[] variableList;
    variableList = NULL;
}