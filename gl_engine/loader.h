#ifndef LOADER_UTILITY
#define LOADER_UTILITY
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <map>
#include <functional>
#include <algorithm>
#include <stdexcept>
#include <map>



namespace LoaderUtility
{
    // template <typename T,typename TS>
    // T GetFromStream(TS& is)
    template <typename T>
    T GetFromStream(std::istream & is)
    {
        T value;
        is>>value;
        return value;
    }

    template <typename K, typename T>
    auto GetValueByKey(const K &key,T& list)
    {
        auto find_to = list.end();
        auto val = std::find_if(list.begin(),find_to,[&key](const auto &b){return b.first == key;});
        if(val == find_to)
        {
           throw std::out_of_range("No name in vocabulary");
        }
        return val->second;
    }

    template <typename K, typename T>
    auto GetValueByKey(const K &key,std::map<K,T>& list)
    {
        return list.at(key);
    }


    template <typename T,typename V>
    auto GetKeyByValue(const V &value,const T& list)
    {
        auto find_to = list.end();
        auto val = std::find_if(list.begin(),find_to,[&value](const auto &b){return b.second == value;});
        if(val == find_to)
        {
           throw std::out_of_range("No value in vocabulary");
        }
        return val->first;
    }


    std::string ReadQuoted(std::istream &source);
    std::string FindPrefix(std::istream &file);
    void LoadLineBlock(std::istream &file,const std::string &sufix,std::vector<std::string> &lines);

    class LinesProcessor
    {
        private:
        std::map<std::string,const std::function<void(std::stringstream&)>> execute_funcs;
        public:
        void Process(std::vector<std::string> &lines) const;
        void Process(const std::string & value) const; 
        void Add(std::string tag,const std::function<void(std::stringstream&)> function);
    };
}

#endif