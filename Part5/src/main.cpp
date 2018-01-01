#include "dependencygraph.h"

#include <boost/graph/topological_sort.hpp>

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

void Compile(const DependencyGraph& graph)
{
    std::vector<DependencyGraph::Vertex> vertices;
    boost::topological_sort(graph, std::back_inserter(vertices));

    //const auto& nameVertexMap = boost::get(boost::vertex_name_t{}, graph);

    std::vector<std::string> objFiles;
    for(const auto& v : vertices)
    {
        std::stringstream sourceCommand;

        const std::string filename;// = nameVertexMap[v];
        sourceCommand << "g++ -c ";
        sourceCommand << filename;

        const std::string objFilename = filename + ".o";
        sourceCommand << " -o ";
        sourceCommand << objFilename;
        
        std::cout << sourceCommand.str() << std::endl;
        objFiles.push_back( objFilename );
        //std::system(sourceCommand.str().c_str());
    }

    std::stringstream objCommand;
    objCommand << "g++ -o output ";
    std::copy(objFiles.begin(), objFiles.end(), std::ostream_iterator<std::string>(objCommand, " "));
    std::cout << objCommand.str() << std::endl;
    //std::system(objCommand.str().c_str());
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
        DependencyGraph dGraph(argv[1]);
        Compile(dGraph);
    }

    return 0;
}