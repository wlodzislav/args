#include <stdexcept>
#include <iostream>
#include <string>
#include <cstdlib>

#include "../args.h"

using std::vector;

int main(int argc, const char** argv) {
	auto a = ""s;
	auto b = ""s;
	auto c = ""s;
	auto d = ""s;

	auto carg1 = ""s;
	auto crest = std::vector<std::string>{};
	args::parser p = args::parser{}
		.name("cli-cmd")
		.command_required()
		.option("-a", "Global option A", &a)
		.option("-b", "--bb", "Global option B", &b);

	p.command("list", "l", "List command")
		.arg(args::required, "carg1", "List command arg1", &carg1)
		.rest("crest", "List command rest args", &crest)
		.option("-c", "Option C", &c)
		.option(args::required, "-d", "--dd", "Option D", &d);

	p.command("get", "g", "Get command")
		.arg(args::required, "smt", "What to get", &carg1)
		.option("-h", "How to get", &c);

	p.help([&]() {
		auto ind = "  "s;
		std::cout << "NAME\n";
		std::cout << ind << "cli-cmd - Command that does nothing\n";
		std::cout << "\n";
		std::cout << "SYNOPSIS\n";
		std::cout << p.format_usage(ind);
		std::cout << "\n";
		std::cout << "DESCRIPTION\n";
		std::cout << ind << "Command that does nothing and more of nothing\n";
		std::cout << "\n";
		std::cout << "OPTIONS\n";
		std::cout << p.format_options(ind);
		std::cout << "\n";
		std::cout << "COMMANDS\n";
		std::cout << ind << "list - List something\n";
		std::cout << p.format_command_usage("list", ind + ind);
		std::cout << "\n";
		std::cout << p.format_command_args("list", ind + ind);
		std::cout << "\n";
		std::cout << p.format_command_options("list", ind + ind);
		std::cout << "\n";
		std::cout << ind << "get - Get something\n";
		std::cout << p.format_command_usage("get", ind + ind);
		std::cout << "\n";
		std::cout << p.format_command_args("get", ind + ind);
		std::cout << "\n";
		std::cout << p.format_command_options("get", ind + ind);
		std::exit(0);
	});

	p.parse(argc, argv);
}
