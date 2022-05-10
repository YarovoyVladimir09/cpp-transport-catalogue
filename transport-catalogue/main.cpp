#include <iostream>
#include "input_reader.h"
#include "stat_reader.h"

using namespace std;

int main() {
	Transport SPB;
	int num;
	cin >> num;
	Input::AddInf(cin, num, SPB);
	cin >> num;
	Output::Inf(cin, num, SPB);
}