#include "Settings.h"

namespace BruteForce
{
    void Settings::SetExecuteDir(const std::filesystem::path& path)
    {
        mExecuteDir = path;
        mContentDir = mExecuteDir;
        const std::filesystem::path content_dir{ "..\\data\\" };
        mContentDir += content_dir;
    }

    const wchar_t* Settings::GetExecuteDirWchar()
    {
        return mExecuteDir.c_str();
    }

	const wchar_t* Settings::GetContentDirWchar()
	{
		return mContentDir.c_str();
	}

    static Settings gSettings;

    Settings& GetSettings()
    {
        return gSettings;
    }
}
