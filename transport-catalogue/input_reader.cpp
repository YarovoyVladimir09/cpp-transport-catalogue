#include "input_reader.h"

using namespace std;
namespace Input {
	namespace Split {

		pair<string_view, string_view> OnChar(string_view word_to_split, char split_char) {
			size_t pos = word_to_split.find(split_char);
			return { word_to_split.substr(0, pos), word_to_split.substr(pos + 1) };
		}

		vector<string_view> OnVector(string_view line, char split_char) {
			vector<string_view> result;
			int64_t pos = 0;
			const int64_t pos_end = line.npos;
			while (true) {
				int64_t space = line.find(split_char, pos);
				string_view station = space == pos_end ? line.substr(pos) : line.substr(pos, space - pos);
				result.push_back((detail::SpaceClear(station)));
				if (space == pos_end) {
					break;
				}
				else {
					pos = space + 1;
				}
			}
			return result;
		}

		void StopReq(string_view line, Transport& city,
			vector<tuple<string, string, double>>& stops_info) {
			auto [head, tail] = Split::OnChar(line, ':');
			auto [latit, other] = Split::OnChar(tail, ',');
			head = detail::SpaceClear(head);
			if (other.find(',') == other.npos) {
				city.AddStop(move(string(head)), move(stod(string(latit))), move(stod(string(other))));
			}
			else {

				auto [longit, dist_inf] = Split::OnChar(other, ',');

				city.AddStop((string(head)), move(stod(string(latit))), move(stod(string(longit))));
				vector<string_view> dist_arr = Split::OnVector(dist_inf, ',');
				for (auto step : dist_arr) {
					auto [distance, to_stop_name] = Split::OnChar(step, 'm');
					size_t pos = to_stop_name.find_first_not_of(" ");
					to_stop_name = to_stop_name.substr(pos + 2);
					stops_info.push_back({ string(head),
						move(string(detail::SpaceClear(to_stop_name))),stod(string(distance)) });
				}

			}
		}
	}

	namespace detail {
		string_view SpaceClear(string_view word_to_clear) {
			size_t start = word_to_clear.find_first_not_of(" ");
			size_t end = word_to_clear.find_last_not_of(" ");
			return word_to_clear.substr(start, end - start + 1);
		}
	}
	void AddInf(std::istream& input, int numb_of_lines, Transport& city) {

		string operation_type;
		vector<tuple<string, vector<string>, char>> buses;
		vector<tuple<string, string, double>> stops_info;
		while (numb_of_lines > 0) {
			input >> operation_type;
			if (operation_type == "Stop") {
				string line;
				getline(input, line);
				Split::StopReq(line, city, stops_info);
			}
			else if (operation_type == "Bus") {
				string line;
				vector <string> stops;
				char typerout = '0';
				getline(input, line);
				auto [head, tail] = Split::OnChar(line, ':');
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
					stops.push_back(string(detail::SpaceClear(station)));
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
		for (auto& [name, stops, type] : buses) {
			city.AddBus(move(name), move(stops), type);
		}
		for (auto& [stop1, stop2, dist] : stops_info) {
			city.AddRealDistance(stop1, stop2, dist);
		}

	}
}