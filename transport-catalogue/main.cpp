#include "json_builder.h"
#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"

#include <fstream>
#include <iostream>
#include <string_view>

using namespace std::literals;

void PrintUsage(std::ostream& stream = std::cerr) {
    stream << "Usage: transport_catalogue [make_base|process_requests]\n"sv;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        PrintUsage();
        return 1;
    }

    const std::string_view mode(argv[1]);

    if (mode == "make_base"sv) {
        std::fstream s;
        s.open("inputMake.json", std::ios_base::in);
        TransportCatalogue city;
        JSONReader reader(city);
        reader.MakeBase(s);

    } else if (mode == "process_requests"sv) {
        std::fstream s;
        s.open("inputProcess.json", std::ios_base::in);
        TransportCatalogue city;
        JSONReader reader(city);
        reader.ProcessRequest(s);

    } else {
        PrintUsage();
        return 1;
    }

//    if (mode == "make_base"sv) {
//
//                TransportCatalogue city;
//        JSONReader reader(city);
//        reader.MakeBase(std::cin);
//
//    } else if (mode == "process_requests"sv) {
//
//                TransportCatalogue city;
//        JSONReader reader(city);
//        reader.ProcessRequest(std::cin);
//
//    } else {
//        PrintUsage();
//        return 1;
//    }
}
