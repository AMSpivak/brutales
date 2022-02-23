#include "Settings.h"

namespace BruteForce
{
    void Settings::SetExecuteDir(const std::filesystem::path& path)
    {
        mExecuteDir = path;
    }
    const wchar_t* Settings::GetExecuteDirWchar()
    {
        return mExecuteDir.c_str();
    }

    static Settings gSettings;

    Settings& GetSettings()
    {
        return gSettings;
    }
}
