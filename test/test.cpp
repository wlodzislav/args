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
	describe("Function clap::parse()", []{
		describe("supports bool short options", []{
			it("exec -s", []{
				const char* argv[] = {
					"exec",
					"-s"
				};
				bool s = false;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("exec -s 1", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"1"
				};
				bool s = false;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("exec -s 0", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"0"
				};
				bool s = true;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_fail(s);
			});

			it("exec -s=1", []{
				const char* argv[] = {
					"./exec",
					"-s=1"
				};
				bool s = false;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("exec -s=0", []{
				const char* argv[] = {
					"./exec",
					"-s=0"
				};
				bool s = true;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_fail(s);
			});

			it("exec -s1", []{
				const char* argv[] = {
					"./exec",
					"-s1"
				};
				bool s = false;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("exec -s0", []{
				const char* argv[] = {
					"./exec",
					"-s0"
				};
				bool s = true;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_fail(s);
			});
		});

		describe("supports int short options", []{
			it("exec -s 42", []{
				const char* argv[] = {
					"exec",
					"-s",
					"42"
				};
				int s = 0;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(s, 42);
			});

			it("exec -s=42", []{
				const char* argv[] = {
					"exec",
					"-s=42"
				};
				int s = 0;
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(s, 42);
			});
		});

		describe("supports std::string short options", []{
			it("exec -s str", []{
				const char* argv[] = {
					"exec",
					"-s",
					"str"
				};
				std::string s = "";
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(s, std::string("str"));
			});

			it("exec -s=str", []{
				const char* argv[] = {
					"exec",
					"-s=str"
				};
				std::string s = "";
				std::vector<clap::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(s, std::string("str"));
			});
		});

		describe("supports bool long options", []{
			it("exec --long", []{
				const char* argv[] = {
					"exec",
					"--long"
				};
				bool l = false;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(l);
			});

			it("exec --long 1", []{
				const char* argv[] = {
					"exec",
					"--long",
					"1"
				};
				bool l = false;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(l);
			});

			it("exec --long 0", []{
				const char* argv[] = {
					"exec",
					"--long",
					"0"
				};
				bool l = true;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_fail(l);
			});

			it("exec --long=1", []{
				const char* argv[] = {
					"exec",
					"--long=1"
				};
				bool l = false;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(l);
			});

			it("exec --long=0", []{
				const char* argv[] = {
					"exec",
					"--long=0"
				};
				bool l = true;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_fail(l);
			});
		});

		describe("supports int long options", []{
			it("exec --long=42", []{
				const char* argv[] = {
					"exec",
					"--long=42"
				};
				int l = 0;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(l, 42);
			});
		});

		describe("supports std::string long options", []{
			it("exec --long 42", []{
				const char* argv[] = {
					"exec",
					"--long",
					"42"
				};
				int l = 0;
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(l, 42);
			});

			it("exec --long str", []{
				const char* argv[] = {
					"exec",
					"--long",
					"str"
				};
				std::string l = "";
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(l, std::string("str"));
			});

			it("exec --long=str", []{
				const char* argv[] = {
					"exec",
					"--long=str"
				};
				std::string l = "";
				std::vector<clap::option> options = {
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(l, std::string("str"));
			});
		});

		describe("supports options with both short and long names", []{
			it("exec -s=str --long=str", []{
				const char* argv[] = {
					"exec",
					"-s=str",
					"--long=str"
				};
				std::string l = "";
				std::vector<clap::option> options = {
					{"-s", "--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_equal(l, std::string("str"));
			});
		});

		describe("supports mixed options", []{
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
				std::vector<clap::option> options = {
					{"-s", &s},
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(s);
				ctl::expect_equal(l, std::string("str"));
			});

			it("exec s1 --long str", []{
				const char* argv[] = {
					"exec",
					"-s1",
					"--long",
					"str"
				};
				bool s = false;
				std::string l = "";
				std::vector<clap::option> options = {
					{"-s", &s},
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

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
				std::vector<clap::option> options = {
					{"-s", &s},
					{"--long", &l}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				clap::parse(argc, argv, options);

				ctl::expect_ok(s);
				ctl::expect_equal(l, std::string("str"));
			});
		});
	});
}

