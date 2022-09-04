#include "graph.h"
#include "router.h"
#include "transport_catalogue.h"

//static std::string WAIT = " wait";

//class TransportSerialization;

class TransportRouter{
    using Time = double;
public:
    friend class TransportSerialization;
    TransportRouter(TransportCatalogue& city_);
    TransportRouter() = default;
    void operator=(TransportRouter&& other);
    std::optional<int> GetStopVertex(const std::string_view) const;
    std::string GetVertexName(int) const;
    graph::DirectedWeightedGraph<Time>& GetGraph() ;
private:
    //TransportCatalogue& city;
    void AddBusToGraph(std::vector<Stop*>&, std::string_view, TransportCatalogue& city);
    void FinishGraph();
    std::unordered_map<std::string_view,std::vector<int>> station_and_graphposition;
    std::unordered_map<int, std::string_view> vertex_name;
    graph::DirectedWeightedGraph<Time> route_graph;
    size_t vertex_count = 0;
    double bus_wait_time = 0;
    double bus_velocity = 0;
};