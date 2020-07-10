#ifndef INPUT_ABSTRACTS_H
#define INPUT_ABSTRACTS_H
#include <string>
namespace Inputs
{
    enum class InputCommands {None, Left, Right, Up, Down, Action, Strike};

    enum class InputCommands CommandFromString(const std::string& value);

}



#endif
