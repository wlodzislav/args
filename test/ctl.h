/*
	CTL - C++ Testing Library

	Copyright (c) 2019 Vladislav Kaminsky(wlodzislav@ya.ru)

	MIT License <https://github.com/wlodzislav/ctl/blob/master/LICENSE>
*/

#ifndef CTL_H
#define CTL_H

#include <functional>
#include <memory>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

using namespace std::string_literals;

namespace ctl {
	class reporter {
		public:
			virtual void before() {}
			virtual void after() {};
			virtual void suite_begin(const std::string& description) {};
			virtual void suite_end(const std::string& description) {};
			virtual void pending_suite(const std::string& description) {};
			virtual void completed_test(const std::string& description) {};
			virtual void failed_test(const std::string& description, const std::string& error_text) {};
			virtual void pending_test(const std::string& description) {};
			virtual ~reporter() = default;
	};
}

namespace {
	auto grey_esc = "\x1B[90m"s;
	auto green_esc = "\x1B[32m"s;
	auto red_esc = "\x1B[31m"s;
	auto cyan_esc = "\x1B[36m"s;
	auto reset_esc = "\x1B[39m"s;

	std::string grey(const std::string& text) {
		return grey_esc + text + reset_esc;
	}

	std::string green(const std::string& text) {
		return green_esc + text + reset_esc;
	}

	std::string red(const std::string& text) {
		return red_esc + text + reset_esc;
	}

	std::string cyan(const std::string& text) {
		return cyan_esc + text + reset_esc;
	}
}

namespace ctl {
	class spec_reporter : public reporter {
	public:
		void after() {
			std::cout << std::endl;
			if (this->completed > 0) {
				std::cout << padding() << green(std::to_string(this->completed) + " " + (this->completed == 1 ? "test" : "tests") + " completed") << std::endl;
			}
			if (this->failed > 0) {
				std::cout << padding() << red(std::to_string(this->failed) + " " + (this->failed == 1 ? "test" : "tests") + " failed") << std::endl;
			}
			if (this->pending > 0) {
				std::cout << padding() << cyan(std::to_string(this->pending) + " " + (this->pending == 1 ? "test" : "tests") + " pending") << std::endl;
			}
		}

		void suite_begin(const std::string& description) {
			std::cout << padding() << description << std::endl;
			indentation++;
		}

		void suite_end(const std::string& description) {
			indentation--;
		}

		void pending_suite(const std::string& description) {
			std::cout << padding() << cyan("- ") << cyan(description) << std::endl;
		}

		void completed_test(const std::string& description) {
			completed++;
			std::cout << padding() << green("✓ ") << grey(description) << std::endl;
		}

		void failed_test(const std::string& description, const std::string& error_text) {
			failed++;
			std::cout << padding() << red("✖ ") << red(description) << std::endl;
			std::cout << padding() << "  " << red(error_text) << std::endl;
		}

		void pending_test(const std::string& description) {
			pending++;
			std::cout << padding() << cyan("- ") << cyan(description) << std::endl;
		}

	private:
		int indentation = 0;
		int completed = 0;
		int failed = 0;
		int pending = 0;

		std::string padding() {
			return std::string(indentation * 2, ' ');
		}
	};

	class short_reporter : public reporter {
	public:
		void after() {
			std::cout << std::endl;
			if (this->completed > 0) {
				std::cout << green(std::to_string(this->completed) + " " + (this->completed == 1 ? "test" : "tests") + " completed") << std::endl;
			}
			if (this->failed > 0) {
				std::cout << red(std::to_string(this->failed) + " " + (this->failed == 1 ? "test" : "tests") + " failed") << std::endl;
			}
			if (this->pending > 0) {
				std::cout << cyan(std::to_string(this->pending) + " " + (this->pending == 1 ? "test" : "tests") + " pending") << std::endl;
			}
		}
		void completed_test(const std::string& description) {
			completed++;
		}

		void failed_test(const std::string& description, const std::string& error_text) {
			failed++;
			std::cout << red("✖ ") << red(description) << std::endl;
			std::cout << "  " << red(error_text);
			std::cout << std::endl;
		}

		void pending_test(const std::string& description) {
			pending++;
		}

	private:
		int completed = 0;
		int failed = 0;
		int pending = 0;
	};

	 auto current_reporter = std::unique_ptr<ctl::reporter>{new ctl::spec_reporter()};
}

namespace {
	auto nop = []{};
	auto before_hook = std::function<void (void)>{nop};
	auto after_hook = std::function<void (void)>{nop};
	auto before_each_hook = std::function<void (void)>{nop};
	auto after_each_hook = std::function<void (void)>{nop};
	auto first_call = true;
	auto first_test = true;

	void do_after_tests() {
		after_hook();
		ctl::current_reporter->after();
	}

	void do_before_tests() {
		if (first_call) {
			first_call = false;
			before_hook();
			ctl::current_reporter->before();
			std::atexit(do_after_tests);
		}
	}

}

namespace ctl {
	void describe(const std::string& description, std::function<void (void)> suit) {
		do_before_tests();

		auto prev_before_each_hook = before_each_hook;
		before_each_hook = nop;
		auto prev_after_each_hook = after_each_hook;
		after_each_hook = nop;
		auto prev_before_hook = before_hook;
		before_hook = nop;
		auto prev_after_hook = after_hook;
		after_hook = nop;

		auto prev_first_test = first_test;
		first_test = true;
		current_reporter->suite_begin(description);
		suit();
		after_hook();
		current_reporter->suite_end(description);
		first_test = prev_first_test;

		before_each_hook = prev_before_each_hook;
		after_each_hook = prev_after_each_hook;
		before_hook = prev_before_hook;
		after_hook = prev_after_hook;
	}

	void describe(const std::string& description) {
		do_before_tests();
		current_reporter->pending_suite(description);
	}

	void it(const std::string& description, std::function<void (void)> test) {
		if (first_test) {
			first_test = false;
			before_hook();
		}
		do_before_tests();
		before_each_hook();
		try {
			test();
			current_reporter->completed_test(description);
		}
		catch (std::exception &e) {
			current_reporter->failed_test(description, e.what());
		}
		after_each_hook();
	}

	void it(const std::string& description) {
		do_before_tests();
		current_reporter->pending_test(description);
	}

	void before(std::function<void (void)> hook) {
		before_hook = hook;
	}

	void after(std::function<void (void)> hook) {
		after_hook = hook;
	}

	void before_each(std::function<void (void)> hook) {
		before_each_hook = hook;
	}

	void after_each(std::function<void (void)> hook) {
		after_each_hook = hook;
	}

	void expect_ok(bool condition, const std::string& message = "") {
		if(!condition) {
			throw std::runtime_error(std::string("Expect condition to be true. ") + message);
		}
	}

	void expect_fail(bool condition, const std::string& message = "") {
		if(condition) {
			throw std::runtime_error(std::string("Expect condition to be false. ") + message);
		}
	}
 
	template<typename T>
	void expect_equal(const T& actual, const T& expected, const std::string& message = "") {
		if(!(actual == expected)) {
			std::stringstream error;
			error << "Expect \"" << actual << "\" to be equal \"" << expected << "\"";
			throw std::runtime_error(error.str());
		}
	}
}

#endif
