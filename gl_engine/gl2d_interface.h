#ifndef GL_2D_INTERFACE
#define GL_2D_INTERFACE
#include "gl2d_item.h"
#include <string>
#include <list>
#include <map>

#include <i_gl_font.h>

namespace Gl2D
{
    class Interface2D
    {
        private:
        std::list<std::pair<std::string ,std::shared_ptr<Gl2D::Gl2dItem>>> m_items;
        std::weak_ptr<Gl2D::Gl2dItem> m_active;
        std::map<std::string, Action> m_actions;
        std::map<std::string, std::shared_ptr<IGlText>> m_fonts;
        float m_aspect_ratio;
        public:

        void Draw();
        void SetAspectRatio(float value) { m_aspect_ratio = value; };
        float GetAspectRatio() { return m_aspect_ratio; };
        void Add(const std::string& name, std::shared_ptr<Gl2D::Gl2dItem> item);
        void AddAction(const std::string& action_name, Action item);
        Action GetAction(const std::string &action_name);
        std::shared_ptr<IGlText> GetFont(const std::string& font_name);
        void AddFont(const std::string& font_name, std::shared_ptr<IGlText> font);
        std::shared_ptr<Gl2D::Gl2dItem> GetElement(const std::string &name);
        void Process();
        void Load(const std::string& file_name);
        void ProcessInput(Inputs::InputCommands input);
        void SetActive(const std::string &name);
    };
}


#endif