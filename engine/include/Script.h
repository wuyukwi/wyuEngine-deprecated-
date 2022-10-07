/*********************************************************************************

  *FileName: Script.h
            コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#pragma once

#define MAX_COMMAND_SIZE   32
#define MAX_LINE_SIZE      3072
#define MAX_PARAM_SIZE     2048

#include  <string>
#include <fstream>

class ScriptLoader
{
public:
    ScriptLoader();
    ~ScriptLoader();

    enum ScriptOpenMode
    {
        MODE_IN = 1,
        MODE_OUT = MODE_IN << 1,
        MODE_ATE = MODE_IN << 2,//ファイル開く時最初の位置になる
        MODE_APP = MODE_IN << 3,//追加モードで開く
        MODE_TRUNC = MODE_IN << 4,//ファイル既にある場合はファイルをクリアする
        MODE_BINARY = MODE_IN << 5,
        MODE_NOCREATE = MODE_IN << 6,
        MODE_NOREPLACE = MODE_IN << 7,

        MODE_SCRIPT = MODE_IN | MODE_NOCREATE
    };

    bool LoadScriptFile(const char* filename);
    bool LoadScriptFile(const std::string& filename, ScriptOpenMode open_mode = MODE_SCRIPT);

    void ParseCommand(char* destCommand);
    std::string& ParseCommand(const std::string& Command);
    std::string& ParseCommand(std::string Command);
    void ParseStringParam(char* destString);
    bool ParseBoolParam();
    int ParseIntParam();
    float ParseFloatParam();
    void ParseSoundFileParan(char* destString, int* repeats);
    void Shutdown();

    void MoveToStart();
    void MoveToNextLine();
    int GetCurrentLineNum();
    int GetTotalLines();
    bool IsLineComment();

    bool SaveScript(const char* filename, const char** data, int currentLine, int currentLineChar);

private:
    std::string m_data;
    int totalScriptLines;
    int currentLine;
    int currentLineChar;
    char** m_script;
};

