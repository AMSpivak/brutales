#include "input_abstracts.h"

namespace Inputs
{
    enum class InputCommands CommandFromString(const std::string & value)
    {
        if (value == "left")
        {
            return  InputCommands::Left;
        }
        if (value == "down")
        {
            return  InputCommands::Down;
        }
        if (value == "up")
        {
            return  InputCommands::Up;
        }
        if (value == "right")
        {
            return  InputCommands::Right;
        }
        if (value == "action")
        {
            return  InputCommands::Action;
        }
        if (value == "strike")
        {
            return  InputCommands::Strike;
        }
        return InputCommands::None;
    }

}

