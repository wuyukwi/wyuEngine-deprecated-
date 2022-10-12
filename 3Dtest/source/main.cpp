/*********************************************************************************

  *FileName: main.cpp
			コウ  キガク
  *Author:  Huang QiYue
  *Version:  1.0
  *Date:  2022/04/02

**********************************************************************************/

#include "main.h"

gameEngine* g_pEngine = nullptr;

//int WINAPI WinMain(_In_ HINSTANCE instance, _In_opt_ HINSTANCE prev, _In_ LPSTR cmdLine, _In_ int cmdShow)
//{
int main(int argc, char** argv)
{
	char result[MAX_PATH];
	const std::string filename(result, GetModuleFileName(nullptr, result, MAX_PATH));

	std::cout << GetPath(filename) << std::endl;

	g_pEngine = new gameEngine();

	SceneManager* scene_manager = new GameManager();

	g_pEngine->InitializeEngine(GetPath(filename), "setting.ini", scene_manager);

	g_pEngine->Run();

	g_pEngine->ShutdownEngine();

	return 0;
}


