#include "gl2d_interface.h"
#include "gl2d_button.h"
#include "gl2d_image.h"
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

    void Interface2D::AddFont(const std::string& font_name, std::shared_ptr<IGlText> font)
    {
        m_fonts[font_name] = font;
    }

    std::shared_ptr<Gl2D::Gl2dItem> Interface2D::GetElement(const std::string &name)
    {
        return LoaderUtility::GetValueByKey(name,m_items);
    }

    void Interface2D::AddAction(const std::string& action_name, Action item)
    {
        m_actions.emplace(std::make_pair(action_name, item));
    }

    Action Interface2D::GetAction(const std::string& action_name)
    {
        return m_actions[action_name];
    }

    std::shared_ptr<IGlText> Interface2D::GetFont(const std::string& font_name)
    {
        return m_fonts[font_name];
    }

    void Interface2D::Process()
    {

    }

    void Interface2D::Load(const std::string& file_name)
    {
        std::ifstream objects_file;
        objects_file.open(file_name);
        if (objects_file.is_open())
        {
            std::vector<std::string> obj_lines;

            while (!objects_file.eof())
            {
                auto name = LoaderUtility::FindPrefix(objects_file);
                LoaderUtility::LoadLineBlock(objects_file, name, obj_lines);

                if (name == "resourses")
                {

                }
                else if(name == "structure")
                {
                    LoaderUtility::LinesProcessor processor;
                    processor.Add("tabbing", [this](std::stringstream& sstream)
                        {
                            std::string tmp_str;
                            std::string tmp_str_dir;
                            std::string tmp_str_dest;
                            sstream >> tmp_str;
                            sstream >> tmp_str_dir;
                            sstream >> tmp_str_dest;
                            auto command = Inputs::CommandFromString(tmp_str_dir);
                            GetElement(tmp_str)->AddTab(command, GetElement(tmp_str_dest));
                        });

                    processor.Add("parent", [this](std::stringstream& sstream)
                        {
                            std::string tmp_str;
                            std::string tmp_str_parent;
                            sstream >> tmp_str_parent;
                            sstream >> tmp_str;
                            
                            GetElement(tmp_str)->SetParent(GetElement(tmp_str_parent));
                        });
                    processor.Add("active", [this](std::stringstream& sstream)
                        {
                            std::string tmp_str;
                            sstream >> tmp_str;

                            SetActive(tmp_str);
                        });

                    processor.Process(obj_lines);
                }
                else if(name == "image")
                {
                    
                    auto obj_ptr = std::make_shared<Gl2D::GlImage>(m_aspect_ratio);
                    auto obj_name = obj_ptr->Load(obj_lines, *this);
                    obj_ptr->SetAspectRatioKeeper(Gl2D::AspectRatioKeeper::Minimal);
                    Add(obj_name, obj_ptr);
                }
                else if(name == "button")
                {
                    auto obj_ptr = std::make_shared<Gl2D::GlButton>(m_aspect_ratio);
                    auto obj_name = obj_ptr->Load(obj_lines, *this);
                    Add(obj_name, obj_ptr);
                }

            }
            objects_file.close();

        }
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