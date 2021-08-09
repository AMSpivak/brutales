#ifndef BRAIN_H
#define BRAIN_H
#include <memory>
#include <iostream>
#include <sstream>
#include <vector>
#include <functional>
class GlCharacter;

namespace Character
{
    enum class BrainTypes {Empty, Hero, Npc, Horde};

    class IBrain
    {
        public:
        virtual void Think(GlCharacter * character) {}
        virtual ~IBrain(){}
        bool LoadBrain(const std::string &filename);
        virtual bool LoadBrain(std::istream &is) = 0;
        virtual void UpdateFromLines(std::vector<std::string> &lines) = 0;
        protected:
        std::function<void(GlCharacter & character)> m_world_reaction;
    };
    

    std::shared_ptr<IBrain> CreateBrain(BrainTypes brain_type, std::function<void(GlCharacter & character)> world_reaction);
}
#endif