#include "game_status.h"
namespace GameSettings
{
    const float HeroStatus::GetLife() const
    {
        return m_life;
    }

    void HeroStatus::SetLife(float value)
    {
        m_life = value;
    }

    const std::string & HeroStatus::GetLevel() const
    {
        return m_level;
    }
    void HeroStatus::SetLevel(const std::string &value)
    {
        m_level = value;
    }

    const std::string &HeroStatus::GetSave() const
    {
        return m_save;
    }
    void HeroStatus::SetSave(const std::string &value)
    {
        m_save = value;
    } 

    HeroStatus * pStatus = nullptr;

    HeroStatus * GetHeroStatus()
    {
        return pStatus;
    }

    void SetHeroStatus(HeroStatus * value)
    {
        pStatus = value;
    }     
}