#include "gl2d_interface.h"
//#include <iostream>
#include "loader.h"

namespace Gl2D
{
    void Interface2D::Draw()
    {
        //std::cout << "Draw: \n";
        for(auto item : m_items)
        {
            //std::cout << "\t"<<item.first << "\n";
            item.second->Draw();
        }
    }

    void Interface2D::Add(const std::string &name,std::shared_ptr<Gl2D::Gl2dItem> item)
    {
        m_items.push_back(std::make_pair(name,item));
    }

    std::shared_ptr<Gl2D::Gl2dItem> Interface2D::GetElement(const std::string &name)
    {
        return LoaderUtility::GetValueByKey(name,m_items);
    }


    void Interface2D::Process()
    {

    }

    void Interface2D::ProcessInput(Inputs::InputCommands input)
    {
        if(auto active = m_active.lock())
        {
            auto new_active = active->ProcessInput(input);
            if(auto new_active_lck = new_active.lock())
            {
                m_active = new_active;
                new_active_lck->SetActive(true);
                active->SetActive(false);
            }
        }
    }

    void Interface2D::SetActive(const std::string &name)
    {
        auto new_active = GetElement(name);
        GetElement(name)->SetActive(true);
        if(auto active = m_active.lock())
        {
            active->SetActive(false);
        }
        m_active = new_active;
    }


}