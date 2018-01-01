#include "dependencygraph.h"

#include <boost/filesystem.hpp>
#include <boost/graph/graphviz.hpp>

#include <fstream>
#include <iterator>
#include <regex>

// TODO: Change it for std::filesystem
namespace fs = boost::filesystem;

DependencyGraph::DependencyGraph(const std::string& rootDir) : mRootDir{rootDir}
{
    for(const auto& dirEntry : fs::recursive_directory_iterator(mRootDir))
    {
        if(!fs::is_regular_file(dirEntry))
        {
            continue;
        }
        
        const std::string extension = dirEntry.path().extension().string();
        if(extension != ".cpp" && extension != ".h")
        {
            continue;
        }

        AddDependencies(dirEntry.path().string());
    }

    AddOutputNodeDependencies();
}

void DependencyGraph::WriteToFile(const std::string& filename) const
{
    std::ofstream outFileStream{filename};
    
    boost::write_graphviz(outFileStream, *this);
}

void DependencyGraph::Read(const std::string& filename)
{
    std::ifstream inFileStream{filename};
    boost::dynamic_properties dynProps;
    //const auto& nameVertexMap = boost::get(boost::vertex_name_t{}, *this);
    //dynProps.property("node_id", nameVertexMap);

    boost::read_graphviz(inFileStream, *this, dynProps, "node_id");
}

void DependencyGraph::ComputeUpdate()
{
    GraphT oldGraph = *this;
    auto oldVertices = boost::vertices(oldGraph);
    this->clear();

    // Load the previous dependency graph back into memory if it exists
    Read();

    // Go recursively through the folders hierarchy starting from mRootDir
    for(const auto& dirEntry : fs::recursive_directory_iterator(mRootDir))
    {
        if(!fs::is_regular_file(dirEntry))
        {
            continue;
        }
        
        const std::string extension = dirEntry.path().extension().string();
        if(extension != ".cpp" && extension != ".h")
        {
            continue;
        }

        auto curTimestamp = fs::last_write_time(dirEntry);
        const std::string curFilename = dirEntry.path().string();

        auto nodeIt = std::find_if(oldVertices.first, oldVertices.second, 
                                   [&curFilename](const DepGraphNode& node)
                                   {
                                       return node.filename == curFilename;
                                   });
        if(nodeIt != oldVertices.second)
        {
            // If the file on the filesystem doesn't exists in the graph add it and its dependencies
            AddDependencies(curFilename);
        }
        else if(curTimestamp != (*this)[*nodeIt].timestamp)
        {
            // If the timestamp is different, then . means we have to recompile the file
            (*this)[*nodeIt].needsCompilation = true;
            (*this)[*nodeIt].isAlive = true;
        }
    }

    // Don't forget to add an output node
    AddOutputNodeDependencies();

    // Clean up any unnecessary leftovers from a previous compilation
    auto deadNodeIt = std::partition(oldVertices.first, oldVertices.second, [](const DepGraphNode& node){ return node.isAlive; });
    std::for_each(deadNodeIt, oldVertices.second, 
                  [this](auto& nodeIterator)
                  { 
                      fs::remove({(*this)[*nodeIterator].filename + ".o"});
                      boost::remove_vertex(*nodeIterator, *this);
                  });

    // Finally, write back the graph for the next compilation
    WriteToFile();
}

void DependencyGraph::AddDependencies(const std::string& filename)
{
    std::cout << "Analyzing " << filename << "\n";

    std::ifstream fileStream(filename);
    const std::string fileContent{ std::istreambuf_iterator<char>{fileStream}, 
                                    std::istreambuf_iterator<char>{} };

    const std::regex includeRx{ "#include \"([a-zA-Z0-9\\./]+\\.h)\"" };                               
    auto rxIt = std::sregex_iterator(fileContent.begin(), fileContent.end(), includeRx);
    auto rxEnd = std::sregex_iterator();
    std::cout << "Found dependencies: " << std::distance(rxIt, rxEnd) << "\n";
    for(; rxIt != rxEnd; ++rxIt)
    {
        const std::string dependencyFile = fs::absolute(rxIt->str(1), fs::path(filename).parent_path()).replace_extension(".cpp").string();
        const std::string dependentFile = fs::path(filename).replace_extension(".cpp").string();

        if(dependencyFile == dependentFile)
        {
            continue;
        }

        std::cout << "\t- " << dependencyFile << "\n";
        AddEdge(dependencyFile, dependentFile);
    }
}

void DependencyGraph::AddEdge(const std::string& dependency, const std::string& dependent)
{
    AddNode(dependency);
    AddNode(dependent);
    boost::add_edge(mNodeNames[dependency], mNodeNames[dependent], *this);
}

void DependencyGraph::AddOutputNodeDependencies()
{
    const std::string outputNodeName = "OUTPUT";
    AddNode(outputNodeName);
    const auto& outputNode = mNodeNames[outputNodeName];

    VertexIterator vIt;
    VertexIterator vEnd;
    std::tie(vIt, vEnd) = boost::vertices(*this);

    for(; vIt != vEnd; ++vIt)
    {
        if((boost::out_degree(*vIt, *this) == 0) && (*vIt != outputNode))
        {
            AddEdge((*this)[*vIt].filename, outputNodeName);
        }
    }
}

void DependencyGraph::AddNode(const std::string& filename) 
{
    if(mNodeNames.find(filename) != mNodeNames.end())
    {
        return;
    }

    const auto lastWriteTime = fs::last_write_time(filename);
    mNodeNames[filename] = boost::add_vertex({filename, lastWriteTime, true, false}, *this);
}
