/*
	Modern C++ Command Line Argumets Library

	Copyright (c) 2019 Vladislav Kaminsky(wlodzislav@outlook.com)

	MIT License <https://github.com/wlodzislav/args/blob/master/LICENSE>
*/

#ifndef ARGS_H
#define ARGS_H

#include <sstream>
#include <functional>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include <stdexcept>
#include <iostream>
#include <iterator>
#include <initializer_list>

#define INSPECT(exp) std::clog << "INSPECT " << #exp << " = " << exp << " on line: " << __LINE__ << '\n'
#define MARK std::clog << "MARK on line: " << __LINE__ << '\n'

namespace {
	constexpr bool is_option(const char* opt) {
		return opt[0] == '-';
	}

	constexpr bool is_posible_short_option(const char* opt) {
		return opt[0] == '-' && opt[1] != '-';
	}

	constexpr bool is_short_option(const char* opt) {
		return opt[0] == '-' && opt[1] != '-' && opt[2] == '\0';
	}

	constexpr bool is_short_option_width_eq_sign_value(const char* opt) {
		return opt[0] == '-' && opt[1] != '-' && opt[2] == '=';
	}

	constexpr bool is_short_grouped_or_with_value(const char* opt) {
		return opt[0] == '-' && opt[1] != '-' && opt[2] != '\0';
	}

	constexpr bool is_short_grouped(const char* opt) {
		return false;
	}

	bool is_long_option(const char* opt) {
		std::string option(opt);
		bool has_eq = (option.find("=") != std::string::npos);
		return opt[0] == '-' && opt[1] == '-' && opt[2] != '\0' && !has_eq;
	}

	bool is_long_option_width_eq_sign_value(const char* opt) {
		std::string option(opt);
		bool has_eq = (option.find("=") != std::string::npos);
		return opt[0] == '-' && opt[1] == '-' && opt[2] != '\0' && has_eq;
	}

	constexpr bool a = is_option("-a");

	template<typename Type>
	void parse_value(const char* value_c, Type* destination) {
		if (*value_c != '\0') {
			std::stringstream value(value_c);
			value >> *destination;
		}
	}

	template<>
	void parse_value(const char* value_c, bool* destination) {
		if (*value_c != '\0') {
			std::string value(value_c);
			if (value == "1" || value == "true" ||
				value == "on" || value == "yes") {
				*destination = true;
			} else if (value == "0" || value == "false" ||
				value == "off" || value == "no") {
				*destination = false;
			} else {
				throw std::invalid_argument(
					std::string("Invalid command line option. Value \"") +
					value + '"' + R"( is not one of "1", "0", "true", "false", "on", "off", "yes", "no".)");
			}
		}
		else {
			*destination = true;
		}
	}
}

namespace args {
	struct option {
		option(const args::option&) = default;

		template<typename Type>
		option(const char* name, Type* destination)
			: short_name(is_short_option(name) ? name : ""),
			long_name(is_long_option(name) ? name : ""),
			parse([=](const char* value) { parse_value(value, destination);}) {}

		template<typename Type>
		option(const char* short_name, const char* long_name, Type* destination)
			: short_name(is_short_option(short_name) ? short_name : ""),
			long_name(is_long_option(long_name) ? long_name : ""),
			parse([=](const char* value) { parse_value(value, destination);}) {}

		/*
		template<typename Type>
		option(const char* name, Type* destination, const char* description);

		template<typename Type>
		option(const char* short_name, const char* long_name, Type* destination, const char* description);
		*/

		const std::function<void (const char*)> parse;
		const std::string short_name;
		const std::string long_name;
	};

	void parse(int argc, const char** argv, const std::vector<args::option>& options) {
		std::map<std::string, std::function<void (const char*)>> global_options;
		for (auto&& option : options) {
			if (!option.short_name.empty()) {
				global_options[option.short_name] = option.parse;
			}
			if (!option.long_name.empty()) {
				global_options[option.long_name] = option.parse;
			}
		}
		for (const char** arg = argv + 1; arg < argv + argc; ++arg) {
			std::string option;
			std::string value;
			auto str_arg = std::string{*arg};
			auto is_short_grouped = false;
			//INSPECT(*arg);
			if (is_short_option(*arg)) {
				option = std::string(*arg);
				auto&& next = std::next(arg);
				if (next != argv + argc && !is_option(*next)) {
					value = std::string(*next);
					++arg;
				}
			} else if (is_short_option_width_eq_sign_value(*arg)) {
				option = std::string(*arg, 2);
				value = std::string(*arg + 3);
			} else if (is_short_grouped_or_with_value(*arg)) {
				auto str_arg = std::string{*arg};
				is_short_grouped = std::all_of(str_arg.begin() + 1, str_arg.end(), [&](auto c) -> bool {
					return global_options.find(std::string("-") + c) != global_options.end();
				});
				if (is_short_grouped) {
				} else {
					option = std::string(*arg, 2);
					value = std::string(*arg + 2);
				}
			} else if (is_long_option(*arg)) {
				option = std::string(*arg);
				auto&& next = std::next(arg);
				if( next != argv + argc && !is_option(*next) ) {
					value = std::string(*next);
					++arg;
				}
			}
			else if (is_long_option_width_eq_sign_value(*arg)) {
				std::string::size_type eq_pos = std::string(*arg).find("=");
				option = std::string(*arg, eq_pos);
				value = std::string(*arg + eq_pos + 1);
			}

			if (is_short_grouped) {
				std::for_each(str_arg.begin() + 1, str_arg.end(), [&](auto c) {
					auto option = std::string("-") + c;
					auto&& it = global_options.find(option);
					it->second(value.c_str());
				});
			} else {
				auto&& it = global_options.find(option);
				if(it != global_options.end()) {
					it->second(value.c_str());
				} else {
					throw std::invalid_argument(std::string("Invalid command line option \"") + *arg + "\".");
				}
			}
		}
	}

	void parse(int argc, const char** argv, const std::initializer_list<args::option>& options) {
		std::vector<option> options_vec;
		for (auto&& e : options) {
			options_vec.push_back(e);
		}
		parse(argc, argv, options_vec);
	}
}

#endif
