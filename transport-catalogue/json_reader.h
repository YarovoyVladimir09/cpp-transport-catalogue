#pragma once

#include"transport_catalogue.h"
#include"json.h"
#include"request_handler.h"

#include <string>
#include <iostream>
#include <sstream>


void ReadCatalogue(std::istream& input, TransportCatalogue& city);
