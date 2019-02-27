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
#include <optional>

using namespace std::literals;

namespace {
	bool is_short_option(std::string opt) {
		return opt[0] == '-' && opt[1] != '-' && opt.length() == 2;
	}

	bool is_long_option(std::string opt) {
		bool has_eq = (opt.find('=') != std::string::npos);
		return opt[0] == '-' && opt[1] == '-' && opt.length() > 2 && !has_eq;
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
				throw std::invalid_argument("Invalid command line option. Value \""s + value
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
				throw std::invalid_argument("Invalid command line option. Value \""s + value
						+ "\" is not key=value pair.");
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

	template<typename T>
	std::optional<typename T::value_type> find_by_name(const T& c, const std::string name) {
		auto it = std::find_if(std::begin(c), std::end(c), [&](auto o) {
			return o.short_name == name || o.long_name == name;
		});
		if (it != std::end(c)) {
			return *it;
		} else {
			return {};
		}
	}
}

namespace args {
	struct option {
		const std::function<void (std::string)> parse;
		const std::string short_name;
		const std::string long_name;
		const bool is_flag = false;

		option(const option&) = default;

		template<typename T>
		option(std::string name, T* destination)
			: short_name(is_short_option(name) ? name : ""),
			long_name(is_long_option(name) ? name : ""),
			is_flag(std::is_same<T, bool>::value),
			parse([=](std::string value) { parse_value(value, destination);}) {}

		template<typename T>
		option(std::string short_name, std::string long_name, T* destination)
			: short_name(is_short_option(short_name) ? short_name : ""),
			long_name(is_long_option(long_name) ? long_name : ""),
			is_flag(std::is_same<T, bool>::value),
			parse([=](std::string value) { parse_value(value, destination);}) {}

	};
}

namespace {
	struct command_internal {
		std::vector<args::option> options = {};
		std::vector<std::function<void (std::string)>> positional_args = {};
		std::function<void (std::string)> rest_args = 0;
		std::function<void ()> action_fun = 0;
		const std::string name;
		const std::string alias;

		command_internal(const command_internal&) = default;

		command_internal(std::string name)
			: name(name),
			alias("") {}

		command_internal(std::string name, std::string alias)
			: name(name),
			alias(alias) {}

		template<typename T>
		command_internal& positional(T* destination) {
			positional_args.push_back([=](std::string value) { parse_value(value, destination); });
			return *this;
		}

		template<typename T>
		command_internal& rest(T* destination) {
			rest_args = [=](std::string value) { parse_value(value, destination); };
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
		command_internal& option(std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(short_name, long_name, destination);
			return *this;
		}

		command_internal& action(std::function<void (void)> action) {
			this->action_fun = action;
			return *this;
		}
	};
}

namespace args {
	class parser {
		private:

		std::vector<option> options = {};
		std::vector<std::function<void (std::string)>> positional_args = {};
		std::function<void (std::string)> rest_args = 0;
		std::vector<command_internal> commands = {};

		public:

		template<typename T>
		parser& positional(T* destination) {
			positional_args.push_back([=](std::string value) { parse_value(value, destination); });
			return *this;
		}

		template<typename T>
		parser& rest(T* destination) {
			rest_args = [=](std::string value) { parse_value(value, destination); };
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
		parser& option(std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(short_name, long_name, destination);
			return *this;
		}

		command_internal& command(std::string name) {
			return this->commands.emplace_back(name);
		}

		command_internal& command(std::string name, std::string alias) {
			return this->commands.emplace_back(name, alias);
		}

		void parse(int argc, const char** argv) {
			auto args = std::vector<std::string>{};
			for (const char** arg = argv + 1; arg < argv + argc; arg++) {
				args.push_back(std::string{*arg});
			}

			auto args_only = false;
			auto positional_index = 0;
			auto command_positional_index = 0;
			auto command_it = std::end(this->commands);
			auto find_option_if = [&](std::function<bool (args::option o)> pred) {
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

			for (auto arg = std::begin(args); arg != std::end(args); arg++) {
				if (*arg == "--"s) {
					args_only = true;
					continue;
				}

				if (!args_only && arg->starts_with('-')) {
					auto option_it = find_option_if([&](auto o) {
						return (!o.short_name.empty() && arg->starts_with(o.short_name))
							|| (!o.long_name.empty() && arg->starts_with(o.long_name));
					});

					if (option_it == std::end(this->options)) {
						throw std::invalid_argument("Invalid command line option \""s + *arg + "\".");
					}

					if (*arg == option_it->short_name || *arg == option_it->long_name) {
						if (option_it->is_flag) {
							auto next = std::next(arg);
							if (next != std::end(args) && is_valid_flag_value(*next)) {
								option_it->parse(*next);
								arg++;
							} else {
								option_it->parse("");
							}
						} else {
							auto next = std::next(arg);
							if (next != std::end(args) && !next->starts_with("-")) {
								option_it->parse(*next);
								arg++;
							} else {
								throw std::invalid_argument("Option \""s + *arg + "\" requires value.");
							}
						}
					} else if ((!option_it->short_name.empty() && arg->starts_with(option_it->short_name + "="))
							|| (!option_it->long_name.empty() && arg->starts_with(option_it->long_name + "="))) {

						option_it->parse(arg->substr(arg->find("=") + 1));
					} else if (!option_it->short_name.empty() && arg->starts_with(option_it->short_name)
							&& option_it->is_flag) {

						auto is_short_grouped = std::all_of(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
							auto name = "-"s + c;
							auto option_it = find_option_if([&](auto o) {
								return o.short_name == name;
							});
							return option_it != std::end(this->options);
						});
						if (is_short_grouped) {
							std::for_each(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
								auto name = "-"s + c;
								auto option_it = find_option_if([&](auto o) {
									return o.short_name == name;
								});
								option_it->parse("");
							});
						} else if (is_valid_flag_value(arg->substr(2))) {
							option_it->parse(arg->substr(2));
						}
					} else if (!option_it->short_name.empty() && arg->starts_with(option_it->short_name)
							&& !option_it->is_flag) {

						option_it->parse(arg->substr(2));
					}
				} else {
					if (!args_only && command_it == std::end(this->commands)) {
						auto it = std::find_if(std::begin(this->commands), std::end(this->commands), [&](auto c) {
							return c.name == *arg || c.alias == *arg;
						});
						if (it != std::end(this->commands)) {
							command_it = it;
							continue;
						}
					}

					if (command_it != std::end(this->commands)) {
						if (command_it->positional_args.size() > command_positional_index) {
							command_it->positional_args[command_positional_index](*arg);
							command_positional_index++;
						} else if (command_it->rest_args) {
							command_it->rest_args(*arg);
						} else {
							if (this->positional_args.size() > positional_index) {
								this->positional_args[positional_index](*arg);
								positional_index++;
							} else if (this->rest_args) {
								this->rest_args(*arg);
							}
						}
					} else if (this->positional_args.size() > positional_index) {
						this->positional_args[positional_index](*arg);
						positional_index++;
					} else if (this->rest_args) {
						this->rest_args(*arg);
					} else {
						throw std::invalid_argument("Unexpected argument"s + *arg);
					}
				}
			}

			if (command_it != std::end(this->commands)) {
				if (command_it->action_fun) {
					command_it->action_fun();
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
