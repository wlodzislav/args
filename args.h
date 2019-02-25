/*
	Modern C++ Command Line Argumets Library

	Copyright (c) 2019 Vladislav Kaminsky(wlodzislav@outlook.com)

	MIT License <https://github.com/wlodzislav/args/blob/master/LICENSE>
*/

#ifndef ARGS_H
#define ARGS_H

#include <type_traits>
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

namespace {
	bool is_option(std::string opt) {
		return opt[0] == '-';
	}

	bool is_short_option(std::string opt) {
		return opt[0] == '-' && opt[1] != '-' && opt.length() == 2;
	}

	bool is_short_option_width_eq_sign_value(std::string opt) {
		return opt[0] == '-' && opt[1] != '-' && opt[2] == '=';
	}

	bool is_short_grouped_or_with_value(std::string opt) {
		return opt[0] == '-' && opt[1] != '-' && opt.length() > 2;
	}

	bool is_long_option(std::string opt) {
		bool has_eq = (opt.find('=') != std::string::npos);
		return opt[0] == '-' && opt[1] == '-' && opt.length() > 2 && !has_eq;
	}

	bool is_long_option_width_eq_sign_value(std::string opt) {
		bool has_eq = (opt.find('=') != std::string::npos);
		return opt[0] == '-' && opt[1] == '-' && opt.length() > 2 && has_eq;
	}

	template<typename T, typename = void>
		struct is_stringstreamable: std::false_type {};

	template<typename T>
		struct is_stringstreamable<T, std::void_t<decltype(std::declval<std::istringstream&>() >> std::declval<T&>())>> : std::true_type {};

	template<typename T>
	std::enable_if_t<is_stringstreamable<T>::value>
	parse_value(std::string value, T* destination) {
		if (!value.empty()) {
			std::stringstream stream(value);
			stream >> *destination;
		}
	}

	template<>
	void parse_value(std::string value, bool* destination) {
		if (!value.empty()) {
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

	template<typename T>
	std::enable_if_t<!is_stringstreamable<T>::value
		&& is_stringstreamable<typename T::value_type>::value
		&& std::is_same<
			decltype(std::declval<T>().insert(std::declval<T>().end(), std::declval<typename T::value_type>())),
			typename T::iterator
		>::value
	>
	parse_value(std::string value, T* destination) {
		if (!value.empty()) {
			std::stringstream stream(value);
			typename T::value_type c;
			stream >> c;
			destination->insert(destination->end(), c);
		}
	}

	template<typename T>
	std::enable_if_t<!is_stringstreamable<T>::value
		&& is_stringstreamable<typename T::key_type>::value
		&& is_stringstreamable<typename T::mapped_type>::value
	>
	parse_value(std::string value, T* destination) {
		if (!value.empty()) {
			auto eq_pos = value.find('=');
			if (eq_pos == std::string::npos) {
				throw std::invalid_argument(
					std::string("Invalid command line option. Value \"") +
					value + '"' + "is not key=value pair.");
			}

			auto k_str = value.substr(0, eq_pos);
			typename T::key_type k;
			std::stringstream k_stream(k_str);
			k_stream >> k;

			auto v_str = value.substr(eq_pos + 1);
			typename T::mapped_type v;
			std::stringstream v_stream(v_str);
			v_stream >> v;

			(*destination)[k] = v;
		}
	}
}

namespace args {
	struct option {
		option(const args::option&) = default;

		template<typename Type>
		option(std::string name, Type* destination)
			: short_name(is_short_option(name) ? name : ""),
			long_name(is_long_option(name) ? name : ""),
			parse([=](std::string value) { parse_value(value, destination);}) {}

		template<typename Type>
		option(std::string short_name, std::string long_name, Type* destination)
			: short_name(is_short_option(short_name) ? short_name : ""),
			long_name(is_long_option(long_name) ? long_name : ""),
			parse([=](std::string value) { parse_value(value, destination);}) {}

		const std::function<void (std::string)> parse;
		const std::string short_name;
		const std::string long_name;
	};

	void parse(int argc, const char** argv, const std::vector<args::option>& options) {
		auto args = std::vector<std::string>{};
		for (const char** arg = argv + 1; arg < argv + argc; arg++) {
			args.push_back(std::string(*arg));
		}

		std::map<std::string, std::function<void (std::string)>> global_options;
		for (auto&& option : options) {
			if (!option.short_name.empty()) {
				global_options[option.short_name] = option.parse;
			}
			if (!option.long_name.empty()) {
				global_options[option.long_name] = option.parse;
			}
		}

		for (auto arg = std::begin(args); arg != std::end(args); arg++) {
			std::string option;
			std::string value;
			auto is_short_grouped = false;
			if (is_short_option(*arg)) {
				option = *arg;
				auto next = std::next(arg);
				if (next != std::end(args) && !is_option(*next)) {
					value = *next;
					arg++;
				}
			} else if (is_short_option_width_eq_sign_value(*arg)) {
				option = arg->substr(0, 2);
				value = arg->substr(3);
			} else if (is_short_grouped_or_with_value(*arg)) {
				is_short_grouped = std::all_of(std::begin(*arg) + 1, std::end(*arg), [&](auto c) -> bool {
					return global_options.find(std::string("-") + c) != global_options.end();
				});
				if (!is_short_grouped) {
					option = arg->substr(0, 2);
					value = arg->substr(2);
				}
			} else if (is_long_option(*arg)) {
				option = *arg;
				auto next = std::next(arg);
				if( next != std::end(args) && !is_option(*next) ) {
					value = *next;
					arg++;
				}
			}
			else if (is_long_option_width_eq_sign_value(*arg)) {
				std::string::size_type eq_pos = std::string(*arg).find("=");
				option = arg->substr(0, eq_pos);
				value = arg->substr(eq_pos + 1);
			}

			if (is_short_grouped) {
				std::for_each(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
					auto option = std::string("-") + c;
					auto&& it = global_options.find(option);
					it->second(value);
				});
			} else {
				auto&& it = global_options.find(option);
				if(it != global_options.end()) {
					it->second(value);
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
