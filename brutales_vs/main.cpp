
#include <iostream>
#include <sstream>
#include <fstream>

#include <memory>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <map>
#include <tuple>
#include <sound/irrKlang.h>

#include "glm.hpp"
#include "trigonometric.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"

#include "glresourses.h"
#include "gl_resources_manager.h"

#include "glscene.h"
#include "gl_light.h"
#include "gl_render_target.h"
#include "gl_model.h"
#include "gl_character.h"
#include "gl_game_state_dungeon.h"
#include "gl_game_state_menu.h"
#include "animation_sequence.h"
#include "engine_settings.h"
#include "game_status.h"

extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);

GLuint SCR_WIDTH = 800, SCR_HEIGHT = 600;
//GLuint SCR_WIDTH = 1200, SCR_HEIGHT =1000;

float key_angle = 0.0f;

// float clamp(float value, float min, float max)
// {
// 	return 
// }

std::map <int, bool> inputs;

void SetRenderTargets(
					std::map<std::string,std::shared_ptr<glRenderTargetSimple>> &render_target_map,
					unsigned int width, unsigned int height)
{
	render_target_map.clear();
	EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
	float quality = settings->GetQualityFactor();

	std::cout << "Set RT quality "<<quality<<"\n";
	auto base = std::make_pair("base_deffered",std::make_shared<glRenderTargetDeffered>());
	render_target_map.insert( base);
	base.second->InitBuffer(width, height,quality);
	auto fin = std::make_pair("final",std::make_shared<glRenderTarget>());
	render_target_map.insert( fin);
	fin.second->InitBuffer(width, height,quality);
	auto post = std::make_pair("postprocess",std::make_shared<glRenderTarget>());
	render_target_map.insert( post);
	post.second->InitBuffer(width, height,quality);

	auto buff1 = std::make_pair("buffer_1",std::make_shared<glRenderTargetSimple>());
	render_target_map.insert( buff1);
	buff1.second->InitBuffer(width/2, height/2,quality);

	auto buff2 = std::make_pair("buffer_2",std::make_shared<glRenderTargetSimple>());
	render_target_map.insert(buff2);
	buff2.second->InitBuffer(width/2, height/2,quality);
}

void FillShaders(std::map<const std::string,GLuint> &shader_map, const std::string filename)
{
	
	std::ifstream shaders_file;
	shaders_file.open(filename);
    if(shaders_file.is_open())
    {
        while(!shaders_file.eof())
        {

            std::string line;
            getline(shaders_file, line);
			{
				std::stringstream ss(line);
				std::string name;
				std::string vs;
				std::string fs;
				ss >> name >> vs >> fs;
				
				shader_map.insert ( std::pair<const std::string,GLuint>(name, LoadshaderProgram(vs,fs)));
			}
        }
        shaders_file.close();
    }
}

void GlInfo(std::ostream & ostream)
{
	ostream << "=============================\n";
	ostream << "OpenGl Info:\n";
	ostream << "=============================\n";
	ostream << glGetString(GL_VENDOR) << "\n";
	ostream << glGetString(GL_RENDERER) << "\n";
	ostream << glGetString(GL_VERSION) << "\n";
	ostream << glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
	ostream << "\nTextures max:" << (GL_MAX_TEXTURE_UNITS - GL_TEXTURE0) << "\n";
	ostream << "=============================\n";

}

std::map<std::string,std::shared_ptr<glRenderTargetSimple>> m_render_target_map;

irrklang::ISoundEngine *SoundEngine = irrklang::createIrrKlangDevice();

GlGameStateDungeon * p_main_game_state = nullptr;

void CountFps()
{
	static size_t counter = 0;
	static double time_r = 0;

	if (counter++ == 0) time_r = glfwGetTime();
	if (counter > 30)
	{
		EngineSettings::GetEngineSettings()->SetFPS((1.0f * counter / static_cast<float>(glfwGetTime() - time_r)));
		counter = 0;
	}
}

