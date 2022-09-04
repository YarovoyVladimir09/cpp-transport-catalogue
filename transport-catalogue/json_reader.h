#pragma once

#include"transport_catalogue.h"
#include"request_handler.h"

#include <string>
#include <iostream>
#include <sstream>

class JSONReader {
public:
	JSONReader(TransportCatalogue& city_);
	void ReadCatalogue(std::istream& input);
	void ParseToCatalogue(json::Document& input);

    void MakeBase(std::istream& input);
    void ProcessRequest(std::istream& input);
private:
	TransportCatalogue& city;
};
