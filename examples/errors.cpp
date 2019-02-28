#include <stdexcept>
#include <iostream>
#include <vector>
#include <map>

#include "../args.h"

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

	p.command("cmd")
		.option("--cbool", &b)
		.option("--cnum", &num)
		.option("--cstr", &str)
		.option("-cv", &v)
		.option("--cmapk", &mapk)
		.option("--cmapv", &mapv)
		.option("-cf", &f);

	try {
		p.parse(argc, argv);
	} catch (const args::invalid_option& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
	} catch (const args::invalid_command_option_value& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".command=\"" << err.command << "\"" << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
		std::cout << ".value=\"" << err.value << "\"" << std::endl;
	} catch (const args::invalid_option_value& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".option=\"" << err.option << "\"" << std::endl;
		std::cout << ".value=\"" << err.value << "\"" << std::endl;
	} catch (const args::invalid_command_arg_value& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".command=\"" << err.command << "\"" << std::endl;
		std::cout << ".arg=\"" << err.arg << "\"" << std::endl;
		std::cout << ".value=\"" << err.value << "\"" << std::endl;
	} catch (const args::invalid_arg_value& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".arg=\"" << err.arg << "\"" << std::endl;
		std::cout << ".value=\"" << err.value << "\"" << std::endl;
	} catch (const args::unexpected_arg& err) {
		std::cout << err.what() << std::endl;
		std::cout << ".value=\"" << err.value << "\"" << std::endl;
	} catch (const std::runtime_error& err) {
		// base class for all parser errors
	}
}
