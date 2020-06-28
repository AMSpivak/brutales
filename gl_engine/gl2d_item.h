#ifndef GL_2D_ENGINE_ITEM
#define GL_2D_ENGINE_ITEM
#include <memory>
#include <tuple>
#include <map>
#include <vector>
#include <string>
#include "input_abstracts.h"

namespace Gl2D
{
    enum class ItemAligment {None,Fill,Center,Left,Right,Top,Bottom};
    enum class AspectRatioKeeper {None,Width,Height,Maximal,Minimal};

    class Gl2dItem
    {
        protected:

        float real_width;
        float real_height;
        float real_x;
        float real_y;
        float m_x;
        float m_y;
        float m_width;
        float m_height;
        float m_aspect_ratio;
        ItemAligment m_aligment;
        AspectRatioKeeper m_aspect_ratio_keeper;
        std::weak_ptr<Gl2dItem> m_parent;
        std::map<Inputs::InputCommands,std::weak_ptr<Gl2dItem>> tab_map;
        bool m_active;
        void CalculateAligment();
        void UseAspectRatioKeeper();
        void RecalculateGeometry();
        public:
        Gl2dItem(float aspect_ratio):m_aspect_ratio(aspect_ratio), m_active(false) {}
        Gl2dItem(float x,float y,float width, float height, float aspect_ratio):
                                                                                     m_x(x)
                                                                                    ,m_y(y)
                                                                                    ,m_width(width)
                                                                                    ,m_height(height)
                                                                                    ,m_aspect_ratio(aspect_ratio)
                                                                                    ,m_active(false)

        {}

        void SetAspectRatioKeeper(AspectRatioKeeper keeper);
        void SetParent(std::weak_ptr<Gl2dItem> parent);
        AspectRatioKeeper GetAspectRatioKeeper();
        void SetItemAligment(ItemAligment aligment);
        void AddTab(Inputs::InputCommands input,std::weak_ptr<Gl2dItem> tab_element);
        virtual std::weak_ptr<Gl2dItem> ProcessInput(Inputs::InputCommands input);
        ItemAligment GetItemAligment();
        std::tuple<float,float,float, float> GetPosAndSize();
        void SetActive(bool status);
        virtual void Draw() = 0;
        virtual void Load(const std::vector<std::string>& lines) = 0;
        virtual ~Gl2dItem(){}

    };

}

#endif