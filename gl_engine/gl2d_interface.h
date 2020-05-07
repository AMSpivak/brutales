#ifndef GL_2D_INTERFACE
#define GL_2D_INTERFACE
#include "gl2d_item.h"
#include <string>
#include <list>

namespace Gl2D
{
    class Interface2D
    {
        private:
        std::list<std::pair<std::string ,std::shared_ptr<Gl2D::Gl2dItem>>> m_items;
        std::weak_ptr<Gl2D::Gl2dItem> m_active;
        public:

        void Draw();
        void Add(const std::string &name,std::shared_ptr<Gl2D::Gl2dItem> item);
        std::shared_ptr<Gl2D::Gl2dItem> GetElement(const std::string &name);
        void Process();
        void ProcessInput(Inputs::InputCommands input);
        void SetActive(const std::string &name);
    };
}


#endif