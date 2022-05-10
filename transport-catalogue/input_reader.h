#pragma once
#include"transport_catalogue.h"
#include "log_duration.h"

#include <string>
#include <iostream>
#include <sstream>

namespace Input {
	void ReadCatalogue(std::istream& input, TransportCatalogue& city);
	namespace detail {
		std::string_view Trim(std::string_view word_to_clear);
	}

	namespace Split {
		std::pair<std::string_view, std::string_view> IntoPair(std::string_view word_to_split, char split_char);
		std::vector<std::string_view> Split(std::string_view line, char split_char);
		void ParseStop(std::string_view line, TransportCatalogue& city, std::vector<std::tuple<std::string, std::string, double>>& stops_info);
	}
}