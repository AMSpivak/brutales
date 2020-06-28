#include "loader.h"
#include <regex>

namespace LoaderUtility
{
    std::string ReadQuoted(std::istream &source)
    {
        std::string skip; // dummy
        std::string quoted;
        std::getline(std::getline(source, skip, '"'), quoted, '"');
        return quoted;
    }
    
    std::string FindPrefix(std::istream &file)
    {
        std::string tempholder("");
        std::regex search_regexp("<([^!][^>]*)>");
        std::smatch match;

        while(!file.eof())
        {
            getline(file, tempholder);
            if (std::regex_search(tempholder, match, search_regexp) && match.size() > 1) 
            {
                tempholder = match.str(1);
                return tempholder;
            }
        }
        return std::string("");
        
    }

    void LoadLineBlock(std::istream &file,const std::string &sufix,std::vector<std::string> &lines)
    {
        lines.clear();
        std::string sufix_end("<!"+sufix+">");
        std::string tempholder("");
        std::regex search_regexp("<([!]" + sufix + ")>");
        std::smatch match;

        bool is_block_endline = false;


        while(!file.eof()&&(!is_block_endline))
        {
            getline(file, tempholder);

            if (std::regex_search(tempholder, match, search_regexp) && match.size() > 1) 
            {
                is_block_endline = true;
                std::cout<<match.str(1)<<"\n";
            }

            if(!is_block_endline)
            {
                lines.push_back(tempholder);
            }
        }
    }

    void LinesProcessor::Process(const std::vector<std::string> &lines) const
    {
        for(auto s : lines)
        {
            Process(s);
        }
    }
    void LinesProcessor::Process(const std::string & value) const
    {
        std::stringstream ss(value);
        std::string parameter;
        ss >> parameter;
        try
        {
            execute_funcs.at(parameter)(ss);               
        }
        catch(const std::out_of_range& exp)
        {
            std::cout<<"Unknown parameter: "<<value<<"\n";
        }
    }

    void LinesProcessor::Add(std::string tag,const std::function<void(std::stringstream&)> function)
    {
        execute_funcs.insert(std::make_pair(tag,function));
    }
}