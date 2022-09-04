#pragma once

#include "transport_router.h"
#include "transport_catalogue.h"
#include "map_renderer.h"
#include "router.h"

#include <transport_catalogue.pb.h>
#include <map_renderer.pb.h>
#include <svg.pb.h>
#include <transport_router.pb.h>
#include <graph.pb.h>
#include <sstream>

class TransportSerialization{
public:
    static void Serialization(TransportCatalogue& city, MapRender& render, std::string_view file,
                              TransportRouter& transport_router,graph::Router<double>& router );
    static void Deserialization(TransportCatalogue& city, MapRender& render, std::string_view file,
                                TransportRouter& transport_router,graph::Router<double>& router);
    static void RouterSerializationDatabase(graph::Router<double>& router, RouterData::Router& database);
    static void TransportRouterSerializationDatabase(TransportRouter& transport_router, RouterData::TransportRouter& database);
    static void RouterDeserializationDatabase(graph::Router<double>& router, const RouterData::Router& database);
    static void TransportRouterDeserializationDatabase(TransportRouter& transport_router, RouterData::TransportRouter& database);
    static void TransportCatalogueSerializationDatabase(TransportCatalogue& city, TransportData::TransportCatalogue& database);
    static void TransportCatalogueDeserializationDatabase(TransportCatalogue& city, TransportData::TransportCatalogue& database);
    static void MapRendererSerializationDatabase(MapRender& render, MapData::MapRender& map_database);
    static void MapRendererDeserializationDatabase(MapRender& render,const MapData::MapRender& map_database);
    static void ColorDeserialization(svg::Color& col,const MapData::Color& data);

};