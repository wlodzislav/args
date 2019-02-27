#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>

#include "../args.h"

using args::option;
using std::vector;

int main(int argc, const char** argv) {
	auto b = false;
	auto num = 0;
	auto str = ""s;
	auto v = std::vector<int>{};
	auto mapk = std::map<int, std::string>{};
	auto mapv = std::map<std::string, int>{};
	auto f = false;
	auto p = args::parser{}
		.option("--bool", &b)
		.option("--num", &num)
		.option("--str", &str)
		.option("-v", &v)
		.option("--mapk", &mapk)
		.option("--mapv", &mapv)
		.option("-f", &f);

	try {
		p.parse(argc, argv);
	} catch (const args::invalid_option& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
	} catch (const args::invalid_value& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
		std::cout << ".value=\"" << err.value << "\"" << std::endl;
	} catch (const args::unexpecter_arg& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".arg=\"" << err.arg << "\"" << std::endl;
	} catch (const std::logic_error& err) {
		std::cout << err.what() << std::endl;
	}
}
