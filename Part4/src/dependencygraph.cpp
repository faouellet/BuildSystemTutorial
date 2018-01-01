#include "dependencygraph.h"

#include <boost/filesystem.hpp>
#include <boost/graph/graphviz.hpp>

#include <cassert>
#include <fstream>
#include <iterator>
#include <regex>

// TODO: Change it for std::filesystem
namespace fs = boost::filesystem;

DependencyGraph::DependencyGraph(const std::string& rootDir)
{
    const std::regex includeRx{ "#include \"([a-zA-Z0-9\\./]+\\.h)\"" };

    for(const auto& dirEntry : fs::recursive_directory_iterator(rootDir))
    {
        if(!fs::is_regular_file(dirEntry))
        {
            continue;
        }
        
        const std::string& extension = dirEntry.path().extension().string();
        if(extension != ".cpp" && extension != ".h")
        {
            continue;
        }

        const std::string curFilename = dirEntry.path().string();
        std::cout << "Analyzing " << curFilename << "\n";
        std::ifstream fileStream(curFilename);
        const std::string fileContent{ std::istreambuf_iterator<char>{fileStream}, 
                                        std::istreambuf_iterator<char>{} };
                                    
        auto rxIt = std::sregex_iterator(fileContent.begin(), fileContent.end(), includeRx);
        auto rxEnd = std::sregex_iterator();
        std::cout << "Found dependencies: " << std::distance(rxIt, rxEnd) << "\n";
        for(; rxIt != rxEnd; ++rxIt)
        {
            const std::string dependencyFile = fs::absolute(rxIt->str(1), dirEntry.path().parent_path()).replace_extension(".cpp").string();
            const std::string dependentFile = fs::path(curFilename).replace_extension(".cpp").string();

            if(dependencyFile == dependentFile)
            {
                continue;
            }

            std::cout << "\t- " << dependencyFile << "\n";
            AddEdge(dependencyFile, dependentFile);
        }            
    }

    const std::string outputNodeName = "OUTPUT";
    AddNode(outputNodeName);
    const auto& outputNode = mNodeNames[outputNodeName];

    const auto& nameVertexMap = boost::get(boost::vertex_name_t{}, *this);

    VertexIterator vIt;
    VertexIterator vEnd;
    std::tie(vIt, vEnd) = boost::vertices(*this);

    for(; vIt != vEnd; ++vIt)
    {
        if((boost::out_degree(*vIt, *this) == 0) && (*vIt != outputNode))
        {
            AddEdge(nameVertexMap[*vIt], outputNodeName);
        }
    }
}

void DependencyGraph::AddNode(const std::string& filename) 
{
    assert(fs::path(filename).extension().string() != ".h");
    if(mNodeNames.find(filename) != mNodeNames.end())
    {
        return;
    }

    mNodeNames[filename] = boost::add_vertex(filename, *this);
}

void DependencyGraph::AddEdge(const std::string& dependency, const std::string& dependent)
{
    AddNode(dependency);
    AddNode(dependent);
    boost::add_edge(mNodeNames[dependency], mNodeNames[dependent], *this);
}
