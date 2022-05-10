#pragma once

#include <unordered_set>
#include <string>
#include <string_view>
#include <vector>
#include <deque>
#include <unordered_map>
#include <set>

#include "geo.h"

class TransportCatalogue {
public:
	struct Stop {
		Stop(std::string name, double x, double y);
		std::string name_;
		Coordinates coordinate;
	};

	struct Bus {
		Bus(std::string name, std::vector<Stop*> stop, double length_, double curve_);
		std::string name_;
		std::vector<Stop*> stops_;
		double length;
		double curve;
	};

	struct BusInfo {
		BusInfo(int stops_, int uniq_stops_, double full_distance_, double curve_);
		int stops;
		int uniq_stops;
		double full_distance;
		double curve;
	};
	class StopPairHash {
	public:
		size_t operator()(const std::pair<Stop*, Stop*>& twoStops) const;
	};

	class StopHash {
	public:
		size_t operator()(const Stop* stop_name) const;
	};

	class BusCompar {
	public:
		bool operator()(const Bus* lhs, const Bus* rhs) const;
	};

		void AddStop(std::string stop_name, double latit, double longit);
		void AddBus(std::string bus_name, std::vector<std::string> stop_rout, char routingtype);
		void AddRealDistance(std::string_view stop1, std::string_view stop2, double dist);

		bool BusCount(std::string_view bus_name) const;
		bool StopCount(std::string_view stop_name) const;

		BusInfo GetBusInfo(std::string_view bus_name);
		std::set<std::string_view>& GetStopInfo(std::string_view stop_name);
		double GetLength(Stop* stop1, Stop* stop2);
		double GetRealLength(Stop* stop1, Stop* stop2);
	

private:
	std::deque<Stop> stops_;
	std::unordered_map<std::string_view, Stop*> stopname_to_stop_;
	std::deque<Bus> buses_;
	std::unordered_map<std::string_view, Bus*> busname_to_bus_;
	std::unordered_map<std::pair<Stop*, Stop*>, double, StopPairHash> stop_distance_;
	std::unordered_map<std::pair<Stop*, Stop*>, double, StopPairHash> real_stop_distance_;
	std::unordered_map<std::string_view, std::set<std::string_view>> bus_on_stop_;

};