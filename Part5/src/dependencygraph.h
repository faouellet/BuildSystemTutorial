#ifndef DEP_GRAPH_H
#define DEP_GRAPH_H

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/subgraph.hpp>

#include <string>
#include <unordered_map>

struct DepGraphNode
{
    std::string filename = {};
    time_t timestamp = {};
    bool needsCompilation = {};
    bool isAlive = {};
};

using GraphT = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, DepGraphNode>;

// TODO: Use static polymorphism instead of runtime polymorphism
class DependencyGraph : public GraphT
{
public:
    using Vertex = boost::graph_traits<DependencyGraph>::vertex_descriptor;
    using Edge = boost::graph_traits<DependencyGraph>::edge_descriptor;
    using OutEdgeIterator = boost::graph_traits<DependencyGraph>::out_edge_iterator;
    using VertexIterator = boost::graph_traits<DependencyGraph>::vertex_iterator;

public:
    DependencyGraph() = default;
    explicit DependencyGraph(const std::string& rootDir);

public:
    void WriteToFile(const std::string& filename = "dep_graph.dot") const;
    void Read(const std::string& filename = "dep_graph.dot");
    void ComputeUpdate();

private:
    void AddDependencies(const std::string& filename);
    void AddEdge(const std::string& dependency, const std::string& dependent);
    void AddOutputNodeDependencies();
    void AddNode(const std::string& filename);

private:
    std::unordered_map<std::string, Vertex> mNodeNames;
    std::string mRootDir;
};

#endif // DEP_GRAPH_H