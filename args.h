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

using namespace std::literals;

namespace {
	bool is_short_option(std::string opt) {
		return opt.starts_with('-') && opt != "--" && opt.size() == 2;
	}

	bool is_long_option(std::string opt) {
		return opt.starts_with("--") && opt.size() > 2;
	}

	bool is_non_conventional(std::string opt) {
		return !is_short_option(opt) && !is_long_option(opt) && opt != "--"s;
	}

	bool is_valid_flag_value(std::string value) {
		return value == "1" || value == "0"
			|| value == "true" || value == "false"
			|| value == "yes" || value == "no"
			|| value == "on" || value == "off";
	}

	template<typename T, typename = void>
		struct is_stringstreamable: std::false_type {};

	template<typename T>
		struct is_stringstreamable<T, std::void_t<decltype(std::declval<std::istringstream&>() >> std::declval<T&>())>> : std::true_type {};

	template<typename T>
	std::enable_if_t<is_stringstreamable<T>::value>
	parse_value(std::string value, T* destination) {
		if (!value.empty()) {
			auto ss = std::stringstream{value};
			ss >> *destination;
			if (ss.fail()) {
				throw std::runtime_error("Can't parse \""s + value + "\"."s);
			}
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
				throw std::runtime_error("Value \""s
					+ value
					+ R"(" is not one of "1", "0", "true", "false", "on", "off", "yes", "no".)");
			}
		} else {
			*destination = true;
		}
	}

	template<typename T>
	std::enable_if_t<!is_stringstreamable<T>::value
		&& is_stringstreamable<typename T::value_type>::value
	>
	parse_value(std::string value, T* destination) {
		if (!value.empty()) {
			auto ss = std::stringstream{value};
			typename T::value_type c;
			ss >> c;
			if (ss.fail()) {
				throw std::runtime_error("Can't parse \""s + value + "\"."s);
			}
			destination->insert(std::end(*destination), c);
		}
	}

	template<typename T>
	std::enable_if_t<!is_stringstreamable<T>::value
		&& is_stringstreamable<typename T::key_type>::value
		&& is_stringstreamable<typename T::mapped_type>::value
	>
	parse_value(std::string value, T* destination) {
		auto eq_pos = value.find('=');
		if (eq_pos == std::string::npos) {
			throw std::runtime_error("Value \""s + value
					+ "\" is not key=value pair.");
		}

		auto k_str = value.substr(0, eq_pos);
		typename T::key_type k;
		auto k_stream = std::stringstream{k_str};
		k_stream >> k;
		if (k_stream.fail()) {
			throw std::runtime_error("Can't parse key in pair \""s + value + "\"."s);
		}

		auto v_str = value.substr(eq_pos + 1);
		typename T::mapped_type v;
		auto v_stream = std::stringstream{v_str};
		v_stream >> v;
		if (v_stream.fail()) {
			throw std::runtime_error("Can't parse value in pair \""s + value + "\"."s);
		}

		(*destination)[k] = v;
	}

	template <typename T>
	std::function<void (const std::string&)> create_parse_fun(T* destination) {
		return [=](const std::string& value) { parse_value(value, destination); };
	}

	template <typename T, typename F>
	std::function<void (const std::string&)> create_parse_fun(F handler) {
		return [=](const std::string& value) {
			T destination;
			parse_value(value, &destination);
			handler(destination);
		};
	}
}

namespace args {
	class required_t {};
	const auto required = required_t{};

	struct option {
		std::string short_name;
		std::string long_name;
		std::string non_conventional;
		bool required = false;
		bool is_flag = false;
		std::function<void (const std::string&)> parse_fun;
		bool exists = false;

		option(const option&) = default;

		option(std::string name, std::string short_name, std::string long_name,
				bool required, bool is_flag, std::function<void (const std::string&)> parse_fun)
			: short_name(is_short_option(name) ? name : short_name),
			long_name(is_long_option(name) ? name : long_name),
			non_conventional(is_non_conventional(name) ? name : ""),
			required(required),
			is_flag(is_flag),
			parse_fun(parse_fun) {}

		option(std::string name)
			: option(name, "", "", false, false,
				[](const std::string& value) {}) {}

