//
// Created by vladimir on 02.09.22.
//
#include "serialization.h"

#include <fstream>
#include <iostream>
#include <filesystem>

using namespace std;

void TransportSerialization::Serialization(TransportCatalogue &city, MapRender &render, std::string_view file,
                                           TransportRouter& transport_router,graph::Router<double>& router ) {
    const filesystem::path path = file;
    ofstream out_file(path, ios::binary);
    //out_file.open(path, ios::binary);
    out_file.clear();
    TransportData::TransportCatalogue database;
    MapRendererSerializationDatabase(render, *database.mutable_map_obj());
    TransportCatalogueSerializationDatabase(city, database);
    RouterSerializationDatabase(router, *database.mutable_transport_router_()->mutable_router_());
    TransportRouterSerializationDatabase(transport_router, *database.mutable_transport_router_());
    database.SerializeToOstream(&out_file);

}

void TransportSerialization::Deserialization(TransportCatalogue &city, MapRender &render, std::string_view file,
                                             TransportRouter &transport_router, graph::Router<double> &router ) {
    const filesystem::path path = file;
    ifstream in_file(path, ios::binary);
    TransportData::TransportCatalogue database;
    database.ParseFromIstream(&in_file);
    TransportCatalogueDeserializationDatabase(city, database);
    MapRendererDeserializationDatabase(render, database.map_obj());
    transport_router = move(TransportRouter(city));
    RouterDeserializationDatabase(router, database.transport_router_().router_());
    router.SetGraph(transport_router.route_graph);
    in_file.clear();
}

void TransportSerialization::TransportCatalogueSerializationDatabase(TransportCatalogue &city, TransportData::TransportCatalogue& database) {

   // TransportData::TransportCatalogue database;
   database.set_bus_wait_time(city.GetWaitTime());
   database.set_bus_velocity(city.GetBusVelocity());
    for(auto& [name, stop ] : city.GetAllStop() ){
        TransportData::Stop* dataStop = database.add_stops();
        dataStop->set_name(stop->name_);
        dataStop->set_latit(stop->coordinate.lat);
        dataStop->set_longit(stop->coordinate.lng);
    }

    for(auto& [name, bus] : city.GetAllBus() ){
        TransportData::Bus* dataBus = database.add_buses();
        dataBus->set_name(bus->name_);
        dataBus->set_routingtype(bus->roundtrip);
        if(bus->roundtrip) {
            for (auto stop: bus->stops_) {
                dataBus->add_stops(stop->name_);
            }
        }else{
            for(int i = 0;i <= bus->stops_.size()/2; ++i){
                dataBus->add_stops(bus->stops_[i]->name_);
            }
        }
    }

    for(auto& [stops_pair, value]: city.GetAllRealDistances() ){
        TransportData::StopDistance* distance = database.add_distances();
        distance->set_first_stop(stops_pair.first->name_);
        distance->set_second_stop(stops_pair.second->name_);
        distance->set_dist(value);
    }

    //database.SerializeToOstream(&file);
}

void TransportSerialization::TransportCatalogueDeserializationDatabase(TransportCatalogue& city, TransportData::TransportCatalogue& database) {

//    TransportData::TransportCatalogue database;
//    database.ParseFromIstream(&file);
    for(auto& stop : database.stops()){
        city.AddStop(stop.name(), stop.latit(), stop.longit());
    }
    for(auto& distance : database.distances() ){
        city.AddRealDistance(distance.first_stop(),
                             distance.second_stop(),
                             distance.dist());
    }
    for(auto& bus : database.buses()){
        city.AddBus(bus.name(),
                    { bus.stops().begin(), bus.stops().end()},
                    bus.routingtype());
    }
    city.SetBusVelocity(database.bus_velocity());
    city.SetWaitTime(database.bus_wait_time());

}

