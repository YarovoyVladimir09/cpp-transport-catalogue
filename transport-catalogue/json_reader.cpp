#include "json_reader.h"

using namespace std;

JSONReader::JSONReader(TransportCatalogue& city_):city(city_){
}

void JSONReader::ReadCatalogue(std::istream& input) {
	json::Document catalogue_data = json::Load(input);
	ParseToCatalogue(catalogue_data);
}

void JSONReader::ParseToCatalogue(json::Document& input) {
	RequestHandler handler(city);
	handler.TransportBase(input);
    handler.TransportRouteSettings(input);
	handler.TransportStat(input);
//    handler.SerializationSettings(input);
//    handler.DeserializationSettings(input);
}

void JSONReader::MakeBase(istream &input) {
    json::Document catalogue_data = json::Load(input);
    RequestHandler handler(city);
    handler.TransportBase(catalogue_data);
    handler.TransportRouteSettings(catalogue_data);
    handler.RenderSettings(catalogue_data, cout);
    handler.SerializationSettings(catalogue_data);
}

void JSONReader::ProcessRequest(istream &input) {
    json::Document catalogue_data = json::Load(input);
    RequestHandler handler(city);
    handler.DeserializationSettings(catalogue_data);
    handler.TransportStat(catalogue_data);
}
