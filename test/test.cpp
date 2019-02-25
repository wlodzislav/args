#include <vector>
#include <list>
#include <iterator>
#include <iostream>

#include "../args.h"

template<typename T>
std::ostream& operator<<(std::ostream &ss, const std::vector<T>& v) {
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
std::ostream& operator<<(std::ostream &ss, const std::list<T>& v) {
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

			ctl::expect_equal(s, std::string("str"));
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
	});
}