GLFWwindow* CreateOpenGlWindow(bool fullscreen, const char * title)
{
	GLFWwindow* window;
	//Инициализация GLFW
	glfwInit();
	//Настройка GLFW
	//Задается минимальная требуемая версия OpenGL.
	//Мажорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	//Минорная
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	/*
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		//Минорная
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);*/
		//Установка профайла для которого создается контекст
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//Выключение возможности изменения размера окна
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	//multisampling
	//glfwWindowHint(GLFW_SAMPLES, 4);
	if (fullscreen)
	{
		int monitor_count = 0;
		auto monitors = glfwGetMonitors(&monitor_count);
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();

		auto setted_monitor = EngineSettings::GetEngineSettings()->GetMonitorIndex();
		if ((setted_monitor > 0) && (setted_monitor < monitor_count))
		{
			monitor = monitors[setted_monitor];
		}

		const GLFWvidmode* mode = glfwGetVideoMode(monitor);
		glfwWindowHint(GLFW_RED_BITS, mode->redBits);
		glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
		glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
		glfwWindowHint(GLFW_REFRESH_RATE, GLFW_DONT_CARE/*mode->refreshRate*/);
		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);

		SCR_WIDTH = mode->width;
		SCR_HEIGHT = mode->height;
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, monitor, nullptr);
	}
	else
	{
		window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, title, nullptr, nullptr);
	}

	if (window == nullptr)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return window;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(0);

	glfwSetKeyCallback(window, key_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	glewExperimental = GL_TRUE;

	if (glewInit() != GLEW_OK)
	{
		std::cout << "Failed to initialize GLEW" << std::endl;
		return nullptr;
	}

	GlInfo(std::cout);

	return window;
}

void ResetInputs()
{
	inputs[GLFW_KEY_LEFT] = false;
	inputs[GLFW_KEY_RIGHT] = false;
	inputs[GLFW_KEY_UP] = false;
	inputs[GLFW_KEY_DOWN] = false;
	inputs[GLFW_KEY_RIGHT_BRACKET] = false;
	inputs[GLFW_KEY_LEFT_BRACKET] = false;
	inputs[GLFW_KEY_F1] = false;
	inputs[GLFW_KEY_F9] = false;
	inputs[GLFW_KEY_F10] = false;
	inputs[GLFW_KEY_LEFT_ALT] = false;
	inputs[GLFW_KEY_LEFT_CONTROL] = false;
	inputs[GLFW_KEY_ESCAPE] = false;
}

std::tuple <double, double> ProcessMouseInputs(GLFWwindow* window, std::map <int, bool> &inputs_)
{
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);
	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	xpos = (xpos * 2.0f - width) / width;
	ypos = (ypos * 2.0f - height) / height;
	int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
	inputs_[GLFW_MOUSE_BUTTON_LEFT] = (state != GLFW_RELEASE) ? true : false;
	state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
	inputs_[GLFW_MOUSE_BUTTON_RIGHT] = (state != GLFW_RELEASE) ? true : false;
	return std::make_tuple(xpos, ypos);
}