void TransportSerialization::MapRendererSerializationDatabase(MapRender &render, MapData::MapRender& map_database) {
    //MapData::MapRender map_database;
    MapData::SphereProjector proj;
    proj.set_padding_(render.proj_.padding_);
    proj.set_min_lon_(render.proj_.min_lon_);
    proj.set_max_lat_(render.proj_.max_lat_);
    proj.set_zoom_coeff_(render.proj_.zoom_coeff_);
    *map_database.mutable_projector()= proj;

    MapData::SVG svg_info;
    svg_info.set_line_w_(render.line_w_);
    svg_info.set_stop_rad_(render.stop_rad_);

    MapData::Label bus_buffer ;
    bus_buffer.set_x(render.bus_.offset.x);
    bus_buffer.set_y(render.bus_.offset.y);
    bus_buffer.set_font_size(render.bus_.font_size);
    *svg_info.mutable_bus_() = bus_buffer;

    MapData::Label stop_buffer ;
    stop_buffer.set_x(render.stop_.offset.x);
    stop_buffer.set_y(render.stop_.offset.y);
    stop_buffer.set_font_size(render.stop_.font_size);
    *svg_info.mutable_stop_() = stop_buffer;


    MapData::Color underlay;
    ostringstream sout;
    svg::SerPrintColor(sout, render.underl_ );
    underlay.set_color_info(sout.str());
    *svg_info.mutable_underl_() = underlay;
    sout.str("");

    svg_info.set_underl_w_(render.underl_w_);

    for(auto& col : render.colors_){
        MapData::Color* ser_col = svg_info.add_colors_();
        svg::SerPrintColor(sout, col);
        auto chek = sout.str();
        ser_col->set_color_info(sout.str());
        sout.str("");
    }
    *map_database.mutable_svg_setting() = svg_info;

    //map_database.SerializeToOstream(&file);
}

void TransportSerialization::MapRendererDeserializationDatabase(MapRender &render, const MapData::MapRender& map_database) {

//    MapData::MapRender map_database;
//    map_database.ParseFromIstream(&file);
    SphereProjector proj_buffer(map_database.projector().padding_(),
                           map_database.projector().min_lon_(),
                           map_database.projector().max_lat_(),
                           map_database.projector().zoom_coeff_());
    Label bus_lab = {map_database.svg_setting().bus_().font_size(),
                     {map_database.svg_setting().bus_().x(), map_database.svg_setting().bus_().y()}};
    Label stop_lab = {map_database.svg_setting().stop_().font_size(),
                     {map_database.svg_setting().stop_().x(), map_database.svg_setting().stop_().y()}};
    svg::Color underlay;
    ColorDeserialization(underlay, map_database.svg_setting().underl_());

    vector<svg::Color> colors;
    svg::Color color_buffer;
    for(auto& col : map_database.svg_setting().colors_()){
        ColorDeserialization(color_buffer, col);
        colors.push_back(color_buffer);
    }
    MapRender result(proj_buffer, map_database.svg_setting().line_w_(), map_database.svg_setting().stop_rad_(),
              bus_lab, stop_lab, underlay, map_database.svg_setting().underl_w_(), colors);
    render = std::move(result);

}

void TransportSerialization::ColorDeserialization(svg::Color &col,const MapData::Color &data) {
    stringstream string_buff;
    string_buff<<data.color_info();
    string type;
    string_buff>>type;
    if (type == "monostate"s){
        col.emplace<std::monostate>();
    } else if (type == "string"s){
        string colinfo;
        string_buff>>colinfo;
        col.emplace<string>(colinfo);
    }else if(type == "rgb"s){
        string red, green, blue;
        string_buff>>red>>green>>blue;
        col.emplace<svg::Rgb>(stoi(red), stoi(green), stoi(blue));
    }else if (type == "rgba"s){
        string red, green, blue, opacity;
        string_buff>>red>>green>>blue>>opacity;
        col.emplace<svg::Rgba>(stoi(red), stoi(green), stoi(blue), stod(opacity));
    }

}

