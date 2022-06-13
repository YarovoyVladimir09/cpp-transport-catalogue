#include "json_builder.h"
#include "json_reader.h"
#include "request_handler.h"
#include "map_renderer.h"
#include <sstream>
#include <fstream>
using namespace std;

int main() {

    stringstream sss(R"(
{
  "base_requests": [
{
  "type": "Stop",
  "name": "Электросети",
  "latitude": 43.598701,
  "longitude": 39.730623,
  "road_distances": {
    "Улица Докучаева": 3000,
    "Улица Лизы Чайкиной": 4300,
    "Stop NoBus": 4300
  }
},
{
  "type": "Stop",
  "name": "NoBus",
  "latitude": 43.59870,
  "longitude": 39.7306,
  "road_distances": {
    "Улица Докучаева": 3000,
    "Улица Лизы Чайкиной": 4300,
    "Stop NoBus": 4300
  }
},
{
  "type": "Bus",
  "name": "14",
  "stops": [
    "Улица Лизы Чайкиной",
    "Электросети",
    "Улица Докучаева",
    "Улица Лизы Чайкиной"
  ],
  "is_roundtrip": true
}  
],
  "stat_requests": 
[ 
{
  "id": 1,
  "type": "Stop",
  "name": ""
},
{
  "id": 2,
  "type": "Stop",
  "name": "Электросети"
} ,
{
  "id": 3,
  "type": "Stop",
  "name": "Stop NoBus"
},
{
  "id": 4,
  "type": "Stop",
  "name": "NoBus"
},
{
  "id": 5,
  "type": "Stop",
  "name": "NoStop"
},
{
  "id": 6,
  "type": "Bus",
  "name": "14"
}     
]
} 

    )"s);
    fstream s("s10_final_opentest_3.json");
    TransportCatalogue MSK;
    JSONReader reader(MSK);
    reader.ReadCatalogue(sss);

}