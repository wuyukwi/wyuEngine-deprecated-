#pragma once
#include "GfxConfiguration.h"

class ConfigManager
{
public:
    void Initialize(const std::string& config_file_path, const std::string& config_file_name, GfxConfiguration* config);
    std::string GetRootFolder();
    std::string GetAssetFolder();
    std::string GetTextureFolder();
    std::string GetSoundFolder();
    std::string GetFontFolder();
    std::string GetFileFullPath(const std::string& name) const;
    std::string GetTextureFullPath(const std::string& name) const;


private:
    std::string m_binary_root_folder;
    std::string m_asset_folder;
    std::string m_texture_folder;
    std::string m_sound_folder;
    std::string m_font_folder;
};