int main(int argc, char const *argv[])
{
    bool is_fullscreen = true;

    if(argc > 1) is_fullscreen = false;

	EngineSettings::Settings main_settings;
	EngineSettings::SetEngineSettings(&main_settings);

    GLFWwindow* window = CreateOpenGlWindow(is_fullscreen, "Brutales");
    
	if (window == nullptr)
	{
		return -1;
	}

	ResetInputs();

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);


	GLResourcesManager resources_atlas("material/textures/","material/meshes/","material/animations/","");
	SetResourceManager(&resources_atlas);
	FillShaders(resources_atlas.m_shader_map,"shaders/list.shd");

	GameSettings::HeroStatus hero_status;
	GameSettings::SetHeroStatus(&hero_status);

	SetRenderTargets(m_render_target_map,width,height);


    std::map<std::string,std::shared_ptr<GlCharacter>> m_glmodels_map;
    auto hero = std::make_shared<GlCharacter>(CharacterTypes::hero);
	m_glmodels_map["Hero"] = hero;
	UpdateCharacterFromFile(argc > 2 ? argv[2] : "heroes/hero.chr",*hero);
	hero->SetName("Hero");

	std::map<std::string,std::shared_ptr<IGlGameState>> states;
    auto game_state_menu = std::make_shared<GlGameStateMenu>(resources_atlas.m_shader_map,m_render_target_map,m_glmodels_map,resources_atlas,states,width,height,SoundEngine,window);
    states["main_menu"] = game_state_menu;
	auto game_state_game = std::make_shared<GlGameStateDungeon>(resources_atlas.m_shader_map,m_render_target_map,m_glmodels_map,resources_atlas,states,width,height,SoundEngine);
    states["main_game"] = game_state_game;
	std::weak_ptr<IGlGameState> game_state = game_state_menu;

	//SoundEngine->play2D("material/audio/breakout.mp3", GL_TRUE);


	while(!glfwWindowShouldClose(window))
	{
		double xpos, ypos;
		std::tie(xpos, ypos) = ProcessMouseInputs(window, inputs);		
		CountFps();

		if(auto state = game_state.lock())
		{
			auto state_new = state->Process(inputs, static_cast<float>(xpos), static_cast<float>(ypos));
			if(auto state_lock = state_new.lock())
			{
				if(state != state_lock)
				{
					state->SwitchOut();
					state_lock->SwitchIn();
				}
				game_state = state_lock;
			}
			EngineSettings::GetEngineSettings()->BeginNewFrame();
			state->Draw();
		}
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	if(p_main_game_state)
	{
		delete(p_main_game_state);
	}

	std::cout << "exit";
	glfwTerminate();
	return 0;
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode)
{

	if (key == GLFW_KEY_ESCAPE)
		inputs[GLFW_KEY_ESCAPE] = (action != GLFW_RELEASE) ? true : false;

	if (key == GLFW_KEY_LEFT_CONTROL)
        inputs[GLFW_KEY_LEFT_CONTROL] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_LEFT || key == GLFW_KEY_A )
        inputs[GLFW_KEY_LEFT] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_RIGHT || key == GLFW_KEY_D )
		inputs[GLFW_KEY_RIGHT] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_UP || key == GLFW_KEY_W )
        inputs[GLFW_KEY_UP] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_DOWN || key == GLFW_KEY_S)
		inputs[GLFW_KEY_DOWN] = (action != GLFW_RELEASE) ?  true : false;

    if (key == GLFW_KEY_SPACE )
    		inputs[GLFW_KEY_SPACE] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_LEFT_ALT )
    		inputs[GLFW_KEY_LEFT_ALT] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_LEFT_SHIFT )
		inputs[GLFW_KEY_LEFT_SHIFT] = (action != GLFW_RELEASE) ?  true : false;

    if (key == GLFW_KEY_LEFT_BRACKET )
        inputs[GLFW_KEY_LEFT_BRACKET] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_ENTER)
		inputs[GLFW_KEY_ENTER] = (action != GLFW_RELEASE) ? true : false;

    if (key == GLFW_KEY_RIGHT_BRACKET )
        inputs[GLFW_KEY_RIGHT_BRACKET] = (action != GLFW_RELEASE) ?  true : false;
    
	if (key == GLFW_KEY_F9 )
        inputs[GLFW_KEY_F9] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_F10 )
        inputs[GLFW_KEY_F10] = (action != GLFW_RELEASE) ?  true : false;

	if (key == GLFW_KEY_F1 )
	{
        inputs[GLFW_KEY_F1] = (action != GLFW_RELEASE) ?  true : false;
		if(action == GLFW_RELEASE)
		{
			EngineSettings::Settings * settings = EngineSettings::GetEngineSettings();
			settings->SetPbr(!settings->IsPbrON());
		}
	}

	
	
}
