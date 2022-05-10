#include "transport_catalogue.h"


using namespace std;


void Transport::AddStop(std::string stop_name, double latit, double longit) {
	auto it = &stops_.emplace_front(Stop(move(stop_name), latit, longit));
	stopname_to_stop_[it->name_] = it;
	bus_on_stop_[it->name_];
}

void Transport::AddBus(string bus_name, vector<string> stop_rout, char routingtype) {
	if (routingtype == '-') {
		vector<string> buff(stop_rout.rbegin() + 1, stop_rout.rend());
		stop_rout.insert(stop_rout.end(), make_move_iterator(buff.begin()), make_move_iterator(buff.end()));
	}
	if (routingtype == '>') {
		if (stop_rout[0] != stop_rout.back()) {
			stop_rout.push_back(stop_rout[0]);
		}
	}
	vector<Stop*> stops_on_rout;

	for (auto stop = stop_rout.begin(); stop != stop_rout.end(); ++stop) {
		stops_on_rout.push_back(stopname_to_stop_[*stop]);
		if (stop + 1 != stop_rout.end()) {
			stop_distance_[{stopname_to_stop_[*stop], stopname_to_stop_[*(stop + 1)]}] =
				ComputeDistance(stopname_to_stop_[*stop]->coordinate, stopname_to_stop_[*(stop + 1)]->coordinate);
		}
	}
	auto it = &buses_.emplace_front(Bus(move(bus_name), stops_on_rout));
	busname_to_bus_[it->name_] = it;
	for (auto& stopp : it->stops_) {
		bus_on_stop_[stopp->name_].insert(it->name_);
	}
}

	void Transport::AddRealDistance(string_view stop1, string_view stop2, double dist) {
		real_stop_distance_[{stopname_to_stop_[stop1], stopname_to_stop_[(stop2)]}] = dist;
	}


size_t Transport::StopPairHash::operator()(const std::pair<Stop*, Stop*>& twoStops) const{

	return std::hash<const void*>{}(twoStops.first) + hash<const void*>{}(twoStops.second);
}

size_t Transport::StopHash::operator()(const Stop* stop_name) const {

	return std::hash<const void*>{}(stop_name);
}

bool Transport::BusCount(string_view bus_name) const{
	return busname_to_bus_.count(bus_name);
}

tuple<int, int, double, double> Transport::GetBusInfo(string_view bus_name){
	
	int size_of_route = busname_to_bus_.at(bus_name)->stops_.size();
	unordered_set<Stop*, StopHash> stops(busname_to_bus_.at(bus_name)->stops_.begin(), busname_to_bus_.at(bus_name)->stops_.end());
	int size_of_unique_stops = stops.size();
	double length = 0;
	double real_length = 0;
	for (auto stop = busname_to_bus_[bus_name]->stops_.begin(); stop != busname_to_bus_[bus_name]->stops_.end(); ++stop) {
		if (stop + 1 != busname_to_bus_[bus_name]->stops_.end()) {
			length += GetLength(*stop, *(stop + 1));
			real_length += GetRealLength(*stop, *(stop + 1));
		}
	}
	return { size_of_route,size_of_unique_stops,real_length, real_length/length };
}

bool Transport::StopCount(string_view stop_name) const {
	return stopname_to_stop_.count(stop_name);
}

std::set<std::string_view>& Transport::GetStopInfo(std::string_view stop_name) {

	return bus_on_stop_.at(stop_name);
}


double Transport::GetLength(Stop* stop1, Stop* stop2) {
	return stop_distance_[{stop1, stop2}];
}
double Transport::GetRealLength(Stop* stop1, Stop* stop2) {
	if (real_stop_distance_.find({ stop1, stop2 }) != real_stop_distance_.end()) {
		return real_stop_distance_[{stop1, stop2}];
	}
	else {
		return real_stop_distance_[{stop2, stop1}];
	}

}