		template<typename T>
		option(std::string name, T* destination)
			: option(name, "", "", false, std::is_same<T, bool>::value, create_parse_fun(destination)) {}

		template<typename T>
		option(required_t, std::string name, T* destination)
			: option(name, "", "", true, std::is_same<T, bool>::value, create_parse_fun(destination)) {}

		option(std::string short_name, std::string long_name)
			: option("", short_name, long_name, false, false,
				[](const std::string& value) {}) {}

		template<typename T>
		option(std::string short_name, std::string long_name, T* destination)
			: option("", short_name, long_name, false, std::is_same<T, bool>::value, create_parse_fun(destination)) {}

		template<typename T>
		option(required_t, std::string short_name, std::string long_name, T* destination)
			: option("", short_name, long_name, true, std::is_same<T, bool>::value, create_parse_fun(destination)) {}

		void parse(std::string value) {
			this->parse_fun(value);
			this->exists = true;
		}
	};
}

namespace {
	struct arg_internal {
		public:
		std::string name;
		bool required = false;
		std::function<void (const std::string&)> parse_fun;
		bool exists = false;

		arg_internal(const arg_internal&) = default;
		arg_internal() = default;

		template <typename T>
		arg_internal(T parse)
			: parse_fun(parse) {}

		template <typename T>
		arg_internal(std::string name, T parse)
			: name(name),
			parse_fun(parse) {}

		template <typename T>
		arg_internal(args::required_t, std::string name, T parse)
			: name(name),
			required(true),
			parse_fun(parse) {}

		void parse(std::string value) {
			this->parse_fun(value);
			this->exists = true;
		}
	};

	struct command_internal {
		std::vector<args::option> options = {};
		std::vector<arg_internal> args = {};
		arg_internal rest_args = {};
		std::function<void ()> action_fun = 0;
		const std::string name;
		const std::string alias;
		bool* destination = nullptr;

		command_internal(const command_internal&) = default;

		command_internal(const std::string& name)
			: name(name),
			alias("") {}

		command_internal(const std::string& name, bool* destination)
			: name(name),
			destination(destination),
			alias("") {}

		command_internal(const std::string& name, const std::string& alias)
			: name(name),
			alias(alias) {}

		command_internal(const std::string& name, const std::string& alias, bool* destination)
			: name(name),
			destination(destination),
			alias(alias) {}

		template<typename T>
		command_internal& arg(T* destination) {
			this->args.emplace_back(create_parse_fun(destination));
			return *this;
		}

		template<typename T>
		command_internal& arg(std::string name, T* destination) {
			this->args.emplace_back(name, create_parse_fun(destination));
			return *this;
		}

		template<typename T>
		command_internal& arg(args::required_t, std::string name, T* destination) {
			this->args.emplace_back(args::required, name, create_parse_fun(destination));
			return *this;
		}

