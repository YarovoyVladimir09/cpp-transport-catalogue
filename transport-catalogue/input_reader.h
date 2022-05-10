#pragma once
#include"transport_catalogue.h"
#include "log_duration.h"

#include <string>
#include <iostream>
#include <sstream>

namespace Input {
	void AddInf(std::istream& input, int numb_of_lines, Transport& city);
	namespace detail {
		std::string_view SpaceClear(std::string_view word_to_clear);
	}

	namespace Split {
		std::pair<std::string_view, std::string_view> OnChar(std::string_view word_to_split, char split_char);
		std::vector<std::string_view> OnVector(std::string_view line, char split_char);
		void StopReq(std::string_view line, Transport& city, std::vector<std::tuple<std::string, std::string, double>>& stops_info);
	}
}