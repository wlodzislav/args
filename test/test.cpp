#include <vector>
#include <iterator>
#include <iostream>

#include "../args.h"
#include "./ctl.h"

using namespace ctl;

/*

	Support functors as options value destination
	Support for -- option
	Support for standart numeric and string types
	Support for required values and default values
	Support for --no- prefix with bool options
	Support for non-conventional syntax: "-frtti", -fno-rtti" and "+fb"
	Support for multiple option values
	Support more more then one occurrences of an option or operand
	Support for standart containers
	Support for option groups
	Support for commands, command-local options
	Support for hidden options
	format_quick_help
	format_help
	Support for validation via functor or regexp
	Support for unregistered options
	Support for options dependencies, usage pattern
	Support for completeon generation via special option
	Support for response files with set of options

*/

/*

	exec -s --long
	exec -s -a -b
	exec -sab

*/

/*

	Boolean

		bool

	Support for standart numeric types:

		short
		unsigned short
		int
		unsigned int
		long
		unsigned long
		long long
		unsigned long long
		float
		double
		long double

	Support for standart string types:

		std::string     std::basic_string<char>
		// with int max_size
		char*

	Support for standart sequence containers:

		// with values delimiter

		std::array<>
		std::vector<>
		std::list<>

	Support for standart associative containers:

		// with values delimiter
	
		std::set<>
		std::unordered_set<>

		// with pairs and key-value delimiter

		std::map<>
		std::unordered_map<>

*/

int main() {
	describe("args::parse()", []{
		it("short flag, -s", []{
			const char* argv[] = {
				"exec",
				"-s"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"-s", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		it("short without space, -s1", []{
			const char* argv[] = {
				"./exec",
				"-s1"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"-s", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		it("short with value, -s 1", []{
			const char* argv[] = {
				"./exec",
				"-s",
				"1"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"-s", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		it("short = value, -s=1", []{
			const char* argv[] = {
				"./exec",
				"-s=1"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"-s", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		it("long flag, --long", []{
			const char* argv[] = {
				"./exec",
				"--long"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"--long", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		it("long with value, --long 1", []{
			const char* argv[] = {
				"./exec",
				"--long",
				"1"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"--long", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		it("long = value, --long=1", []{
			const char* argv[] = {
				"./exec",
				"--long=1"
			};
			bool s = false;
			std::vector<args::option> options = {
				{"--long", &s}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(s);
		});

		describe("Mixed options", []{
			it("exec -s=str --long=str", []{
				const char* argv[] = {
					"exec",
					"-s=str",
					"--long=str"
				};
				std::string l = "";
				std::vector<args::option> options = {
					{"-s", "--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_equal(l, std::string("str"));
			});

			it("exec s 1 --long str", []{
				const char* argv[] = {
					"exec",
					"-s",
					"1",
					"--long",
					"str"
				};
				bool s = false;
				std::string l = "";
				std::vector<args::option> options = {
					{"-s", &s},
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
				ctl::expect_equal(l, std::string("str"));
			});

			it("exec -s1 --long str", []{
				const char* argv[] = {
					"exec",
					"-s1",
					"--long",
					"str"
				};
				bool s = false;
				std::string l = "";
				std::vector<args::option> options = {
					{"-s", &s},
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
				ctl::expect_equal(l, std::string("str"));
			});

			it("exec s=1 --long=str", []{
				const char* argv[] = {
					"exec",
					"-s=1",
					"--long=str"
				};
				bool s = false;
				std::string l = "";
				std::vector<args::option> options = {
					{"-s", &s},
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
				ctl::expect_equal(l, std::string("str"));
			});
		});
	});

	describe("Types", []{
		describe("bool", []{
			it("-s", []{
				const char* argv[] = {
					"./exec",
					"-s"
				};
				bool s = false;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("no flag", []{
				const char* argv[] = {
					"./exec"
				};
				bool s = false;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_fail(s);
			});

			it("-s 1", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"1"
				};
				bool s = false;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("-s 0", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"0"
				};
				bool s = true;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_fail(s);
			});
		});

		describe("std::string", []{
			it("-s str", []{
				const char* argv[] = {
					"exec",
					"-s",
					"str"
				};
				std::string s = "";
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_equal(s, std::string("str"));
			});
		});
	});
}