		template<typename T, typename F>
		command_internal& arg(F handler) {
			this->args.emplace_back(create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		command_internal& arg(std::string name, F handler) {
			this->args.emplace_back(name, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		command_internal& arg(args::required_t, std::string name, F handler) {
			this->args.emplace_back(args::required, name, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T>
		command_internal& rest(T* destination) {
			this->rest_args = {create_parse_fun(destination)};
			return *this;
		}

		template<typename T>
		command_internal& rest(std::string name, T* destination) {
			this->rest_args = {name, create_parse_fun(destination)};
			return *this;
		}

		template<typename T>
		command_internal& rest(args::required_t, std::string name, T* destination) {
			this->rest_args = {args::required, name, create_parse_fun(destination)};
			return *this;
		}

		template<typename T, typename F>
		command_internal& rest(F handler) {
			this->rest_args = {create_parse_fun<T>(handler)};
			return *this;
		}

		template<typename T, typename F>
		command_internal& rest(std::string name, F handler) {
			this->rest_args = {name, create_parse_fun<T>(handler)};
			return *this;
		}

		template<typename T, typename F>
		command_internal& rest(args::required_t, std::string name, F handler) {
			this->rest_args = {args::required, name, create_parse_fun<T>(handler)};
			return *this;
		}

		command_internal& option(args::option option) {
			this->options.push_back(option);
			return *this;
		}

		template<typename T>
		command_internal& option(std::string name, T* destination) {
			this->options.emplace_back(name, destination);
			return *this;
		}

		template<typename T>
		command_internal& option(args::required_t, std::string name, T* destination) {
			this->options.emplace_back(args::required, name, destination);
			return *this;
		}

		template<typename T, typename F>
		command_internal& option(std::string name, F handler) {
			this->options.emplace_back(name, "", "",
					false, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		command_internal& option(args::required_t, std::string name, F handler) {
			this->options.emplace_back(name, "", "",
					true, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T>
		command_internal& option(std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(short_name, long_name, destination);
			return *this;
		}

		template<typename T>
		command_internal& option(args::required_t, std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(args::required, short_name, long_name, destination);
			return *this;
		}

		template<typename T, typename F>
		command_internal& option(std::string short_name, std::string long_name, F handler) {
			this->options.emplace_back("", short_name, long_name,
					false, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		command_internal& option(args::required_t, std::string short_name, std::string long_name, F handler) {
			this->options.emplace_back("", short_name, long_name,
					true, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		command_internal& action(std::function<void (void)> action) {
			this->action_fun = action;
			return *this;
		}
	};

	std::string option_print_name(const args::option& option) {
		auto name = ""s;
		auto first = true;
		if (!option.short_name.empty()) {
			name += option.short_name;
			first = false;
		}
		if (!option.long_name.empty()) {
			if (!first) {
				name += ", "s;
			}
			name += option.long_name;
			first = false;
		}
		if (!option.non_conventional.empty()) {
			if (!first) {
				name += ", "s;
			}
			name += option.non_conventional;
		}
		return name;
	}

	std::string command_print_name(const command_internal& command) {
		auto name = ""s;
		auto first = true;
		if (!command.name.empty()) {
			name += command.name;
			first = false;
		}
		if (!command.alias.empty()) {
			if (!first) {
				name += ", "s;
			}
			name += command.alias;
		}
		return name;
	}

}

namespace args {
	using options = std::vector<option>;

	class invalid_option : public std::runtime_error {
		public:
		const std::string option;

		invalid_option(std::string option)
			: runtime_error("Invalid option \""s + option + "\"."),
			option(option) {}

	};

	class invalid_option_value : public std::runtime_error {
		public:
		const std::string option;
		const std::string value;

		invalid_option_value(std::string option, std::string value, std::string value_what, std::string what = ""s)
			: runtime_error(what.empty() ? "Invalid value for option \""s + option + "\". " + value_what : what),
			option(option),
			value(value) {}
	};

	class invalid_command_option_value : public invalid_option_value {
		public:
		const std::string command;

		invalid_command_option_value(std::string command, std::string option, std::string value, std::string value_what)
			: invalid_option_value(option, value, "Invalid value for command \""s + command + "\" option \""s + option + "\". " + value_what),
			command(command) {}
	};

	class invalid_arg_value : public std::runtime_error {
		public:
		const std::string arg;
		const std::string value;

		invalid_arg_value(std::string arg, std::string value, std::string value_what, std::string what = ""s)
			: runtime_error(what.empty() ? "Invalid value for argument  \""s + arg + "\". " + value_what : what),
			arg(arg),
			value(value) {}
	};

	class invalid_command_arg_value : public invalid_arg_value {
		public:
		const std::string command;

		invalid_command_arg_value(std::string command, std::string arg, std::string value, std::string value_what)
			: invalid_arg_value(arg, value, "Invalid value for command \""s + command + "\" argument  \""s + arg + "\". " + value_what),
			command(command) {}
	};

	class unexpected_arg : public std::runtime_error {
		public:
		const std::string value;

		unexpected_arg(std::string value)
			: runtime_error("Unexpected argument \""s + value + "\"."s),
			value(value) {}
	};

	class missing_arg : public std::runtime_error {
		public:
		const std::string arg;

		missing_arg(std::string arg, std::string what = ""s)
			: runtime_error(what.empty() ? "Argument \""s + arg + "\" is required." : what),
			arg(arg) {}
	};

	class missing_command_arg : public missing_arg {
		public:
		const std::string command;

		missing_command_arg(std::string command, std::string arg)
			: missing_arg(arg, "Command \""s + command + "\" argument \""s + arg + "\" is required."),
			command(command) {}
	};

	class missing_option : public std::runtime_error {
		public:
		const std::string option;

		missing_option(std::string option, std::string what = ""s)
			: runtime_error(what.empty() ? "Option \""s + option + "\" is required." : what),
			option(option) {}
	};

	class missing_command_option : public missing_option {
		public:
		const std::string command;

		missing_command_option(std::string command, std::string option)
			: missing_option(option, "Command \""s + command + "\" option \""s + option + "\" is required."),
			command(command) {}
	};

	class parser {
		private:

		std::vector<option> options = {};
		std::vector<arg_internal> args = {};
		arg_internal rest_args = {};
		std::vector<command_internal> commands = {};

		public:

		template<typename T>
		parser& arg(T* destination) {
			this->args.emplace_back(create_parse_fun(destination));
			return *this;
		}

		template<typename T>
		parser& arg(std::string name, T* destination) {
			this->args.emplace_back(name, create_parse_fun(destination));
			return *this;
		}

		template<typename T>
		parser& arg(required_t, std::string name, T* destination) {
			this->args.emplace_back(required, name, create_parse_fun(destination));
			return *this;
		}

		template<typename T, typename F>
		parser& arg(F handler) {
			this->args.emplace_back(create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		parser& arg(std::string name, F handler) {
			this->args.emplace_back(name, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		parser& arg(required_t, std::string name, F handler) {
			this->args.emplace_back(required, name, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T>
		parser& rest(T* destination) {
			this->rest_args = {create_parse_fun(destination)};
			return *this;
		}

		template<typename T>
		parser& rest(std::string name, T* destination) {
			this->rest_args = {name, create_parse_fun(destination)};
			return *this;
		}

		template<typename T>
		parser& rest(required_t, std::string name, T* destination) {
			this->rest_args = {required, name, create_parse_fun(destination)};
			return *this;
		}

		template<typename T, typename F>
		parser& rest(F handler) {
			this->rest_args = {create_parse_fun<T>(handler)};
			return *this;
		}

		template<typename T, typename F>
		parser& rest(std::string name, F handler) {
			this->rest_args = {name, create_parse_fun<T>(handler)};
			return *this;
		}

		template<typename T, typename F>
		parser& rest(required_t, std::string name, F handler) {
			this->rest_args = {required, name, create_parse_fun<T>(handler)};
			return *this;
		}

		parser& option(option option) {
			this->options.push_back(option);
			return *this;
		}

		template<typename T>
		parser& option(std::string name, T* destination) {
			this->options.emplace_back(name, destination);
			return *this;
		}

		template<typename T>
		parser& option(required_t, std::string name, T* destination) {
			this->options.emplace_back(required, name, destination);
			return *this;
		}

		template<typename T, typename F>
		parser& option(std::string name, F handler) {
			this->options.emplace_back(name, "", "",
					false, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		parser& option(required_t, std::string name, F handler) {
			this->options.emplace_back(name, "", "",
					true, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T>
		parser& option(std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(short_name, long_name, destination);
			return *this;
		}

		template<typename T>
		parser& option(required_t, std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(required, short_name, long_name, destination);
			return *this;
		}

		template<typename T, typename F>
		parser& option(std::string short_name, std::string long_name, F handler) {
			this->options.emplace_back("", short_name, long_name,
					false, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		template<typename T, typename F>
		parser& option(required_t, std::string short_name, std::string long_name, F handler) {
			this->options.emplace_back("", short_name, long_name,
					true, std::is_same<T, bool>::value, create_parse_fun<T>(handler));
			return *this;
		}

		command_internal& command(std::string name) {
			return this->commands.emplace_back(name);
		}

		command_internal& command(std::string name, std::string alias) {
			return this->commands.emplace_back(name, alias);
		}

		command_internal& command(std::string name, bool* destination) {
			return this->commands.emplace_back(name, destination);
		}

		command_internal& command(std::string name, std::string alias, bool* destination) {
			return this->commands.emplace_back(name, alias, destination);
		}

		void parse(int argc, const char** argv) {
			auto args = std::vector<std::string>{};
			for (auto arg = argv + 1; arg < argv + argc; arg++) {
				args.emplace_back(*arg);
			}

			auto command_it = std::end(this->commands);
			auto find_option_if = [&](const std::function<bool (args::option o)>& pred) {
				if (command_it != std::end(this->commands)) {
					auto command_option_it = std::find_if(std::begin(command_it->options), std::end(command_it->options), pred);
					if (command_option_it != std::end(command_it->options)) {
						return command_option_it;
					}
				}
				auto global_option_it = std::find_if(std::begin(this->options), std::end(this->options), pred);
				if (global_option_it != std::end(this->options)) {
					return global_option_it;
				} else {
					return std::end(this->options);
				}
			};

			auto args_only = false;
			auto arg_index = 0;
			auto command_arg_index = 0;
			for (auto arg = std::begin(args); arg != std::end(args); arg++) {
				if (*arg == "--"s) {
					args_only = true;
					continue;
				}

				if (!args_only) {
					auto option_it = find_option_if([&](auto o) {
						return (!o.short_name.empty() && arg->starts_with(o.short_name))
							|| (!o.long_name.empty() && arg->starts_with(o.long_name))
							|| (!o.non_conventional.empty() && arg->starts_with(o.non_conventional));
					});

					if (option_it == std::end(this->options) && arg->starts_with("--no-")) {
						auto name = "--"s + arg->substr(5);
						option_it = find_option_if([&](auto o) {
							return !o.long_name.empty() && o.long_name == name;
						});

						if (option_it == std::end(this->options) || !option_it->is_flag) {
							option_it = std::end(this->options);
						}
					}

					if (option_it == std::end(this->options) && arg->starts_with('-')) {
						throw invalid_option{*arg};
					}

					if (option_it != std::end(this->options)) {
						/*
						auto is_command_option = command_it != std::end(this->commands)
							&& std::find(std::begin(command_it->options), std::end(command_it->options), *option_it) != std::end(command_it->options);
							*/

						if (*arg == option_it->short_name || *arg == option_it->long_name
								|| *arg == option_it->non_conventional) {

							if (option_it->is_flag) {
								auto next = std::next(arg);
								if (next != std::end(args) && is_valid_flag_value(*next)) {
									try {
										option_it->parse(*next);
									} catch (const std::runtime_error& err) {
										throw invalid_option_value{*arg, *next, err.what()};
									}
									arg++;
								} else {
									option_it->parse("1");
								}
							} else {
								auto next = std::next(arg);
								if (next != std::end(args) && !next->starts_with("-")) {
									try {
										option_it->parse(*next);
									} catch (const std::runtime_error& err) {
										throw invalid_option_value{*arg, *next, err.what()};
									}
									arg++;
								} else {
									auto name = *arg;
									throw invalid_option_value{name, "", "Value is empty."};
								}
							}
						} else if ((!option_it->short_name.empty() && arg->starts_with(option_it->short_name + "="))
								|| (!option_it->long_name.empty() && arg->starts_with(option_it->long_name + "="))
								|| (!option_it->non_conventional.empty() && arg->starts_with(option_it->non_conventional + "="))) {

							auto value = arg->substr(arg->find("=") + 1);
							try {
								option_it->parse(value);
							} catch (const std::runtime_error& err) {
								auto name = arg->substr(0, arg->find("="));
								throw invalid_option_value{name, value, err.what()};
							}
						} else if (!option_it->short_name.empty() && arg->starts_with(option_it->short_name)
								&& option_it->is_flag) {

							auto is_short_grouped = std::all_of(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
								auto name = "-"s + c;
								auto option_it = find_option_if([&](auto o) {
									return o.short_name == name;
								});
								return option_it != std::end(this->options) && option_it->is_flag;
							});
							if (is_short_grouped) {
								std::for_each(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
									auto name = "-"s + c;
									auto option_it = find_option_if([&](auto o) {
										return o.short_name == name;
									});
									option_it->parse("1");
								});
							} else if (is_valid_flag_value(arg->substr(2))) {
								option_it->parse(arg->substr(2));
							} else {
								throw invalid_option{*arg};
							}
						} else if (!option_it->short_name.empty() && arg->starts_with(option_it->short_name)
								&& !option_it->is_flag) {

							auto value = arg->substr(2);
							try {
								option_it->parse(value);
							} catch (const std::runtime_error& err) {
								auto name = arg->substr(0, 2);
								throw invalid_option_value{name, value, err.what()};
							}
						} else if (arg->starts_with("--no-")) {
							option_it->parse("0");
						}

						continue;
					}
				}

				if (!args_only && command_it == std::end(this->commands)) {
					auto prefix_it = std::find_if(std::begin(this->commands), std::end(this->commands), [&](auto c) {
						return arg->starts_with(c.name) || arg->starts_with(c.alias);
					});
					if (prefix_it != std::end(this->commands)) {
						auto possible_command = *arg;
						auto next = arg;
						while (possible_command.size() <= prefix_it->name.size()
								|| possible_command.size() <= prefix_it->alias.size()) {
							if (prefix_it->name == possible_command || prefix_it->alias == possible_command) {
								command_it = prefix_it;
								arg = next;
								break;
							}
							next = std::next(next);
							if (next != std::end(args) && !next->starts_with("-")) {
								possible_command += " "s + *next;
							} else {
								break;
							}
						}
					}

					if (command_it != std::end(this->commands)) {
						if (command_it->destination) {
							*command_it->destination = true;
						}
						continue;
					}
				}

				if (command_it != std::end(this->commands)) {
					if (command_it->args.size() > command_arg_index) {
						try {
							command_it->args[command_arg_index].parse(*arg);
						} catch (const std::runtime_error& err) {
							auto name = command_it->args[command_arg_index].name;
							throw invalid_arg_value{name, *arg, err.what()};
						}
						command_arg_index++;
						continue;
					} else if (command_it->rest_args.parse_fun) {
						try {
							command_it->rest_args.parse(*arg);
						} catch (const std::runtime_error& err) {
							auto name = command_it->rest_args.name;
							throw invalid_arg_value{name, *arg, err.what()};
						}
						continue;
					}
				}

				if (this->args.size() > arg_index) {
					try {
						this->args[arg_index].parse(*arg);
					} catch (const std::runtime_error& err) {
						auto name = this->args[arg_index].name;
						throw invalid_arg_value{name, *arg, err.what()};
					}
					arg_index++;
				} else if (this->rest_args.parse_fun) {
					try {
						this->rest_args.parse(*arg);
					} catch (const std::runtime_error& err) {
						auto name = this->rest_args.name;
						throw invalid_arg_value{name, *arg, err.what()};
					}
				} else {
					throw unexpected_arg{*arg};
				}
			}

			auto missing_option_it = std::find_if(std::begin(this->options), std::end(this->options), [](auto o) {
				return o.required && !o.exists;
			});

			if (missing_option_it != std::end(this->options)) {
				throw missing_option{option_print_name(*missing_option_it)};
			}

			auto missing_arg_it = std::find_if(std::begin(this->args), std::end(this->args), [](auto a) {
				return a.required && !a.exists;
			});

			if (missing_arg_it != std::end(this->args)) {
				throw missing_arg{missing_arg_it->name};
			}

			if (this->rest_args.parse_fun && this->rest_args.required && !this->rest_args.exists) {
				throw missing_arg{this->rest_args.name};
			}

			if (command_it != std::end(this->commands)) {
				auto missing_command_option_it = std::find_if(std::begin(command_it->options), std::end(command_it->options), [](auto o) {
					return o.required && !o.exists;
				});

				if (missing_command_option_it != std::end(command_it->options)) {
					throw missing_command_option{command_print_name(*command_it), option_print_name(*missing_command_option_it)};
				}

				if (command_it->action_fun) {
					command_it->action_fun();
				}

				auto missing_command_arg_it = std::find_if(std::begin(command_it->args), std::end(command_it->args), [](auto a) {
					return a.required && !a.exists;
				});

				if (missing_command_arg_it != std::end(command_it->args)) {
					throw missing_command_arg{command_print_name(*command_it), missing_command_arg_it->name};
				}

				if (command_it->rest_args.parse_fun && command_it->rest_args.required && !command_it->rest_args.exists) {
					throw missing_command_arg{command_print_name(*command_it), command_it->rest_args.name};
				}
			}

		}
	};

	void parse(int argc, const char** argv, const std::vector<args::option>& options) {
		auto p = parser{};
		for (auto& option : options) {
			p.option(option);
		}
		p.parse(argc, argv);
	}
}

#endif
