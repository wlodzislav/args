#include <stdexcept>
#include <iostream>
#include <string>

#include "../args.h"

using std::vector;

int main(int argc, const char** argv) {
	auto a = ""s;
	auto b = ""s;
	auto c = ""s;
	auto d = ""s;

	auto arg1 = ""s;
	auto rest = std::vector<std::string>{};
	auto carg1 = ""s;
	auto crest = std::vector<std::string>{};
	args::parser p = args::parser{}
		.name("cli-cmd")
		.command_required()
		.arg(args::required, "arg1", "Global arg 1", &arg1)
		.arg("arg2", "Global arg 2", &arg1)
		.rest("rest", "Rest global args", &rest)
		.option(args::required, "-a", "Global option A", &a)
		.option("-b", "--bb", "Global option B", &b);

	p.command("list", "l", "List command")
		.arg(args::required, "carg1", "List command arg1", &carg1)
		.rest("crest", "List command rest args", &crest)
		.option("-c", "Option C", &c)
		.option(args::required, "-d", "--dd", "Option D", &d);

	p.command("get", "g", "Get command")
		.arg(args::required, "smt", "What to get", &carg1)
		.option("-h", "How to get", &c);

	p.parse(argc, argv);
}
