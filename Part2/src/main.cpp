#include <boost/filesystem.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>

#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>
#include <unordered_map>

// TODO: Change it for std::filesystem
namespace fs = boost::filesystem;

class DependencyGraph
{
public:
    explicit DependencyGraph(const std::string& rootDir)
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
                std::cout << "\t- " << fs::absolute(rxIt->str(1), dirEntry.path().parent_path()).string() << "\n";
                AddEdge(rxIt->str(1), fs::path(curFilename).filename().string());
            }            
        }
    }

public:
    void Print() const
    {
        std::ofstream outFileStream{"dep_graph.dot"};
        boost::write_graphviz(outFileStream, mGraph);
    }

private:
    void AddNode(const std::string& filename) 
    {
        if(mNodeNames.find(filename) != mNodeNames.end())
            return;

        mNodeNames[filename] = boost::add_vertex(filename, mGraph);
    }

    void AddEdge(const std::string& dependency, const std::string& dependent)
    {
        AddNode(dependency);
        AddNode(dependent);
        boost::add_edge(mNodeNames[dependency], mNodeNames[dependent], mGraph);
    }

private:
    using VertexProperty = boost::property<boost::vertex_name_t, std::string>;
    using GraphT = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexProperty>;
    using VertexDesc = boost::graph_traits<GraphT>::vertex_descriptor;

    GraphT mGraph;
    std::unordered_map<std::string, VertexDesc> mNodeNames;
};

int main(int argc, char** argv)
{
    if(argc != 2)
    {
        std::cout << " Buils System Tutorial \n\n"
                  << " Usage: bst rootDir\n";
    }
    else
    {
        DependencyGraph dGraph(argv[1]);
        dGraph.Print();
    }

    return 0;
}