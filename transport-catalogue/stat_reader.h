#pragma once
#include"transport_catalogue.h"
#include"input_reader.h"

#include <string>
#include <iostream>
#include <iomanip>

namespace Output {
	void Inf(std::istream& input, int numb_of_lines, Transport& city);
}