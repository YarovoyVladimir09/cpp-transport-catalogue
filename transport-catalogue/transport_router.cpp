#include "transport_router.h"

using Time = double;

TransportRouter::TransportRouter(TransportCatalogue &city_):/*city(city_), */route_graph(2*city_.GetStopNumber()),
bus_velocity(city_.GetBusVelocity()), bus_wait_time(city_.GetWaitTime()){
    for(auto& [bus_name, bus_data] : city_.GetAllBus()){
        AddBusToGraph(bus_data->stops_, bus_name, city_);
    }
    FinishGraph();
}

void TransportRouter::AddBusToGraph(std::vector<Stop *>& stops, std::string_view bus_name, TransportCatalogue& city) {
    for (int i = 0; i<stops.size();++i) {
        if(!station_and_graphposition.count(stops[i]->name_)){
            station_and_graphposition[stops[i]->name_].push_back(vertex_count);// Will make [0]->[1] wait edge
            vertex_name[vertex_count++] = stops[i]->name_;
            station_and_graphposition[stops[i]->name_].push_back(vertex_count);
            vertex_name[vertex_count++] = stops[i]->name_;
        }

        for(int j = i + 1; j < stops.size(); ++j){
            if (!station_and_graphposition.count(stops[j]->name_)) {
                station_and_graphposition[stops[j]->name_].push_back(vertex_count);
                vertex_name[vertex_count++] = stops[j]->name_;
                station_and_graphposition[stops[j]->name_].push_back(vertex_count);
                vertex_name[vertex_count++] = stops[j]->name_;
            }
            double distance = 0;
            int start = i;
            int end = i+1;
            while (end <= j){
                distance += city.GetRealLength(stops[start], stops[end]);
                ++start;
                ++end;
            }
            Time between_stops = (60.0* distance/(1000.0*bus_velocity));
            graph::Edge<Time> edge;
            edge.from = station_and_graphposition.at(stops[i]->name_)[1];
            edge.to = station_and_graphposition.at(stops[j]->name_)[0];
            edge.weight = between_stops;
            edge.edge_status = graph::RouteInfo::Ride;
            edge.ride_info = graph::RideInfo{bus_name,(j-i)};
            route_graph.AddEdge(edge);
        }
    }
}

void TransportRouter::FinishGraph() {
    for(auto& [stop_name, stop_vertexes] : station_and_graphposition){
        graph::Edge<Time> edge;
        edge.from = stop_vertexes[0];
        edge.to = stop_vertexes[1];
        edge.weight = bus_wait_time;
        edge.edge_status = graph::RouteInfo::Wait;
        edge.wait_info.stop = stop_name;
        route_graph.AddEdge(edge);
    }
}

 graph::DirectedWeightedGraph<Time>& TransportRouter::GetGraph() {
    return route_graph;
}

std::optional<int> TransportRouter::GetStopVertex(const std::string_view stop) const{
    if(station_and_graphposition.count(stop)){
        return station_and_graphposition.at(stop)[0];
    }else{
        return std::nullopt;
    }
}

std::string TransportRouter::GetVertexName(int vertex) const {
    return std::string(vertex_name.at(vertex));
}

void TransportRouter::operator=(TransportRouter &&other) {
    station_and_graphposition = std::move(other.station_and_graphposition);
    vertex_name = std::move(other.vertex_name);
    route_graph = std::move(other.route_graph);
    vertex_count = std::move(other.vertex_count);
    bus_wait_time = std::move(other.bus_wait_time);
    bus_velocity = std::move(other.bus_velocity);

}
