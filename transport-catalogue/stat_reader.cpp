#include"stat_reader.h"

using namespace std;

namespace Output {
	void Inf(std::istream& input, int numb_of_lines, Transport& city) {
		LOG_DURATION("Output");
		string operation_type;
		vector<tuple<string, vector<string>, char>> buses;
		while (numb_of_lines > 0) {
			input >> operation_type;
			if (operation_type == "Bus") {
				string bus_name;
				getline(input, bus_name);
				string_view name = Input::detail::SpaceClear(bus_name);
				if (!city.BusCount(name)) {
					cout << "Bus " << name << ": not found" << endl;
				}
				else {
					auto [stops, uniq, length, curve] = city.GetBusInfo(name);
					cout << "Bus " << name << ": " << stops << " stops on route, " << uniq
						<< " unique stops, " << setprecision(6) << length << " route length, "
						<< curve << " curvature" << endl;
				}
			}
			else if (operation_type == "Stop") {
				string stop_name;
				getline(input, stop_name);
				string_view name = Input::detail::SpaceClear(stop_name);
				if (!city.StopCount(name)) {
					cout << "Stop " << name << ": not found" << endl;
				}
				else {
					auto& list_of_bus = city.GetStopInfo(name);
					if (list_of_bus.empty()) {
						cout << "Stop " << name << ": no buses" << endl;
					}
					else {
						cout << "Stop " << name << ": buses";
						for (auto& bus : list_of_bus) {
							cout << " " << bus;
						}
						cout << endl;
					}
				}
			}
			else {
				cout << "Wrong request" << endl;
			}
			--numb_of_lines;
		}
	}
}