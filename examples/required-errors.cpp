#include <stdexcept>
#include <iostream>
#include <vector>
#include <string>

#include "../args.h"

int main(int argc, const char** argv) {
	auto a = false;
	auto b = false;
	auto c = false;
	auto d = false;

	auto arg1 = ""s;
	auto rest = std::vector<std::string>{};
	auto carg1 = ""s;
	auto crest = std::vector<std::string>{};
	auto p = args::parser{}
		.arg(args::required, "arg1", &arg1)
		.rest(args::required, "rest", &rest)
		.option(args::required, "-a", &a)
		.option(args::required, "-b", "--bb", &b);

	p.command("cmd")
		.arg(args::required, "carg1", &carg1)
		.rest(args::required, "crest", &crest)
		.option(args::required, "-c", &c)
		.option(args::required, "-d", "--dd", &d);

	try {
		p.parse(argc, argv);
	} catch (const args::missing_command_option& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".command=\"" << err.command << "\"" << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
	} catch (const args::missing_option& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
	} catch (const args::missing_command_arg& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".command=\"" << err.command << "\"" << std::endl;
		std::cout << ".arg=\"" << err.arg << "\"" << std::endl;
	} catch (const args::missing_arg& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".arg=\"" << err.arg << "\"" << std::endl;
	}
}
