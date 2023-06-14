#ifndef _BRUTEFORCE_SETTINGS_H
#define _BRUTEFORCE_SETTINGS_H
#include <filesystem>
namespace BruteForce
{
    class Settings
    {
    private:
		std::filesystem::path mExecuteDir;
		std::filesystem::path mContentDir;
    public:
        void SetExecuteDir(const std::filesystem::path &path);
        const wchar_t* GetExecuteDirWchar();
        const wchar_t* GetContentDirWchar();
    };

    Settings& GetSettings();
}
#endif

