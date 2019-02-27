#include <iterator>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>

#include "../args.h"

template<typename T>
std::ostream& print_arr(std::ostream &ss, const T& v) {
	ss << "[";
	for (auto item = std::begin(v); item != std::end(v); item++) {
		if (item != std::begin(v)) {
			ss << ", ";
		}
		ss << *item;
	}
	ss << "]";
	return ss;
}

template<typename T>
std::ostream& print_map(std::ostream &ss, const T& v) {
	ss << "{";
	for (auto item = std::begin(v); item != std::end(v); item++) {
		if (item != std::begin(v)) {
			ss << ", ";
		}
		ss << item->first << ": " << item->second;
	}
	ss << "}";
	return ss;
}

template<typename T>
std::ostream& operator<<(std::ostream &ss, const std::vector<T>& v) {
	return print_arr(ss, v);
}

template<typename T>
std::ostream& operator<<(std::ostream &ss, const std::list<T>& v) {
	return print_arr(ss, v);
}

template<typename T>
std::ostream& operator<<(std::ostream &ss, const std::set<T>& v) {
	return print_arr(ss, v);
}

template<typename T>
std::ostream& operator<<(std::ostream &ss, const std::unordered_set<T>& v) {
	return print_arr(ss, v);
}

template<typename T, typename V>
std::ostream& operator<<(std::ostream &ss, const std::map<T, V>& v) {
	return print_map(ss, v);
}

template<typename T, typename V>
std::ostream& operator<<(std::ostream &ss, const std::unordered_map<T, V>& v) {
	return print_map(ss, v);
}

