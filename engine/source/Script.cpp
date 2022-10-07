/*********************************************************************************

  *FileName: Script.cpp
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/11

**********************************************************************************/

#include"Script.h"

using namespace std;

ScriptLoader::ScriptLoader() : totalScriptLines(0), currentLine(0), currentLineChar(0), m_script(0)
{
}

ScriptLoader::~ScriptLoader()
{
    Shutdown();
}

bool ScriptLoader::LoadScriptFile(const char* filename)
{
    ifstream input, input2;
    char tempLine[MAX_LINE_SIZE];

    input.open(filename);
    if (!input.is_open())
        return false;

    Shutdown();

    // テキストファイルを開く行ごと取得する
    while (!input.eof())
    {
        input.getline(tempLine, MAX_LINE_SIZE, '\n');
        totalScriptLines++;
    }

    input.close();

    input2.open(filename);
    if (!input2.is_open())
        return false;

    // テキストのすべての行をロードする
    m_script = new char* [totalScriptLines];

    for (int i = 0; i < totalScriptLines; i++)
    {
        m_script[i] = new char[MAX_LINE_SIZE + 1];
        input2.getline(m_script[i], MAX_LINE_SIZE, '\n');
    }

    input2.close();
    return true;
}

bool ScriptLoader::LoadScriptFile(const std::string& filename, ScriptOpenMode open_mode)
{

    return true;
}


void ScriptLoader::ParseCommand(char* destCommand)
{
    //この関数は、現在の行の最初の単語を取得し、それをdestCommandに保存します。

    int commandSize = 0;

    // destcommandがNULLの場合、行がオーバーしている場合、または現在の行の終わりにある場合は、戻ります。
    if (!destCommand)
        return;
    if (currentLine >= totalScriptLines)
        return;
    /*if(currentLineChar >= (int)strlen(m_script[currentLine]))
        return;*/

    destCommand[0] = '\0';

    // 行の先頭に＃がある場合、これはコメントである
    if (IsLineComment())
    {
        destCommand[0] = '#';
        destCommand[1] = '\0';
        return;
    }
    // 行の先が改行の場合
    if (m_script[currentLine][0] == '\0')
    {
        return;
    }

    // スペースまたは改行が見つかるまですべての文字を読み込みます。
    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] == ' ' ||
            m_script[currentLine][currentLineChar] == '\n')
            break;

        // 配列にテキストデータを保存します
        destCommand[commandSize] = m_script[currentLine][currentLineChar];
        commandSize++;
        currentLineChar++;
    }

    // 次のデータまでスキップします。
    currentLineChar++;

    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] != ' ')
        {
            break;
        }

        currentLineChar++;
    }
    destCommand[commandSize] = '\0';
}

std::string& ScriptLoader::ParseCommand(const std::string& Command)
{
    string out;
    return out;
}

std::string& ScriptLoader::ParseCommand(std::string Command)
{
    string out;
    return out;
}

void ScriptLoader::ParseStringParam(char* destString)
{
    // この関数は“ ”の中にテキストを取り、それをdestStringに保存します。

    int paramSize = 0;
    bool beginQuoteFound = false;
    bool endQuoteFound = false;

    // destcommandがNULLの場合、行がオーバーしている場合、または現在の行の終わりにある場合は、戻ります。
    if (!destString)
        return;
    if (currentLine >= totalScriptLines)
        return;
    if (currentLineChar >= (int)strlen(m_script[currentLine]))
        return;

    // 文字列配列を初期化します
    destString[0] = '\0';

    // 最初の"をスキップします。
    currentLineChar++;

    // "または改行が見つかるまですべての文字を読み込みます。
    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] == '"')
        {
            endQuoteFound = true;
            break;
        }

        if (m_script[currentLine][currentLineChar] == '\n')
        {
            break;
        }

        // テキストデータを保存
        destString[paramSize] = m_script[currentLine][currentLineChar];
        paramSize++;
        currentLineChar++;
    }

    // 終了引用符と次のスペースまたは改行をスキップします。文字列が複数行を占めることは許可されていません。
    if (endQuoteFound)
        currentLineChar += 2;
    else
        currentLineChar++;

    destString[paramSize] = '\0';
}

