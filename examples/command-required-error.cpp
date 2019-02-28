#include <iostream>

#include "../args.h"

int main(int argc, const char** argv) {
	auto p = args::parser{}
		.command_required();

	p.command("cmd");

	try {
		p.parse(argc, argv);
	} catch (const args::missing_command& err) {
		std::cout << err.what() << std::endl;
	}
}
