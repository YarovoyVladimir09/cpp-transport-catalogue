#pragma once
#include"transport_catalogue.h"
#include"json.h"
#include"map_renderer.h"

#include <string>
#include <iostream>
#include <iomanip>
#include <sstream>

void ParseToCatalogue(json::Document& input, TransportCatalogue& city);
namespace Input {
    void AddStopFromHandler(const json::Dict& inf, TransportCatalogue& city);
    void AddDistanceBetweenStop(const json::Dict& inf, const std::string& name,
        std::vector<std::tuple<std::string, std::string, double>>& stops_info);
    void TransportBase(json::Document& input, TransportCatalogue& city);

}
namespace Output {
    json::Dict BusOut(const json::Dict& input, TransportCatalogue& city);
    json::Dict StopOut(const json::Dict& input, TransportCatalogue& city);
    void TransportStat(json::Document& input, TransportCatalogue& city);
    void RenderSettings(json::Document& input, TransportCatalogue& city, std::ostream& out);
}