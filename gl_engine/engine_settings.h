#ifndef ENGINE_SETTINGS
#define ENGINE_SETTINGS

namespace EngineSettings
{

class Settings
{
    private:
    bool pbr_light = false;
    unsigned short frame_number = 0;
    float m_quality_factor = 1.0;
    float m_fps = 0;
    int monitor = 0;
    public:
    Settings();
    ~Settings();
    void LoadSettings(std::istream& is);
    void SaveSettings(std::ostream& os);

    void SetQualityFactor(float value);
    float GetQualityFactor();
    float GetFPS();
    int GetMonitorIndex();
    void SetFPS(float value);
    bool IsPbrON();
    void SetPbr(bool value);
    void BeginNewFrame();
    unsigned short GetFrame();
};



Settings * GetEngineSettings();
void SetEngineSettings(Settings * settings);



}

#endif
