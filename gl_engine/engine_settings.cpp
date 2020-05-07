#include <stdexcept>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "engine_settings.h"
#include "loader.h"
namespace EngineSettings
{
    Settings::Settings()
    {
        std::ifstream loadfile;
        loadfile.open ("engine.cfg");
        if(loadfile.is_open())
            LoadSettings(loadfile);
        loadfile.close();
    }

    Settings::~Settings()
    {
        std::ofstream savefile;
        savefile.open ("engine.cfg");
        SaveSettings(savefile);
        savefile.close();
    }

    bool Settings::IsPbrON()
    {
        return pbr_light;
    }
    void Settings::SetPbr(bool value)
    {
        pbr_light = value;
    }

    void Settings::SetQualityFactor(float value)
    {
        m_quality_factor = value;
    }
    float Settings::GetQualityFactor()
    {
        return m_quality_factor;
    }

    void Settings::BeginNewFrame()
    {
        ++frame_number;
    }

    void Settings::SaveSettings(std::ostream &os)
    {
        os<<"pbr_light "<<pbr_light<<"\n"
        <<"quality_factor "<<m_quality_factor<<"\n"
        <<"monitor "<<monitor<<"\n";
    }
    void Settings::LoadSettings(std::istream& is)
    {
        std::vector<std::string> lines;
        std::string tempholder("");
        while(!is.eof())
        {
            getline(is, tempholder);
            lines.emplace_back(tempholder);
        }

        LoaderUtility::LinesProcessor proc;

        proc.Add("pbr_light",[this](std::stringstream &sstream){SetPbr(LoaderUtility::GetFromStream<bool>(sstream));});
        proc.Add("quality_factor",[this](std::stringstream &sstream){SetQualityFactor(LoaderUtility::GetFromStream<float>(sstream));});
        proc.Add("monitor",[this](std::stringstream &sstream){monitor = (LoaderUtility::GetFromStream<int>(sstream));});
        proc.Process(lines);
    }
    int Settings::GetMonitorIndex()
    {
        return monitor;
    }

    float Settings::GetFPS()
    {
        return m_fps;
    }

    void Settings::SetFPS(float value)
    {
        m_fps = value;
    }

    unsigned short Settings::GetFrame(){return frame_number;}

    static Settings * main_settings = nullptr;

    Settings * GetEngineSettings()
    {
        if(main_settings == nullptr)
        {
            throw std::out_of_range("No recource manager defined!");
        }
        else
        return main_settings;
    }

    void SetEngineSettings(Settings * settings)
    {
        main_settings = settings;
    }
}