void TransportSerialization::TransportRouterSerializationDatabase(TransportRouter &transport_router,
                                                                  RouterData::TransportRouter &database) {
    RouterData::DirectedWeightedGraph weightedGraph;
    for(auto& vert : transport_router.route_graph.GetAllEdges()){
        GraphData::Edge* edge = weightedGraph.add_edges_();
        edge->set_from(vert.from);
        edge->set_to(vert.to);
        edge->set_weight(vert.weight);
        GraphData::RideInfo r_info;
        r_info.set_bus(string (vert.ride_info.bus));
        r_info.set_span_count(vert.ride_info.span_count);
        *edge->mutable_ride_info() = r_info;
        edge->set_edge_status(static_cast<GraphData::RouteInfo>((int)vert.edge_status));
        *edge->mutable_wait_info()->mutable_stop() = string(vert.wait_info.stop);
    }

    for(auto& inc_list : transport_router.route_graph.GetIncidenceLists()){
        RouterData::IncidenceList* incidenceList = weightedGraph.add_incidence_lists_();
        for (auto edge_id : inc_list) {
            incidenceList->add_edgeids(edge_id);
        }
    }
    google::protobuf::Map<string,RouterData::Vertexes > stationAndGraph;
    for(const auto& [stop_name, vec_vert] : transport_router.station_and_graphposition){
        RouterData::Vertexes veres;
        for(int to_add : vec_vert){
            veres.add_vertexes(to_add);
        }
        stationAndGraph[string(stop_name)] = veres;
    }
    *database.mutable_station_and_graphposition() = stationAndGraph;


    google::protobuf::Map<int,string > vertexesName;
    for(const auto& [vert_id, stop_name] : transport_router.vertex_name  ){
        vertexesName[vert_id] = string (stop_name);
    }
    *database.mutable_vertex_name() = vertexesName;

    database.set_bus_wait_time(transport_router.bus_wait_time);
    database.set_bus_velocity(transport_router.bus_velocity);
    database.set_vertex_count(transport_router.vertex_count);
}

void TransportSerialization::RouterSerializationDatabase(graph::Router<double> &router, RouterData::Router &database) {
    for(auto& horiz_elem: router.GetInternalData()){
        RouterData::RouteInternalData* routInternalData = database.add_routes_internal_data_();
        for(auto& int_data : horiz_elem){
            RouterData::InternalData* internal_data = routInternalData->add_data();
            if(int_data.has_value()) {
                internal_data->set_isweigth(true);
                internal_data->set_weight(int_data->weight);
                if(int_data->prev_edge.has_value()) {
                    internal_data->set_isedge(true);
                    internal_data->set_prev_edge(int_data->prev_edge.value());
                } else{
                    internal_data->set_isedge(false);
                }
            } else{
                internal_data->set_isweigth(false);
                internal_data->set_isedge(false);

            }
        }
    }
}

void TransportSerialization::RouterDeserializationDatabase(graph::Router<double> &router,
                                                           const RouterData::Router &database) {
    std::vector<std::vector<std::optional<graph::Router<double>::RouteInternalData>>> result;
    for(auto& item : database.routes_internal_data_()){
        std::vector<std::optional<graph::Router<double>::RouteInternalData>> vert;
            for (auto &iterData: item.data()) {
                if(iterData.isweigth()) {
                    if (iterData.isedge()) {
                        vert.push_back(graph::Router<double>::RouteInternalData{
                                iterData.weight(), iterData.prev_edge()
                        });
                    } else {
                        vert.push_back(graph::Router<double>::RouteInternalData{
                                iterData.weight(), std::nullopt
                        });
                    }
                }else{
                    vert.push_back(nullopt);
                }
            }
        result.push_back(vert);
    }
    router.SetInternalData(move(result));
}

void TransportSerialization::TransportRouterDeserializationDatabase(TransportRouter &transport_router,
                                                                    RouterData::TransportRouter &database) {

}





