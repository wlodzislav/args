#include <iterator>
#include <iostream>
#include <vector>
#include <list>
#include <set>
#include <unordered_set>
#include <utility>
#include <map>
#include <unordered_map>

#include "../args.h"

using namespace std::literals;

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

template<typename T1, typename T2>
std::ostream& operator<<(std::ostream &ss, const std::pair<T1, T2>& p) {
	ss << "[" << p.first << ", " << p.second << "]";
	return ss;
}

#include "./ctl.h"
using namespace ctl;

int main() {
	describe("Options", []{
		it("short name, -s", []{
			const char* argv[] = {
				"exec",
				"-s"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"-s", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("grouped short flags, -rf", []{
			const char* argv[] = {
				"exec",
				"-rf"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto f = false;
			args::options options = {
				{"-r", &r},
				{"-f", &f}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(r, true);
			ctl::expect_equal(f, true);
		});

		it("short with no space value, -s1", []{
			const char* argv[] = {
				"./exec",
				"-s1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"-s", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("short with value, -s 1", []{
			const char* argv[] = {
				"./exec",
				"-s",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"-s", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("short = value, -s=1", []{
			const char* argv[] = {
				"./exec",
				"-s=1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"-s", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("long name, --long", []{
			const char* argv[] = {
				"./exec",
				"--long"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"--long", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("long with value, --long 1", []{
			const char* argv[] = {
				"./exec",
				"--long",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"--long", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("long = value, --long=1", []{
			const char* argv[] = {
				"./exec",
				"--long=1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			args::options options = {
				{"--long", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
		});

		it("implicit --no-long for flags", []{
			const char* argv[] = {
				"./exec",
				"--no-long"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = true;
			args::options options = {
				{"--long", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, false);
		});

		it("non conventional", []{
			const char* argv[] = {
				"./exec",
				"-frtti",
				"-fno-rtti",
				"value",
				"+fb=1",
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto frtti = false;
			auto fnorrtti = ""s;
			auto fb = 0;
			args::options options = {
				{"-frtti", &frtti},
				{"-fno-rtti", &fnorrtti},
				{"+fb", &fb}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(frtti, true);
			ctl::expect_equal(fnorrtti, "value"s);
			ctl::expect_equal(fb, 1);
		});
	});

	describe("Global Args", []{
		it("arg", []{
			const char* argv[] = {
				"./exec",
				"arg1",
				"--long=1",
				"123.123"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto arg1 = ""s;
			auto arg2 = 0.0;

			auto p = args::parser{}
				.arg(&arg1)
				.arg(&arg2)
				.option("--long", &s);

			p.parse(argc, argv);

			ctl::expect_equal(s, true);
			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, 123.123);
		});

		it("arg, rest", []{
			const char* argv[] = {
				"./exec",
				"arg1",
				"--long=1",
				"123.123",
				"a",
				"b",
				"c"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto arg1 = ""s;
			auto arg2 = 0.0;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{}
				.arg(&arg1)
				.arg(&arg2)
				.rest(&rest)
				.option("--long", &s);

			p.parse(argc, argv);

			ctl::expect_equal(s, true);
			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, 123.123);
			ctl::expect_equal(rest, {"a", "b", "c"});
		});

		it("--", []{
			const char* argv[] = {
				"./exec",
				"arg1",
				"--",
				"--long=1",
				"123.123",
				"a",
				"b",
				"c"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto arg1 = ""s;
			auto arg2 = ""s;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{}
				.arg(&arg1)
				.arg(&arg2)
				.rest(&rest)
				.option("--long", &s);

			p.parse(argc, argv);

			ctl::expect_equal(s, false);
			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, "--long=1"s);
			ctl::expect_equal(rest, {"123.123", "a", "b", "c" });
		});
	});

	describe("Commands", []{
		it("multi word name", []{
			const char* argv[] = {
				"./exec",
				"multi", "word",
				"multi", "word", "cmd"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto rest = std::vector<std::string>{};
			auto list_called = false;

			auto p = args::parser{}
				.rest(&rest);

			p.command("multi word cmd", &list_called);

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
			ctl::expect_equal(rest, {"multi", "word"});
		});

		it("options", []{
			const char* argv[] = {
				"./exec",
				"l",
				"--long",
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto list_called = false;

			auto p = args::parser{};
			p.command("l", "list")
				.option("--long", &s)
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_equal(s, true);
			ctl::expect_ok(list_called);
		});

		it("args + rest", []{
			const char* argv[] = {
				"./exec",
				"list",
				"arg1",
				"arg2",
				"arg3",
				"arg4",
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto arg1 = ""s;
			auto arg2 = ""s;
			auto rest = std::vector<std::string>{};
			auto list_called = false;

			auto p = args::parser{};
			p.command("list")
				.arg(&arg1)
				.arg(&arg2)
				.rest(&rest)
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, "arg2"s);
			ctl::expect_equal(rest, {"arg3", "arg4"});
			ctl::expect_ok(list_called);
		});

		it("args + global args", []{
			const char* argv[] = {
				"./exec",
				"list",
				"arg1",
				"arg2",
				"garg1",
				"garg2",
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto arg1 = ""s;
			auto arg2 = ""s;
			auto garg1 = ""s;
			auto grest = std::vector<std::string>{};
			auto list_called = false;

			auto p = args::parser{}
				.arg(&garg1)
				.rest(&grest);

			p.command("list")
				.arg(&arg1)
				.arg(&arg2)
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_equal(arg1, "arg1"s);
			ctl::expect_equal(arg2, "arg2"s);
			ctl::expect_equal(garg1, "garg1"s);
			ctl::expect_equal(grest, {"garg2"});
			ctl::expect_ok(list_called);
		});
	});

	describe("Required throws", []{
		it("simple syntax option name", []{
			const char* argv[] = {
				"exec"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto d = false;
			args::options options = {
				{args::required, "-d", &d},
			};

			auto catched = false;
			try {
				args::parse(argc, argv, options);
			} catch (args::missing_option err) {
				catched = true;
				ctl::expect_equal(err.option, "-d"s);
			}
			ctl::expect_ok(catched);
		});

		it("simple syntax option short, long", []{
			const char* argv[] = {
				"exec"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto e = false;
			args::options options = {
				{args::required, "-e", "--ee", &e},
			};

			auto catched = false;
			try {
				args::parse(argc, argv, options);
			} catch (args::missing_option err) {
				catched = true;
				ctl::expect_equal(err.option, "-e, --ee"s);
			}
			ctl::expect_ok(catched);
		});

		it("option name", []{
			const char* argv[] = {
				"./exec"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto p = args::parser{}
				.option(args::required, "-r", &r);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (args::missing_option err) {
				catched = true;
				ctl::expect_equal(err.option, "-r"s);
			}
			ctl::expect_ok(catched);
		});

		it("option short, long", []{
			const char* argv[] = {
				"./exec"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto p = args::parser{}
				.option(args::required, "-r", "--required", &r);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (args::missing_option err) {
				catched = true;
				ctl::expect_equal(err.option, "-r, --required"s);
			}
			ctl::expect_ok(catched);
		});

		it("option non-conventional", []{
			const char* argv[] = {
				"./exec"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto p = args::parser{}
				.option(args::required, "-rd", &r);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (args::missing_option err) {
				catched = true;
				ctl::expect_equal(err.option, "-rd"s);
			}
			ctl::expect_ok(catched);
		});

		it("arg", []{
			const char* argv[] = {
				"./exec",
				"arg1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto arg1 = ""s;
			auto arg2 = ""s;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{}
				.arg(args::required, "arg1", &arg1)
				.arg(args::required, "arg2", &arg2)
				.rest(args::required, "rest", &rest);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (const args::missing_arg& err) {
				catched = true;
				ctl::expect_equal(err.arg, "arg2"s);
			}
			ctl::expect_ok(catched);
		});

		it("rest args", []{
			const char* argv[] = {
				"./exec",
				"arg1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto arg1 = ""s;
			auto arg2 = ""s;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{}
				.arg(args::required, "arg1", &arg1)
				.arg(args::required, "arg2", &arg2)
				.rest(args::required, "rest", &rest);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (const args::missing_arg& err) {
				catched = true;
				ctl::expect_equal(err.arg, "arg2"s);
			}
			ctl::expect_ok(catched);
		});

		it("command option name", []{
			const char* argv[] = {
				"./exec",
				"cmd"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto p = args::parser{};
			
			p.command("cmd")
				.option(args::required, "-r", &r);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (args::missing_command_option err) {
				catched = true;
				ctl::expect_equal(err.command, "cmd"s);
				ctl::expect_equal(err.option, "-r"s);
			}
			ctl::expect_ok(catched);
		});

		it("command option short, long", []{
			const char* argv[] = {
				"./exec",
				"cmd"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto p = args::parser{};
			
			p.command("cmd")
				.option(args::required, "-r", "--required", &r);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (args::missing_command_option err) {
				catched = true;
				ctl::expect_equal(err.command, "cmd"s);
				ctl::expect_equal(err.option, "-r, --required"s);
			}
			ctl::expect_ok(catched);
		});

		it("command option non-conventional", []{
			const char* argv[] = {
				"./exec",
				"cmd"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = false;
			auto p = args::parser{};
			
			p.command("cmd")
				.option(args::required, "-rd", &r);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (args::missing_command_option err) {
				catched = true;
				ctl::expect_equal(err.command, "cmd"s);
				ctl::expect_equal(err.option, "-rd"s);
			}
			ctl::expect_ok(catched);
		});

		it("command args", []{
			const char* argv[] = {
				"./exec",
				"cmd",
				"arg1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto arg1 = ""s;
			auto arg2 = ""s;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{};
			p.command("cmd")
				.arg(args::required, "arg1", &arg1)
				.arg(args::required, "arg2", &arg2)
				.rest(args::required, "rest", &rest);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (const args::missing_arg& err) {
				catched = true;
				ctl::expect_equal(err.arg, "arg2"s);
			}
			ctl::expect_ok(catched);
		});

		it("command rest args", []{
			const char* argv[] = {
				"./exec",
				"cmd",
				"arg1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto arg1 = ""s;
			auto arg2 = ""s;
			auto rest = std::vector<std::string>{};

			auto p = args::parser{};
			p.command("cmd")
				.arg(args::required, "arg1", &arg1)
				.arg(args::required, "arg2", &arg2)
				.rest(args::required, "rest", &rest);

			auto catched = false;
			try {
				p.parse(argc, argv);
			} catch (const args::missing_arg& err) {
				catched = true;
				ctl::expect_equal(err.arg, "arg2"s);
			}
			ctl::expect_ok(catched);
		});
	});

	describe("Types", []{
		it("bool", []{
			const char* argv[] = {
				"./exec",
				"-a",
				"-b", "1", "-B", "0",
				"-c", "true", "-C", "false",
				"-d", "on", "-D", "off",
				"-e", "yes", "-E", "no",
				"-f1", "-F0",
				"-gtrue", "-Gfalse",
				"-hon", "-Hoff",
				"-iyes", "-Ino"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = true;
			auto b = true; auto B = false;
			auto c = true; auto C = false;
			auto d = true; auto D = false;
			auto e = true; auto E = false;
			auto f = true; auto F = false;
			auto g = true; auto G = false;
			auto h = true; auto H = false;
			auto i = true; auto I = false;
			args::options options = {
				{"-a", &a},
				{"-b", &b}, {"-B", &B},
				{"-c", &c}, {"-C", &C},
				{"-d", &d}, {"-D", &D},
				{"-e", &e}, {"-E", &E},
				{"-f", &f}, {"-F", &F},
				{"-g", &g}, {"-G", &G},
				{"-h", &h}, {"-H", &H},
				{"-i", &i}, {"-I", &I}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(B, false);
			ctl::expect_equal(c, true);
			ctl::expect_equal(C, false);
			ctl::expect_equal(d, true);
			ctl::expect_equal(D, false);
			ctl::expect_equal(e, true);
			ctl::expect_equal(E, false);
			ctl::expect_equal(f, true);
			ctl::expect_equal(F, false);
			ctl::expect_equal(g, true);
			ctl::expect_equal(G, false);
			ctl::expect_equal(h, true);
			ctl::expect_equal(H, false);
			ctl::expect_equal(i, true);
			ctl::expect_equal(I, false);
		});

		it("std::string", []{
			const char* argv[] = {
				"exec",
				"-s",
				"str"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = ""s;
			args::options options = {
				{"-s", &s}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, "str"s);
		});

		it("int", []{
			const char* argv[] = {
				"exec",
				"-i",
				"1234567"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto i = 0;
			args::options options = {
				{"-i", &i}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(i, 1234567);
		});

		it("double", []{
			const char* argv[] = {
				"exec",
				"-d",
				"1234567.1234567"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto d = 0.0;
			args::options options = {
				{"-d", &d}
			};

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
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto v = std::vector<int>{};
			args::options options = {
				{"-v", &v}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(v, {0, 1, 2});
		});

		it("std::vector action", []{
			const char* argv[] = {
				"exec",
				"-v",
				"0",
				"-v",
				"1",
				"-v",
				"2"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto vec = std::vector<int>{};

			auto p = args::parser{}
				.option<int>("-v", [&](auto v) { vec.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(vec, {0, 1, 2});
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
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto v = std::list<int>{};
			args::options options = {
				{"-v", &v}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(v, {0, 1, 2});
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
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto v = std::set<int>{};
			args::options options = {
				{"-v", &v}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(v, {0, 1, 2});
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
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto v = std::unordered_set<int>{};
			args::options options = {
				{"-v", &v}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(v, {0, 1, 2});
		});

		it("std::pair", []{
			const char* argv[] = {
				"exec",
				"-p",
				"a=0"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto p = std::pair<std::string, int>{"", 0};
			args::options options = {
				{"-p", &p}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(p, {"a", 0});
		});
		it("std::map", []{
			const char* argv[] = {
				"exec",
				"-v",
				"a=A",
				"-v",
				"b=B"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto v = std::map<std::string, std::string>{};
			args::options options = {
				{"-v", &v}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(v, {
				{"a", "A"},
				{"b", "B"},
			});
		});

		it("std::map action", []{
			const char* argv[] = {
				"exec",
				"-v",
				"a=A",
				"-v",
				"b=B"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto m = std::map<std::string, std::string>{};

			auto p = args::parser{}
				.option<std::pair<std::string, std::string>>("-v", [&](auto v) { m.insert(v); });

			p.parse(argc, argv);

			ctl::expect_equal(m, {
				{"a", "A"},
				{"b", "B"},
			});
		});

		it("std::unordered_map", []{
			const char* argv[] = {
				"exec",
				"-v",
				"a=A",
				"-v",
				"b=B"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto v = std::unordered_map<std::string, std::string>{};
			args::options options = {
				{"-v", &v}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(v, {
				{"a", "A"},
				{"b", "B"},
			});
		});
	});
	describe("Overloads", []{
		it("simple syntax options", []{
			const char* argv[] = {
				"exec",
				"-a",
				"-b",
				"--cc",
				"-d",
				"-e",
				"--ff"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto b = false;
			auto c = false;
			auto d = false;
			auto e = false;
			auto f = false;
			args::options options = {
				{"-a", &a},
				{"-b", "--bb", &b},
				{"-c", "--cc", &c},
				{args::required, "-d", &d},
				{args::required, "-e", "--ee", &e},
				{args::required, "-f", "--ff", &f}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(d, true);
			ctl::expect_equal(e, true);
			ctl::expect_equal(f, true);
		});

		it("options", []{
			const char* argv[] = {
				"exec",
				"-a",
				"-b",
				"--cc",
				"-d",
				"-e",
				"--ff",
				"-A",
				"-B",
				"--CC",
				"-D",
				"-E",
				"--FF"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto A = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto d = false;
			auto D = false;
			auto e = false;
			auto E = false;
			auto f = false;
			auto F = false;
			auto desc = "Description"s;

			auto p = args::parser{}
				.option("-a", &a)
				.option("-A", desc, &A)
				.option("-b", "--bb", &b)
				.option("-B", "--BB", desc, &B)
				.option("-c", "--cc", &c)
				.option("-C", "--CC", desc, &C)
				.option(args::required, "-d", &d)
				.option(args::required, "-D", desc, &D)
				.option(args::required, "-e", "--ee", &e)
				.option(args::required, "-E", "--EE", desc, &E)
				.option(args::required, "-f", "--ff", &f)
				.option(args::required, "-F", "--FF", desc, &F);

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(d, true);
			ctl::expect_equal(e, true);
			ctl::expect_equal(f, true);
			ctl::expect_equal(A, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(C, true);
			ctl::expect_equal(D, true);
			ctl::expect_equal(E, true);
			ctl::expect_equal(F, true);
		});

		it("options handlers", []{
			const char* argv[] = {
				"exec",
				"-a",
				"-b",
				"--cc",
				"-d",
				"-e",
				"--ff",
				"-A",
				"-B",
				"--CC",
				"-D",
				"-E",
				"--FF"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto A = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto d = false;
			auto D = false;
			auto e = false;
			auto E = false;
			auto f = false;
			auto F = false;
			auto desc = "Description"s;

			auto p = args::parser{}
				.option<bool>("-a", [&](auto v) { a = v; })
				.option<bool>("-A", desc, [&](auto v) { A = v; })
				.option<bool>("-b", "--bb", [&](auto v) { b = v; })
				.option<bool>("-B", "--BB", desc, [&](auto v) { B = v; })
				.option<bool>("-c", "--cc", [&](auto v) { c = v; })
				.option<bool>("-C", "--CC", desc, [&](auto v) { C = v; })
				.option<bool>(args::required, "-d", [&](auto v) { d = v; })
				.option<bool>(args::required, "-D", desc, [&](auto v) { D = v; })
				.option<bool>(args::required, "-e", "--ee", [&](auto v) { e = v; })
				.option<bool>(args::required, "-E", "--EE", desc, [&](auto v) { E = v; })
				.option<bool>(args::required, "-f", "--ff", [&](auto v) { f = v; })
				.option<bool>(args::required, "-F", "--FF", desc, [&](auto v) { F = v; });

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(d, true);
			ctl::expect_equal(e, true);
			ctl::expect_equal(f, true);
			ctl::expect_equal(A, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(C, true);
			ctl::expect_equal(D, true);
			ctl::expect_equal(E, true);
			ctl::expect_equal(F, true);
		});

		it("args", []{
			const char* argv[] = {
				"exec",
				"true",
				"true",
				"true",
				"true",
				"true"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto desc = "Description"s;

			auto p = args::parser{}
				.arg(&a)
				.arg("b", &b)
				.arg("B", desc, &B)
				.arg(args::required, "c", &c)
				.arg(args::required, "C", desc, &C);

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(C, true);
		});

		it("args handlers", []{
			const char* argv[] = {
				"exec",
				"true",
				"true",
				"true",
				"true",
				"true"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto desc = "Description"s;

			auto p = args::parser{}
				.arg<bool>([&](auto v) { a = v; })
				.arg<bool>("b", [&](auto v) { b = v; })
				.arg<bool>("B", desc, [&](auto v) { B = v; })
				.arg<bool>(args::required, "c", [&](auto v) { c = v; })
				.arg<bool>(args::required, "C", desc, [&](auto v) { C = v; });

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(C, true);
		});

		it("rest", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest(&r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest handler", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest<bool>([&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest("rest", &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name, description", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest("rest", desc, &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name, handler", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest<bool>("rest", [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name, description, handler", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest<bool>("rest", desc, [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest(args::required, "rest", &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name, description", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest(args::required, "rest", desc, &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name, handler", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest<bool>(args::required, "rest", [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name, handler, description", []{
			const char* argv[] = {
				"exec",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{}
				.rest<bool>(args::required, "rest", desc, [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("command name, destination", []{
			const char* argv[] = {
				"./exec",
				"l"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto list_called = false;

			auto p = args::parser{};
			p.command("l", &list_called);

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name, decription, destination", []{
			const char* argv[] = {
				"./exec",
				"l"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Command description"s;
			auto list_called = false;

			auto p = args::parser{};
			p.command("l", &list_called);

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name, alias, destination", []{
			const char* argv[] = {
				"./exec",
				"l"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto list_called = false;

			auto p = args::parser{};
			p.command("l", "list", &list_called);

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name, alias, description, destination", []{
			const char* argv[] = {
				"./exec",
				"l"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Command description"s;
			auto list_called = false;

			auto p = args::parser{};
			p.command("l", "list", desc, &list_called);

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name + action", []{
			const char* argv[] = {
				"./exec",
				"list"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto list_called = false;

			auto p = args::parser{};
			p.command("list")
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name, description + action", []{
			const char* argv[] = {
				"./exec",
				"list"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Command description"s;
			auto list_called = false;

			auto p = args::parser{};
			p.command("list", desc)
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name, alias + action", []{
			const char* argv[] = {
				"./exec",
				"list"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto list_called = false;

			auto p = args::parser{};
			p.command("l", "list")
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

		it("command name, alias, description + action", []{
			const char* argv[] = {
				"./exec",
				"list"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Command description"s;
			auto list_called = false;

			auto p = args::parser{};
			p.command("l", "list", desc)
				.action([&]{ list_called = true; });

			p.parse(argc, argv);

			ctl::expect_ok(list_called);
		});

	});

	describe("Command overloads", []{
		it("options", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"-a",
				"-b",
				"--cc",
				"-d",
				"-e",
				"--ff",
				"-A",
				"-B",
				"--CC",
				"-D",
				"-E",
				"--FF"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto A = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto d = false;
			auto D = false;
			auto e = false;
			auto E = false;
			auto f = false;
			auto F = false;
			auto desc = "Description"s;

			auto p = args::parser{};

			p.command("cmd")
				.option("-a", &a)
				.option("-A", desc, &A)
				.option("-b", "--bb", &b)
				.option("-B", "--BB", desc, &B)
				.option("-c", "--cc", &c)
				.option("-C", "--CC", desc, &C)
				.option(args::required, "-d", &d)
				.option(args::required, "-D", desc, &D)
				.option(args::required, "-e", "--ee", &e)
				.option(args::required, "-E", "--EE", desc, &E)
				.option(args::required, "-f", "--ff", &f)
				.option(args::required, "-F", "--FF", desc, &F);

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(d, true);
			ctl::expect_equal(e, true);
			ctl::expect_equal(f, true);
			ctl::expect_equal(A, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(C, true);
			ctl::expect_equal(D, true);
			ctl::expect_equal(E, true);
			ctl::expect_equal(F, true);
		});

		it("options handlers", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"-a",
				"-b",
				"--cc",
				"-d",
				"-e",
				"--ff",
				"-A",
				"-B",
				"--CC",
				"-D",
				"-E",
				"--FF"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto A = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto d = false;
			auto D = false;
			auto e = false;
			auto E = false;
			auto f = false;
			auto F = false;
			auto desc = "Description"s;

			auto p = args::parser{};

			p.command("cmd")
				.option<bool>("-a", [&](auto v) { a = v; })
				.option<bool>("-A", desc, [&](auto v) { A = v; })
				.option<bool>("-b", "--bb", [&](auto v) { b = v; })
				.option<bool>("-B", "--BB", desc, [&](auto v) { B = v; })
				.option<bool>("-c", "--cc", [&](auto v) { c = v; })
				.option<bool>("-C", "--CC", desc, [&](auto v) { C = v; })
				.option<bool>(args::required, "-d", [&](auto v) { d = v; })
				.option<bool>(args::required, "-D", desc, [&](auto v) { D = v; })
				.option<bool>(args::required, "-e", "--ee", [&](auto v) { e = v; })
				.option<bool>(args::required, "-E", "--EE", desc, [&](auto v) { E = v; })
				.option<bool>(args::required, "-f", "--ff", [&](auto v) { f = v; })
				.option<bool>(args::required, "-F", "--FF", desc, [&](auto v) { F = v; });

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(d, true);
			ctl::expect_equal(e, true);
			ctl::expect_equal(f, true);
			ctl::expect_equal(A, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(C, true);
			ctl::expect_equal(D, true);
			ctl::expect_equal(E, true);
			ctl::expect_equal(F, true);
		});

		it("args", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"true",
				"true",
				"true",
				"true",
				"true"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto desc = "Description"s;

			auto p = args::parser{};

			p.command("cmd")
				.arg(&a)
				.arg("b", &b)
				.arg("B", desc, &B)
				.arg(args::required, "c", &c)
				.arg(args::required, "C", desc, &C);

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(C, true);
		});

		it("args handlers", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"true",
				"true",
				"true",
				"true",
				"true"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto a = false;
			auto b = false;
			auto B = false;
			auto c = false;
			auto C = false;
			auto desc = "Description"s;

			auto p = args::parser{};

			p.command("cmd")
				.arg<bool>([&](auto v) { a = v; })
				.arg<bool>("b", [&](auto v) { b = v; })
				.arg<bool>("B", desc, [&](auto v) { B = v; })
				.arg<bool>(args::required, "c", [&](auto v) { c = v; })
				.arg<bool>(args::required, "C", desc, [&](auto v) { C = v; });

			p.parse(argc, argv);

			ctl::expect_equal(a, true);
			ctl::expect_equal(b, true);
			ctl::expect_equal(B, true);
			ctl::expect_equal(c, true);
			ctl::expect_equal(C, true);
		});

		it("rest", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest(&r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest handler", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest<bool>([&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest("rest", &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name, description", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest("rest", desc, &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name, handler", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest<bool>("rest", [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest name, description, handler", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest<bool>("rest", desc, [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest(args::required, "rest", &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name, description", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest(args::required, "rest", desc, &r);

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name, handler", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest<bool>(args::required, "rest", [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});

		it("rest required, name, handler, description", []{
			const char* argv[] = {
				"exec",
				"cmd",
				"0",
				"1"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto desc = "Description"s;
			auto r = std::vector<int>{};
			auto p = args::parser{};

			p.command("cmd")
				.rest<bool>(args::required, "rest", desc, [&](auto v) { r.push_back(v); });

			p.parse(argc, argv);

			ctl::expect_equal(r, {0, 1});
		});
	});

	describe("Mixed", []{
		it("exec -s=on --long=str", []{
			const char* argv[] = {
				"exec",
				"-s=on",
				"--long=str"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto l = ""s;
			args::options options = {
				{"-s", &s},
				{"--long", &l}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
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
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto l = ""s;
			args::options options = {
				{"-s", &s},
				{"--long", &l}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
			ctl::expect_equal(l, "str"s);
		});

		it("exec -s1 --long str", []{
			const char* argv[] = {
				"exec",
				"-s1",
				"--long",
				"str"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto l = ""s;
			args::options options = {
				{"-s", &s},
				{"--long", &l}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
			ctl::expect_equal(l, "str"s);
		});

		it("exec -s=1 --long=str", []{
			const char* argv[] = {
				"exec",
				"-s=1",
				"--long=str"
			};
			const int argc = std::distance(std::begin(argv), std::end(argv));

			auto s = false;
			auto l = ""s;
			args::options options = {
				{"-s", &s},
				{"--long", &l}
			};

			args::parse(argc, argv, options);

			ctl::expect_equal(s, true);
			ctl::expect_equal(l, "str"s);
		});
	});
}

