#include "transport_catalogue.h"


using namespace std;


TransportCatalogue::Stop::Stop(std::string name, double x, double y) :name_(name) {
	coordinate.lat = x;
	coordinate.lng = y;
}

TransportCatalogue::Bus::Bus(std::string name, std::vector<Stop*> stop, double length_, double curve_) 
	:name_(name), length(length_),curve(curve_) {
	stops_.insert(stops_.begin(), make_move_iterator(stop.begin()), make_move_iterator(stop.end()));
}
TransportCatalogue::BusInfo::BusInfo(int stops_, int uniq_stops_, double full_distance_, double curve_) :
	stops(stops_), uniq_stops(uniq_stops_), full_distance(full_distance_), curve(curve_) {

}

void TransportCatalogue::AddStop(std::string stop_name, double latit, double longit) {
	auto it = &stops_.emplace_front(Stop(move(stop_name), latit, longit));
	stopname_to_stop_[it->name_] = it;
	bus_on_stop_[it->name_];
}

void TransportCatalogue::AddBus(string bus_name, vector<string> stop_rout, char routingtype) {
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
	double length = 0;
	double real_length = 0;
	for (auto stop = stop_rout.begin(); stop != stop_rout.end(); ++stop) {
		stops_on_rout.push_back(stopname_to_stop_[*stop]);
		if (stop + 1 != stop_rout.end()) {
			stop_distance_[{stopname_to_stop_[*stop], stopname_to_stop_[*(stop + 1)]}] =
				ComputeDistance(stopname_to_stop_[*stop]->coordinate, 
					stopname_to_stop_[*(stop + 1)]->coordinate);
			length += GetLength(stopname_to_stop_[*stop], stopname_to_stop_[*(stop + 1)]);
			real_length += GetRealLength(stopname_to_stop_[*stop], stopname_to_stop_[*(stop + 1)]);
		}
	}
	auto it = &buses_.emplace_front(Bus(move(bus_name), stops_on_rout,real_length, real_length / length));
	busname_to_bus_[it->name_] = it;
	for (auto& stopp : it->stops_) {
		bus_on_stop_[stopp->name_].insert(it->name_);
	}
}

	void TransportCatalogue::AddRealDistance(string_view stop1, string_view stop2, double dist) {
		real_stop_distance_[{stopname_to_stop_[stop1], stopname_to_stop_[(stop2)]}] = dist;
	}


size_t TransportCatalogue::StopPairHash::operator()(const std::pair<Stop*, Stop*>& twoStops) const{

	return std::hash<const void*>{}(twoStops.first) + hash<const void*>{}(twoStops.second);
}

size_t TransportCatalogue::StopHash::operator()(const Stop* stop_name) const {

	return std::hash<const void*>{}(stop_name);
}

bool TransportCatalogue::BusCompar::operator()(const Bus* lhs, const Bus* rhs) const {
	return lhs->name_ < rhs->name_;
}

bool TransportCatalogue::BusCount(string_view bus_name) const{
	return busname_to_bus_.count(bus_name);
}

TransportCatalogue::BusInfo TransportCatalogue::GetBusInfo(string_view bus_name){
	
	int size_of_route = busname_to_bus_.at(bus_name)->stops_.size();
	unordered_set<Stop*, StopHash> stops(busname_to_bus_.at(bus_name)->stops_.begin(), busname_to_bus_.at(bus_name)->stops_.end());
	int size_of_unique_stops = stops.size();


	return {BusInfo( size_of_route,size_of_unique_stops,busname_to_bus_.at(bus_name)->length,
		busname_to_bus_.at(bus_name)->curve)};
}

bool TransportCatalogue::StopCount(string_view stop_name) const {
	return stopname_to_stop_.count(stop_name);
}

std::set<std::string_view>& TransportCatalogue::GetStopInfo(std::string_view stop_name) {

	return bus_on_stop_.at(stop_name);
}


double TransportCatalogue::GetLength(Stop* stop1, Stop* stop2) {
	return stop_distance_[{stop1, stop2}];
}
double TransportCatalogue::GetRealLength(Stop* stop1, Stop* stop2) {
	if (real_stop_distance_.find({ stop1, stop2 }) != real_stop_distance_.end()) {
		return real_stop_distance_[{stop1, stop2}];
	}
	else {
		return real_stop_distance_[{stop2, stop1}];
	}

}