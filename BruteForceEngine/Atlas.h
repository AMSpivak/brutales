#ifndef ARTEM_ATLAS
#define ARTEM_ATLAS


#include <map>
#include <string>
#include <memory>
#include <iostream>
#include <functional>

namespace BruteForce
{

    template <typename T>
    class StringAtlas
    {

    public:
        StringAtlas(const std::string& ResourseFolder)
        {
            m_resourse_folder = ResourseFolder;
        }


        ~StringAtlas()
        {
            assert(m_map.empty());
        }
        template <typename... Params>
        std::shared_ptr<T> Assign(const std::string& filename, Params... params)
        {
            auto it = m_map.find(filename);
            if (it != m_map.end())
            {
                std::cout << "Reuse element: " << filename << "\n";
                return it->second.lock();
            }
            auto resource = std::shared_ptr<T>(new T(m_resourse_folder + filename, params...), Deleter(this, filename));
            m_map.insert(std::pair<const std::string, std::weak_ptr<T>>(filename, resource));
            std::cout << "New element: " << filename << "\n";
            return resource;

        }

        std::shared_ptr<T> Find(const std::string& filename)
        {
            auto it = m_map.find(filename);
            if (it != m_map.end())
            {
                std::cout << "Found element: " << filename << "\n";
                return it->second.lock();
            }
            return std::shared_ptr<T>(nullptr);
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
            Deleter(StringAtlas* atlas, const std::string& filename)
                : m_atlas(atlas)
                , m_filename(filename)
            {}

            void operator()(T* item)
            {
                m_atlas->m_map.erase(m_filename);

                delete item;
            }

            StringAtlas* m_atlas;
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
        {
            assert (m_map.empty());
        }
        template <typename... Params>
        std::shared_ptr<T> Assign(const std::string& filename, Params... params)
        {
            size_t hash = m_hasher(filename);
            auto it = m_map.find(hash);
            if (it != m_map.end())
            {
                return it->second.lock();
            }
            auto resource = std::shared_ptr<T>(new T(m_resourse_folder + filename, params...), Deleter(this, hash));
            m_map.insert(std::pair<const size_t, std::weak_ptr<T>>(hash, resource));
            return resource;

        }

        std::shared_ptr<T> Find(const std::string& filename)
        {
            size_t hash = m_hasher(filename);
            auto it = m_map.find(hash);
            if (it != m_map.end())
            {
                return it->second.lock();
            }
            return std::shared_ptr<T>(nullptr);
        }

        void Clean()
        {
        }

        const std::string& GetResourceFolder() const
        {
            return m_resourse_folder;
        }
    private:
        std::hash<std::string> m_hasher;
        std::string m_resourse_folder;
        std::map<const size_t, std::weak_ptr<T>> m_map;

        struct Deleter
        {
            Deleter(HashAtlas* atlas, const size_t& filename_hash)
                : m_atlas(atlas)
                , m_filename_hash(filename_hash)
            {}

            void operator()(T* item)
            {
                m_atlas->m_map.erase(m_filename_hash);

                delete item;
            }

            HashAtlas* m_atlas;
            size_t m_filename_hash;
        };
    };

#ifdef _DEBUG
#define  Atlas StringAtlas
#else
#define  Atlas HashAtlas
#endif
}


#endif