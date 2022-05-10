#include "input_reader.h"

using namespace std;
namespace Input {
	namespace Split {

		pair<string_view, string_view> IntoPair(string_view word_to_split, char split_char) {
			size_t pos = word_to_split.find(split_char);
			return { word_to_split.substr(0, pos), word_to_split.substr(pos + 1) };
		}

		vector<string_view> Split(string_view line, char split_char) {
			vector<string_view> result;
			int64_t pos = 0;
			const int64_t pos_end = line.npos;
			while (true) {
				int64_t space = line.find(split_char, pos);
				string_view station = space == pos_end ? line.substr(pos) : line.substr(pos, space - pos);
				result.push_back((detail::Trim(station)));
				if (space == pos_end) {
					break;
				}
				else {
					pos = space + 1;
				}
			}
			return result;
		}

		void ParseStop(string_view line, TransportCatalogue& city,
			vector<tuple<string, string, double>>& stops_info) {
			auto [head, tail] = Split::IntoPair(line, ':');
			auto [latit, other] = Split::IntoPair(tail, ',');
			head = detail::Trim(head);
			if (other.find(',') == other.npos) {
				city.AddStop(move(string(head)), move(stod(string(latit))), move(stod(string(other))));
			}
			else {

				auto [longit, dist_inf] = Split::IntoPair(other, ',');

				city.AddStop((string(head)), move(stod(string(latit))), move(stod(string(longit))));
				vector<string_view> dist_arr = Split::Split(dist_inf, ',');
				for (auto step : dist_arr) {
					auto [distance, to_stop_name] = Split::IntoPair(step, 'm');
					size_t pos = to_stop_name.find_first_not_of(" ");
					to_stop_name = to_stop_name.substr(pos + 2);
					stops_info.push_back({ string(head),
						move(string(detail::Trim(to_stop_name))),stod(string(distance)) });
				}

			}
		}
	}

	namespace detail {
		string_view Trim(string_view word_to_clear) {
			size_t start = word_to_clear.find_first_not_of(" ");
			size_t end = word_to_clear.find_last_not_of(" ");
			return word_to_clear.substr(start, end - start + 1);
		}
	}
	void ReadCatalogue(std::istream& input, TransportCatalogue& city) {
		int numb_of_lines = 0;
		input >> numb_of_lines;
		string operation_type;
		vector<tuple<string, vector<string>, char>> buses;
		vector<tuple<string, string, double>> stops_info;
		while (numb_of_lines > 0) {
			input >> operation_type;
			if (operation_type == "Stop") {
				string line;
				getline(input, line);
				Split::ParseStop(line, city, stops_info);
			}
			else if (operation_type == "Bus") {
				string line;
				vector <string> stops;
				char typerout = '0';
				getline(input, line);
				auto [head, tail] = Split::IntoPair(line, ':');
				if (tail.find('>') != tail.npos) {
					typerout = '>';
				}
				else if (tail.find('-'))
				{
					typerout = '-';
				}
				int64_t pos = 0;
				const int64_t pos_end = tail.npos;
				while (true) {
					int64_t space = tail.find(typerout, pos);
					string_view station = space == pos_end ? tail.substr(pos) : tail.substr(pos, space - pos);
					stops.push_back(string(detail::Trim(station)));
					if (space == pos_end) {
						break;
					}
					else {
						pos = space + 1;
					}
				}

				buses.push_back({ move(string(head.substr(1))), move(stops), typerout });
			}
			--numb_of_lines;
		}

		for (auto& [stop1, stop2, dist] : stops_info) {
			city.AddRealDistance(stop1, stop2, dist);
		}	
		for (auto& [name, stops, type] : buses) {
			city.AddBus(move(name), move(stops), type);
		}

	}
}