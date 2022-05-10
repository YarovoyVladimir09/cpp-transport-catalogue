#pragma once
#include"transport_catalogue.h"
#include"input_reader.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace Output {
	void Inf(std::istream& input, std::ostream& out, TransportCatalogue& city);
	void BusOut(std::istream& input, TransportCatalogue& city);
	void StopOut(std::istream& input, TransportCatalogue& city);
}