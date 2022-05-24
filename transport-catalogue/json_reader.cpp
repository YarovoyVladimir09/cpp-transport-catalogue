#include "json_reader.h"

using namespace std;
void ReadCatalogue(std::istream& input, TransportCatalogue& city) {
	json::Document catalogue_data = json::Load(input);
	ParseToCatalogue(catalogue_data, city);
}