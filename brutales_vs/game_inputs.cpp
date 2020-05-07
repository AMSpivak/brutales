#include <GL/glfw3.h>
#include "game_inputs.h"

namespace GameInputs
{
    std::pair<float,float> ProcessInputsMoveControl(std::map <int, bool> &inputs)
    {
        //int joy_axes_count;
        //const float* joy_axes = glfwGetJoystickAxes(GLFW_JOYSTICK_1, &joy_axes_count);  
        float x = 0;
        float z = 0;

        GLFWgamepadstate state;
        if (glfwJoystickIsGamepad(GLFW_JOYSTICK_1)&&glfwGetGamepadState(GLFW_JOYSTICK_1, &state))
        {
            x = -state.axes[GLFW_GAMEPAD_AXIS_LEFT_X];
            z = -state.axes[GLFW_GAMEPAD_AXIS_LEFT_Y];
            // Use as gamepad
        }
        else
        {
            if(inputs[GLFW_KEY_UP])
            {
                z=1.0f;
            }
            else
            if(inputs[GLFW_KEY_DOWN])
            {
                z=-1.0f;                            
            }

            if(inputs[GLFW_KEY_LEFT])
            {
                x=1.0f;
            }
            else
            if(inputs[GLFW_KEY_RIGHT])
            {
                x=-1.0f;                          
            }
        }
        return std::make_pair(x,z); 
    }
}