#include "./ctl.h"
using namespace ctl;

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

		it("grouped short flags, -rf", []{
			const char* argv[] = {
				"exec",
				"-rf"
			};
			bool r = false;
			bool f = false;
			std::vector<args::option> options = {
				{"-r", &r},
				{"-f", &f}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_ok(r);
			ctl::expect_ok(f);
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

		it("positional args", []{
			const char* argv[] = {
				"./exec",
				"arg1",
				"--long=1",
				"123.123"
			};
			auto s = false;
			auto arg1 = ""s;
			auto arg2 = 0.0;

			auto p = args::parser{}
				.positional(&arg1)
				.positional(&arg2)
				.option("--long", &s);

			const int argc = std::distance(std::begin(argv), std::end(argv));
			p.parse(argc, argv);

			ctl::expect_ok(s);
			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, 123.123);
		});

		it("rest positional args", []{
			const char* argv[] = {
				"./exec",
				"arg1",
				"--long=1",
				"123.123",
				"a",
				"b",
				"c"
			};
			auto s = false;
			auto arg1 = ""s;
			auto arg2 = 0.0;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{}
				.positional(&arg1)
				.positional(&arg2)
				.rest(&rest)
				.option("--long", &s);

			const int argc = std::distance(std::begin(argv), std::end(argv));
			p.parse(argc, argv);

			ctl::expect_ok(s);
			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, 123.123);
			ctl::expect_equal(rest, {"a", "b", "c"});
		});

		describe("Commands", []{
			it("command short + long name", []{
				const char* argv[] = {
					"./exec",
					"l",
					"--long",
				};
				auto s = false;
				auto list_called = false;

				auto p = args::parser{};
				p.command("l", "list")
					.option("--long", &s)
					.action([&]{ list_called = true; });

				const int argc = std::distance(std::begin(argv), std::end(argv));
				p.parse(argc, argv);

				ctl::expect_ok(s);
				ctl::expect_ok(list_called);
			});

			it("command options", []{
				const char* argv[] = {
					"./exec",
					"list",
					"--long",
				};
				auto s = false;
				auto list_called = false;

				auto p = args::parser{};
				p.command("list")
					.option("--long", &s)
					.action([&]{ list_called = true; });

				const int argc = std::distance(std::begin(argv), std::end(argv));
				p.parse(argc, argv);

				ctl::expect_ok(s);
				ctl::expect_ok(list_called);
			});

			it("command args + rest", []{
				const char* argv[] = {
					"./exec",
					"list",
					"arg1",
					"arg2",
					"arg3",
					"arg4",
				};
				auto arg1 = ""s;
				auto arg2 = ""s;
				auto rest = std::vector<std::string>{};
				auto list_called = false;

				auto p = args::parser{};
				p.command("list")
					.positional(&arg1)
					.positional(&arg2)
					.rest(&rest)
					.action([&]{ list_called = true; });

				const int argc = std::distance(std::begin(argv), std::end(argv));
				p.parse(argc, argv);

				ctl::expect_equal(arg1, "arg1"s);
				ctl::expect_equal(arg2, "arg2"s);
				ctl::expect_equal(rest, {"arg3", "arg4"});
				ctl::expect_ok(list_called);
			});

			it("command args + global args", []{
				const char* argv[] = {
					"./exec",
					"list",
					"arg1",
					"arg2",
					"garg1",
					"garg2",
				};
				auto arg1 = ""s;
				auto arg2 = ""s;
				auto garg1 = ""s;
				auto grest = std::vector<std::string>{};
				auto list_called = false;

				auto p = args::parser{}
					.positional(&garg1)
					.rest(&grest);

				p.command("list")
					.positional(&arg1)
					.positional(&arg2)
					.action([&]{ list_called = true; });

				const int argc = std::distance(std::begin(argv), std::end(argv));
				p.parse(argc, argv);

				ctl::expect_equal(arg1, "arg1"s);
				ctl::expect_equal(arg2, "arg2"s);
				ctl::expect_equal(garg1, "garg1"s);
				ctl::expect_equal(grest, {"garg2"});
				ctl::expect_ok(list_called);
			});
		});

		describe("Mixed options", []{
			it("exec -s=on --long=str", []{
				const char* argv[] = {
					"exec",
					"-s=on",
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
				ctl::expect_equal(l, "str"s);
			});

			it("exec -s 1 --long str", []{
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
				ctl::expect_equal(l, "str"s);
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
				ctl::expect_equal(l, "str"s);
			});

			it("exec -s=1 --long=str", []{
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
				ctl::expect_equal(l, "str"s);
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

			it("-s true", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"true"
				};
				bool s = false;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("-s false", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"false"
				};
				bool s = true;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_fail(s);
			});

			it("-s on", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"on"
				};
				bool s = false;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("-s off", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"off"
				};
				bool s = true;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_fail(s);
			});

			it("-s yes", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"yes"
				};
				bool s = false;
				std::vector<args::option> options = {
					{"-s", &s}
				};

				const int argc = std::distance(std::begin(argv), std::end(argv));
				args::parse(argc, argv, options);

				ctl::expect_ok(s);
			});

			it("-s no", []{
				const char* argv[] = {
					"./exec",
					"-s",
					"no"
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

		it("std::string", []{
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

			ctl::expect_equal(s, "str"s);
		});

		it("int", []{
			const char* argv[] = {
				"exec",
				"-i",
				"1234567"
			};
			int i = 0;
			std::vector<args::option> options = {
				{"-i", &i}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_equal(i, 1234567);
		});

		it("double", []{
			const char* argv[] = {
				"exec",
				"-d",
				"1234567.1234567"
			};
			double d = 0;
			std::vector<args::option> options = {
				{"-d", &d}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			ctl::expect_equal(d, 1234567.1234567);
		});

		it("std::vector", []{
			const char* argv[] = {
				"exec",
				"-v",
				"0",
				"-v",
				"1",
				"-v",
				"2"
			};
			std::vector<int> actual = {};
			std::vector<args::option> options = {
				{"-v", &actual}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			std::vector expected = {0, 1, 2};
			ctl::expect_equal(actual, expected);
		});

		it("std::list", []{
			const char* argv[] = {
				"exec",
				"-v",
				"0",
				"-v",
				"1",
				"-v",
				"2"
			};
			std::list<int> actual = {};
			std::vector<args::option> options = {
				{"-v", &actual}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			std::list expected = {0, 1, 2};
			ctl::expect_equal(actual, expected);
		});

		it("std::set", []{
			const char* argv[] = {
				"exec",
				"-v",
				"0",
				"-v",
				"1",
				"-v",
				"2"
			};
			std::set<int> actual = {};
			std::vector<args::option> options = {
				{"-v", &actual}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			std::set expected = {0, 1, 2};
			ctl::expect_equal(actual, expected);
		});

		it("std::unordered_set", []{
			const char* argv[] = {
				"exec",
				"-v",
				"0",
				"-v",
				"1",
				"-v",
				"2"
			};
			std::unordered_set<int> actual = {};
			std::vector<args::option> options = {
				{"-v", &actual}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			std::unordered_set expected = {0, 1, 2};
			ctl::expect_equal(actual, expected);
		});

		it("std::map", []{
			const char* argv[] = {
				"exec",
				"-v",
				"a=A",
				"-v",
				"b=B"
			};
			std::map<std::string, std::string> actual = {};
			std::vector<args::option> options = {
				{"-v", &actual}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			std::map<std::string, std::string> expected = {
				{"a", "A"},
				{"b", "B"},
			};
			ctl::expect_equal(actual, expected);
		});

		it("std::unordered_map", []{
			const char* argv[] = {
				"exec",
				"-v",
				"a=A",
				"-v",
				"b=B"
			};
			std::unordered_map<std::string, std::string> actual = {};
			std::vector<args::option> options = {
				{"-v", &actual}
			};

			const int argc = std::distance(std::begin(argv), std::end(argv));
			args::parse(argc, argv, options);

			std::unordered_map<std::string, std::string> expected = {
				{"a", "A"},
				{"b", "B"},
			};
			ctl::expect_equal(actual, expected);
		});
	});
}

