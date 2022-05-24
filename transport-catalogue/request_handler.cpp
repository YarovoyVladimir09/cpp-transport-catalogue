#include "request_handler.h"

using namespace std;

void ParseToCatalogue(json::Document& input, TransportCatalogue& city) {
	Input::TransportBase(input, city);
	Output::TransportStat(input, city);	
}
string_view Trim(string_view word_to_clear) {
	size_t start = word_to_clear.find_first_not_of(" ");
	size_t end = word_to_clear.find_last_not_of(" ");
	return word_to_clear.substr(start, end - start + 1);
}
namespace Input {

	void AddStopFromHandler(const json::Dict& inf, TransportCatalogue& city) {
		city.AddStop(string(Trim(inf.at("name"s).AsString())),
			inf.at("latitude"s).AsDouble(),
			inf.at("longitude"s).AsDouble());
	}

	void AddDistanceBetweenStop(const json::Dict& inf, const string& name,
		vector<tuple<string, string, double>>& stops_info) {
		for (const auto& stop : inf) {
			stops_info.push_back({ name,string(Trim(stop.first)),stop.second.AsDouble() });
		}
	}

	void TransportBase(json::Document& input, TransportCatalogue& city) {
		vector<tuple<string, vector<string>, bool>> buses;
		vector<tuple<string, string, double>> stops_info;
		const auto& req_info = input.GetRoot().AsMap();
		for (const auto& base_ : req_info.at("base_requests"s).AsArray()) {
			const auto& trans_inf = base_.AsMap();
			if (trans_inf.at("type"s).AsString() == "Stop"s) {
				AddStopFromHandler(trans_inf, city);
				if (trans_inf.count("road_distances"s)) {
					AddDistanceBetweenStop(trans_inf.at("road_distances"s).AsMap(),
						string(Trim(trans_inf.at("name"s).AsString())),
						stops_info);
				}
			}
			else if (trans_inf.at("type"s).AsString() == "Bus"s) {
				vector <string> stops;
				for (const auto& stop_ : trans_inf.at("stops"s).AsArray()) {
					stops.push_back(string(Trim(stop_.AsString())));
				}
				buses.push_back({ string(Trim(trans_inf.at("name"s).AsString())),
					move(stops), trans_inf.at("is_roundtrip"s).AsBool() });
			}
		}
		for (auto& [stop1, stop2, dist] : stops_info) {
			city.AddRealDistance(stop1, stop2, dist);
		}
		for (auto& [name, stops, type] : buses) {
			city.AddBus(move(name), move(stops), type);
		}
	}

}
namespace Output {
	using namespace json;
	Dict BusOut(const json::Dict& input, TransportCatalogue& city) {
		const string& name = string(Trim(input.at("name"s).AsString()));
		if (!city.BusCount(name)) {
			return Dict{ {"request_id"s,input.at("id"s).AsInt()},{"error_message"s, "not found"s} };
		}
		else {
			auto [stops, uniq, length, curve] = city.GetBusInfo(name);
			return Dict{
				{"curvature"s,curve},
				{"request_id"s,input.at("id").AsInt()},
				{"route_length"s, length},
				{"stop_count"s,stops},
				{"unique_stop_count"s, uniq}
			};
		}
	}

	Dict StopOut(const json::Dict& input, TransportCatalogue& city) {
		const string& name = string(Trim(input.at("name").AsString()));
		if (!city.StopCount(name)) {
			return Dict{ {"request_id"s,input.at("id").AsInt()},
				{"error_message"s, "not found"s} };

		}
		else {
			vector<string> list_of_bus;
			for (auto bus : city.GetStopInfo(name)) {
				list_of_bus.push_back(string(bus));
			}
			return Dict{ {"buses"s,Array(list_of_bus.begin(),list_of_bus.end())},
					{"request_id"s, input.at("id"s).AsInt()} };
		}
	}

	Dict MapOut(const json::Dict& input, ostringstream& stream) {
		return Dict{ {"map"s,stream.str()},
			{"request_id"s, input.at("id"s).AsInt()} };
	}

	void TransportStat(json::Document& input, TransportCatalogue& city) {
		const auto& req_info = input.GetRoot().AsMap();
		Array result;
		auto context = RenderContext(cout, 2, 0);
		for (const auto& base_ : req_info.at("stat_requests"s).AsArray()) {
			const auto& trans_inf = base_.AsMap();
			if (trans_inf.at("type"s).AsString() == "Stop"s) {

				result.emplace_back(StopOut(trans_inf, city));

			}
			else if (trans_inf.at("type"s).AsString() == "Bus"s) {

				result.emplace_back(BusOut(trans_inf, city));

			}
			else if (trans_inf.at("type"s).AsString() == "Map"s) {
				ostringstream svg_res;
				RenderSettings(input, city, svg_res);
				result.emplace_back(MapOut(trans_inf, svg_res));

			}

		}
		PrintNode(result, context);


	}

	svg::Color ToColor(const json::Node& node) {
		if (node.IsArray()) {
			if (node.AsArray().size() == 3) {
				return svg::Rgb(
					node.AsArray()[0].AsInt(),
					node.AsArray()[1].AsInt(),
					node.AsArray()[2].AsInt());
			}
			else {
				return svg::Rgba(
					node.AsArray()[0].AsInt(),
					node.AsArray()[1].AsInt(),
					node.AsArray()[2].AsInt(),
					node.AsArray()[3].AsDouble());
			}
		}
		else {
			return node.AsString();
		}
	}

	void RenderSettings(json::Document& input, TransportCatalogue& city, ostream& out) {
		const auto& req_info = input.GetRoot().AsMap();
		const auto& render_inf = req_info.at("render_settings"s).AsMap();
		const auto& stops = city.GetAllStopWithBus();
		vector<geo::Coordinates>coordinates;
		for (const auto& stop : stops) {
			coordinates.push_back(stop->coordinate);
		}
		SphereProjector projector(coordinates.begin(), coordinates.end(),
			render_inf.at("width"s).AsDouble(), render_inf.at("height"s).AsDouble(),
			render_inf.at("padding"s).AsDouble());
		Label bus({ render_inf.at("bus_label_font_size"s).AsInt(),
			{
			render_inf.at("bus_label_offset"s).AsArray()[0].AsDouble(),
			render_inf.at("bus_label_offset"s).AsArray()[1].AsDouble()
			} });
		Label stop({ render_inf.at("stop_label_font_size"s).AsInt(),
			{
	render_inf.at("stop_label_offset"s).AsArray()[0].AsDouble(),
	render_inf.at("stop_label_offset"s).AsArray()[1].AsDouble()
			} });

		svg::Color underlayer = ToColor(render_inf.at("underlayer_color"s));
		vector<svg::Color> colors;
		for (const auto& col : render_inf.at("color_palette"s).AsArray()) {
			colors.push_back(ToColor(col));
		}

		MapRenderObj map_obj(projector, render_inf.at("line_width"s).AsDouble(),
			render_inf.at("stop_radius"s).AsDouble(), bus, stop, underlayer,
			render_inf.at("underlayer_width"s).AsDouble(), colors);

		for (const auto& bus : city.GetAllBus()) {
			map_obj.BusRender(bus.second);
		}

		map_obj.SvgRender(out);

	}
}