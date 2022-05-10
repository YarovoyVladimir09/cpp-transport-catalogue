#include"stat_reader.h"

using namespace std;

namespace Output {
	void BusOut(std::istream& input, TransportCatalogue& city) {
		string bus_name;
		getline(input, bus_name);
		string_view name = Input::detail::Trim(bus_name);
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

	void StopOut(std::istream& input, TransportCatalogue& city) {
		string stop_name;
		getline(input, stop_name);
		string_view name = Input::detail::Trim(stop_name);
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

	void Inf(std::istream& input, ostream& out , TransportCatalogue& city) {
		int numb_of_lines = 0;
		input >> numb_of_lines;
		string operation_type;
		vector<tuple<string, vector<string>, char>> buses;
		while (numb_of_lines > 0) {
			input >> operation_type;
			if (operation_type == "Bus") {
				BusOut(input, city);
			}
			else if (operation_type == "Stop") {
				StopOut(input, city);
			}
			else {
				cout << "Wrong request" << endl;
			}
			--numb_of_lines;
		}
	}
}