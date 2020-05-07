#ifndef GAME_STATUS
#define GAME_STATUS
#include <string>
namespace GameSettings
{

    class HeroStatus
    {
        private:
        std::string m_save;
        std::string m_level;
        float m_life;
        public:
        HeroStatus():m_life(1.0f),m_save("sav"),m_level("levels/test.lvl"){}
        ~HeroStatus(){}

        const std::string & GetLevel() const;
        void SetLevel(const std::string&);

        const std::string & GetSave() const;
        void SetSave(const std::string&);

        const float GetLife() const;
        void SetLife(float value);       
    };

    HeroStatus * GetHeroStatus();
    void SetHeroStatus(HeroStatus * value);

}



#endif