bool ScriptLoader::ParseBoolParam()
{
    // この関数は次のテキストを取得し、このテキストが「true」の場合はtrueを返し、「false」の場合はfalseを返します。

    char string[MAX_PARAM_SIZE];
    int paramSize = 0;

    // 行が足りなくなった場合、または現在の行の終わりにある場合は、戻ります。
    if (currentLine >= totalScriptLines)
        return false;
    if (currentLineChar >= (int)strlen(m_script[currentLine]))
        return false;

    // スペースまたは改行が見つかるまで、すべての文字を読み込みます
    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] == ' ' ||
            m_script[currentLine][currentLineChar] == '\n')
        {
            break;
        }

        // データを保存
        string[paramSize] = m_script[currentLine][currentLineChar];
        paramSize++;
        currentLineChar++;
    }

    // 次のスペースまたは改行をスキップします。
    currentLineChar++;
    string[paramSize] = '\0';

    if (_stricmp(string, "true") == 0)
        return true;

    return false;
}

int ScriptLoader::ParseIntParam()
{
    // この関数は次のテキストを受け取り、それをintに変換します。

    char string[MAX_PARAM_SIZE];
    int paramSize = 0;

    // 行が足りなくなった場合、または現在の行の終わりにある場合は、戻ります。
    if (currentLine >= totalScriptLines)
        return false;
    if (currentLineChar >= (int)strlen(m_script[currentLine]))
        return false;

    // スペースまたは改行が見つかるまで、すべての文字を読み込みます
    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] == ' ' ||
            m_script[currentLine][currentLineChar] == '\n')
            break;

        // データを保存
        string[paramSize] = m_script[currentLine][currentLineChar];
        paramSize++;
        currentLineChar++;
    }

    // 次のスペースまたは改行をスキップします。
    currentLineChar++;
    string[paramSize] = '\0';
    return atoi(string);
}

float ScriptLoader::ParseFloatParam()
{
    // この関数は次のテキストを受け取り、それをfloatに変換します。

    char string[MAX_PARAM_SIZE];
    int paramSize = 0;

    // 行が足りなくなった場合、または現在の行の終わりにある場合は、戻ります。
    if (currentLine >= totalScriptLines)
        return false;
    if (currentLineChar >= (int)strlen(m_script[currentLine]))
        return false;

    // スペースまたは改行が見つかるまで、すべての文字を読み込みます
    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] == ' ' ||
            m_script[currentLine][currentLineChar] == '\n')
            break;

        // データを保存
        string[paramSize] = m_script[currentLine][currentLineChar];
        paramSize++;
        currentLineChar++;
    }

    // 次のスペースまたは改行をスキップします。
    currentLineChar++;
    string[paramSize] = '\0';
    return (float)atof(string);
}

void ScriptLoader::ParseSoundFileParan(char* destString, int* repeats)
{
    MoveToNextLine();

    // この関数は“ ”の中にテキストを取り、それをdestStringに保存します。
    char stringInt[MAX_PARAM_SIZE];
    int paramSizeInt = 0;

    int paramSize = 0;
    bool beginQuoteFound = false;
    bool endQuoteFound = false;

    // destcommandがNULLの場合、行がオーバーしている場合、または現在の行の終わりにある場合は、戻ります。
    if (!destString || !repeats)
        return;
    if (currentLine >= totalScriptLines)
        return;
    if (currentLineChar >= (int)strlen(m_script[currentLine]))
        return;

    // 文字列配列を初期化します
    destString[0] = '\0';

    // 最初の"をスキップします。
    currentLineChar++;

    // "または改行が見つかるまですべての文字を読み込みます。
    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        if (m_script[currentLine][currentLineChar] == '"')
        {
            endQuoteFound = true;

            break;
        }

        if (m_script[currentLine][currentLineChar] == '\n')
        {
            break;
        }

        // テキストデータを保存
        destString[paramSize] = m_script[currentLine][currentLineChar];
        paramSize++;
        currentLineChar++;
    }
    // 終了引用符と次のスペースまたは改行をスキップします。文字列が複数行を占めることは許可されていません。
    if (endQuoteFound)
        currentLineChar += 2;
    else
        currentLineChar++;
    destString[paramSize] = '\0';

    while (currentLineChar < (int)strlen(m_script[currentLine]))
    {
        // 変数を読み込む
        if (m_script[currentLine][currentLineChar] != ' ')
        {
            // データを保存
            stringInt[paramSizeInt] = m_script[currentLine][currentLineChar];
            paramSizeInt++;
            currentLineChar++;
        }
        else
        {
            break;
        }
    }

    stringInt[paramSize] = '\0';
    *repeats = atoi(stringInt);
}

void ScriptLoader::MoveToStart()
{
    currentLine = 0;
    currentLineChar = 0;
}

void ScriptLoader::MoveToNextLine()
{
    currentLine++;
    currentLineChar = 0;
}

int ScriptLoader::GetCurrentLineNum()
{
    return currentLine;
}

