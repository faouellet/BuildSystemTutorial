#ifndef DEP_GRAPH_H
#define DEP_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
//#include <boost/graph/graph_concepts.hpp>

#include <string>
#include <unordered_map>

// TODO: Use static polymorphism instead of runtime polymorphism
class DependencyGraph : public boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, boost::property<boost::vertex_name_t, std::string>>
{
public:
    using Vertex = boost::graph_traits<DependencyGraph>::vertex_descriptor;
    using Edge = boost::graph_traits<DependencyGraph>::edge_descriptor;
    using OutEdgeIterator = boost::graph_traits<DependencyGraph>::out_edge_iterator;
    using VertexIterator = boost::graph_traits<DependencyGraph>::vertex_iterator;

public:
    explicit DependencyGraph(const std::string& rootDir);

private:
    void AddNode(const std::string& filename);
    void AddEdge(const std::string& dependency, const std::string& dependent);

private:
    std::unordered_map<std::string, Vertex> mNodeNames;
};

#endif // DEP_GRAPH_H