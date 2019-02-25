// ./examples/simple -s

#include <iostream>

#include "../args.h"

using args::option;
using std::vector;

int main(int argc, const char** argv) {
	bool s;

	auto options = {
		option{"-s", &s}
	};

	args::parse(argc, argv, options);

	std::clog << "-s " << s << "\n";
}
