#ifndef GAME_INPUTS_H
#define GAME_INPUTS_H
#include <map>
namespace GameInputs
{
    std::pair<float,float> ProcessInputsMoveControl(std::map <int, bool> &inputs);
}


#endif