int ScriptLoader::GetTotalLines()
{
    return totalScriptLines;
}

bool ScriptLoader::IsLineComment()
{
    // 行の最初の文字が＃の場合、その行はコメントアウトする必要があります。
    if (m_script[currentLine][0] == '#')
        return true;

    return false;
}

void ScriptLoader::Shutdown()
{
    if (m_script)
    {
        // スクリプトファイルのすべてのデータを削除します。
        for (int i = 0; i < totalScriptLines; i++)
        {
            if (m_script[i])
            {
                delete[] m_script[i];
                m_script[i] = 0;
            }
        }

        delete m_script;
        m_script = 0;
    }

    // 変数を初期化します
    totalScriptLines = 0;
    currentLineChar = 0;
    currentLine = 0;
}

//-----------------------------------------------------------------------------
// スクリプトファイルを保存
//-----------------------------------------------------------------------------
bool ScriptLoader::SaveScript(const char* filename, const char** data, int currentLine, int currentLineChar)
{
    //std::ofstream output, output2;
    //char tempLine[MAX_LINE_SIZE];

    //output.open(filename, std::ios::trunc);
    //if (!output.is_open())
    //    return false;

    //Shutdown();

    //for (int i = 0; i < currentLine; i++)
    //{
    //    output << data[i][0];
    //    output.write
    //}

    //while (!output.eof())
    //{
    //

    //    output.getline(tempLine, MAX_LINE_SIZE, '\n');
    //    totalScriptLines++;
    //}

    //output.close();

    //output2.open(filename);
    //if (!output2.is_open())
    //    return false;

    //// テキストのすべての行をロードする
    //m_script = new char* [totalScriptLines];

    //for (int i = 0; i < totalScriptLines; i++)
    //{
    //    m_script[i] = new char[MAX_LINE_SIZE + 1];
    //    output2.getline(m_script[i], MAX_LINE_SIZE, '\n');
    //}

    //output2.close();
    //return true;
    //FILE* file = NULL;
    //char output[260];

    //// ファイル名ない場合は現有のファイルをあく
    //if (filename != NULL)
    //{
    //    if ((file = fopen(filename, "w")) == NULL)
    //        return;
    //}
    //else
    //{
    //    if ((file = fopen(GetFilename(), "w")) == NULL)
    //        return;
    //}

    //// #begin を書き込み
    //fputs("#begin\n", file);

    //// 全ての変数を書き込み
    //m_variables->Iterate(true);
    //while (m_variables->Iterate() != NULL)
    //{
    //    switch (m_variables->GetCurrent()->GetType())
    //    {
    //    case VARIABLE_BOOL:
    //        if (*((bool*)m_variables->GetCurrent()->GetData()) == true)
    //            sprintf(output, "%s bool true", m_variables->GetCurrent()->GetName());
    //        else
    //            sprintf(output, "%s bool false", m_variables->GetCurrent()->GetName());
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;

    //    case VARIABLE_COLOUR:
    //        sprintf(output, "%s colour %f %f %f %f", m_variables->GetCurrent()->GetName(), ((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->r, ((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->g, ((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->b, ((D3DCOLORVALUE*)m_variables->GetCurrent()->GetData())->a);
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;

    //    case VARIABLE_FLOAT:
    //        sprintf(output, "%s float %f", m_variables->GetCurrent()->GetName(), *(float*)m_variables->GetCurrent()->GetData());
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;

    //    case VARIABLE_NUMBER:
    //        sprintf(output, "%s number %d", m_variables->GetCurrent()->GetName(), *(long*)m_variables->GetCurrent()->GetData());
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;

    //    case VARIABLE_STRING:
    //        sprintf(output, "%s string \"%s\"", m_variables->GetCurrent()->GetName(), (char*)m_variables->GetCurrent()->GetData());
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;

    //    case VARIABLE_VECTOR:
    //        sprintf(output, "%s vector %f %f %f", m_variables->GetCurrent()->GetName(), ((D3DXVECTOR3*)m_variables->GetCurrent()->GetData())->x, ((D3DXVECTOR3*)m_variables->GetCurrent()->GetData())->y, ((D3DXVECTOR3*)m_variables->GetCurrent()->GetData())->z);
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;

    //    default:
    //        sprintf(output, "%s unknown %s", m_variables->GetCurrent()->GetName(), (char*)m_variables->GetCurrent()->GetData());
    //        fputs(output, file);
    //        fputs("\n", file);
    //        continue;
    //    }
    //}

    //// #end を書き込み
    //fputs("#end", file);

    //// ファイルを閉じる
    //fclose(file);

    return true;
}