#include "ConfigManager.h"
#include <fstream>
#include <string>

void ConfigManager::Initialize(const std::string& config_file_path, const std::string& config_file_name, GfxConfiguration* config)
{
    std::ifstream config_file(config_file_path + config_file_name);
    std::string   config_line;

    while (std::getline(config_file, config_line))
    {
        size_t seperate_pos = config_line.find_first_of('=');
        if (seperate_pos > 0 && seperate_pos < (config_line.length() - 1))
        {
            std::string name = config_line.substr(0, seperate_pos);
            std::string value = config_line.substr(seperate_pos + 1, config_line.length() - seperate_pos - 1);
            if (name == "BinaryRootFolder")
            {
                m_binary_root_folder = config_file_path + value;
            }
            else if (name == "AssetFolder")
            {
                m_asset_folder = m_binary_root_folder + value;
            }
            else if (name == "TextureFolder")
            {
                m_texture_folder = m_binary_root_folder + value;
            }
            else if (name == "SoundFolder")
            {
                m_sound_folder = m_binary_root_folder + value;
            }
            else if (name == "FontFolder")
            {
                m_font_folder = m_binary_root_folder + value;
            }
            else if (name == "ApplicationName")
            {
                config->appName = value;
            }
            else if (name == "WindowsWidth")
            {
                config->screenWidth = std::stoi(value);
            }
            else if (name == "WindowsHeight")
            {
                config->screenHeight = std::stoi(value);
            }
            else if (name == "FullScreen")
            {
                config->fullscreen = std::stoi(value);
            }
            else if (name == "MultiSampling")
            {
                config->msaaSamples = std::stoi(value);
            }

        }
    }
}

std::string ConfigManager::GetRootFolder()
{
    return m_binary_root_folder;
}

std::string ConfigManager::GetAssetFolder()
{
    return m_asset_folder;
}

std::string ConfigManager::GetTextureFolder()
{
    return m_texture_folder;
}

std::string ConfigManager::GetSoundFolder()
{
    return m_sound_folder;
}

std::string ConfigManager::GetFontFolder()
{
    return m_font_folder;
}


std::string ConfigManager::GetFileFullPath(const std::string& name) const
{
    return m_asset_folder + name;
}

std::string ConfigManager::GetTextureFullPath(const std::string& name) const
{
    return m_texture_folder + name;
}


