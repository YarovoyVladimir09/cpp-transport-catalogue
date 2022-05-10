#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
	TransportCatalogue SPB;
	Input::ReadCatalogue(cin, SPB);
	Output::Inf(cin,cout, SPB);
}