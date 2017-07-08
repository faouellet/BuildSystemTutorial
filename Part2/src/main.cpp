#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/labeled_graph.hpp>
#include <boost/filesystem.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>

// TODO: Change it for std::filesystem
namespace fs = boost::filesystem;

void CreateDependencyGraph(const std::string& rootDir)
{
    const std::regex includeRx{ "#include (<|\")([a-zA-Z0-9]+\\.h)(>|\")" };

    for(const auto& dirEntry : fs::recursive_directory_iterator(rootDir))
    {
        if(fs::is_regular_file(dirEntry))
        {
            std::ifstream fileStream(fs::path(dirEntry).string());
            const std::string fileContent{ std::istreambuf_iterator<char>{fileStream}, 
                                           std::istreambuf_iterator<char>{} };
            
            auto rxIt = std::sregex_iterator(fileContent.begin(), fileContent.end(), includeRx);
            auto rxEnd = std::sregex_iterator();
            for(; rxIt != rxEnd; ++rxIt)
            {
                std::cout << rxIt->str(2) << "\n";
            }            
        }
    }
}

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout << " Buils System Tutorial \n\n"
                  << " Usage: bst rootDir\n";
    }
    else
    {
        CreateDependencyGraph(argv[1]);
    }

    return 0;
}