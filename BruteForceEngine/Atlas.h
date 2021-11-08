#ifndef ARTEM_ATLAS
#define ARTEM_ATLAS


#include <map>
#include <string>
#include <memory>
#include <iostream>

namespace BruteForce
{

    template <typename T>
    class Atlas
    {

    public:
        Atlas(const std::string& ResourseFolder)
        {
            m_resourse_folder = ResourseFolder;
        }


        ~Atlas()
        {}

        std::shared_ptr<T> Assign(const std::string& filename)
        {
            auto it = m_map.find(filename);
            if (it != m_map.end())
            {
                std::cout << "Reuse element: " << filename << "\n";
                return it->second.lock();
            }
            auto resource = std::shared_ptr<T>(new T(m_resourse_folder + filename), Deleter(this, filename));
            m_map.insert(std::pair<const std::string, std::weak_ptr<T>>(filename, resource));
            std::cout << "New element: " << filename << "\n";
            return resource;

        }

        void Clean()
        {
        }

        const std::string& GetResourceFolder() const
        {
            return m_resourse_folder;
        }
    private:

        std::string m_resourse_folder;
        std::map<const std::string, std::weak_ptr<T>> m_map;

        struct Deleter
        {
            Deleter(Atlas* atlas, const std::string& filename)
                : m_atlas(atlas)
                , m_filename(filename)
            {}

            void operator()(T* item)
            {
                m_atlas->m_map.erase(m_filename);
                delete item;
            }

            Atlas* m_atlas;
            std::string m_filename;
        };



    };

    template <typename T>
    class HashAtlas
    {

    public:
        HashAtlas(const std::string& ResourseFolder)
        {
            m_resourse_folder = ResourseFolder;
        }


        ~HashAtlas()
        {}

        std::shared_ptr<T> Assign(const std::string& filename)
        {
            auto it = m_map.find(filename);
            if (it != m_map.end())
            {
                std::cout << "Reuse element: " << filename << "\n";
                return it->second.lock();
            }
            auto resource = std::shared_ptr<T>(new T(m_resourse_folder + filename), Deleter(this, filename));
            m_map.insert(std::pair<const std::string, std::weak_ptr<T>>(filename, resource));
            std::cout << "New element: " << filename << "\n";
            return resource;

        }

        void Clean()
        {
        }

        const std::string& GetResourceFolder() const
        {
            return m_resourse_folder;
        }
    private:

        std::string m_resourse_folder;
        std::map<const std::string, std::weak_ptr<T>> m_map;

        struct Deleter
        {
            Deleter(HashAtlas* atlas, const std::string& filename)
                : m_atlas(atlas)
                , m_filename(filename)
            {}

            void operator()(T* item)
            {
                m_atlas->m_map.erase(m_filename);
                delete item;
            }

            HashAtlas* m_atlas;
            std::string m_filename;
        };



    };
}


#endif