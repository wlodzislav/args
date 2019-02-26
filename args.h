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

	template<typename T>
	std::optional<typename T::value_type> find_by_name(const T& c, const std::string name) {
		auto it = std::find_if(std::begin(c), std::end(c), [&](auto o) {
			return o.name == name || o.alias == name;
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
		option(const option&) = default;

		template<typename Type>
		option(std::string name, Type* destination)
			: name(name),
			alias(""),
			parse([=](std::string value) { parse_value(value, destination);}) {}

		template<typename Type>
		option(std::string name, std::string alias, Type* destination)
			: name(name),
			alias(alias),
			parse([=](std::string value) { parse_value(value, destination);}) {}

		const std::function<void (std::string)> parse;
		const std::string name;
		const std::string alias;
	};
}

namespace {
	struct command_internal {
		command_internal(const command_internal&) = default;

		std::vector<args::option> options = {};
		std::vector<std::function<void (std::string)>> positional_args = {};
		std::function<void (std::string)> rest_args = 0;
		std::function<void ()> action_fun = 0;
		const std::string name;
		const std::string alias;

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
		command_internal& option(std::string name, std::string alias, T* destination) {
			this->options.emplace_back(name, alias, destination);
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
		parser& option(std::string name, std::string alias, T* destination) {
			this->options.emplace_back(name, alias, destination);
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
				args.push_back(std::string(*arg));
			}

			auto positional_index = 0;
			auto command_positional_index = 0;
			auto command_it = std::end(this->commands);
			for (auto arg = std::begin(args); arg != std::end(args); arg++) {
				std::string name;
				std::string value;
				auto is_short_grouped = false;
				if (is_option(*arg)) {
					if (is_short_option(*arg)) {
						name = *arg;
						auto next = std::next(arg);
						if (next != std::end(args) && !is_option(*next)) {
							value = *next;
							arg++;
						}
					} else if (is_short_option_width_eq_sign_value(*arg)) {
						name = arg->substr(0, 2);
						value = arg->substr(3);
					} else if (is_short_grouped_or_with_value(*arg)) {
						is_short_grouped = std::all_of(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
							auto name = std::string("-") + c;
							if (command_it != std::end(this->commands)) {
								if (auto c_it = find_by_name(command_it->options, name)) {
									return c_it;
								} else {
									auto g_it = find_by_name(this->options, name);
									return g_it;
								}
							} else {
								auto g_it = find_by_name(this->options, name);
								return g_it;
							}
						});
						if (!is_short_grouped) {
							name = arg->substr(0, 2);
							value = arg->substr(2);
						}
					} else if (is_long_option(*arg)) {
						name = *arg;
						auto next = std::next(arg);
						if( next != std::end(args) && !is_option(*next) ) {
							value = *next;
							arg++;
						}
					}
					else if (is_long_option_width_eq_sign_value(*arg)) {
						std::string::size_type eq_pos = std::string(*arg).find("=");
						name = arg->substr(0, eq_pos);
						value = arg->substr(eq_pos + 1);
					}

					if (is_short_grouped) {
						std::for_each(std::begin(*arg) + 1, std::end(*arg), [&](auto c) {
							auto name = std::string("-") + c;
							if (command_it != std::end(this->commands)) {
								if (auto c_it = find_by_name(command_it->options, name)) {
									c_it->parse(value);
								} else {
									auto g_it = find_by_name(this->options, name);
									g_it->parse(value);
								}
							} else {
								auto g_it = find_by_name(this->options, name);
								g_it->parse(value);
							}
						});
					} else if (!name.empty()) {
						if (command_it != std::end(this->commands)) {
							if (auto c_it = find_by_name(command_it->options, name)) {
								c_it->parse(value);
							} else if(auto g_it = find_by_name(this->options, name)) {
								g_it->parse(value);
							} else {
								throw std::invalid_argument(std::string("Invalid command line option \"") + *arg + "\".");
							}
						} else if (auto g_it = find_by_name(this->options, name)) {
							g_it->parse(value);
						} else {
							throw std::invalid_argument(std::string("Invalid command line option \"") + *arg + "\".");
						}
					}
				} else {
					auto it = std::find_if(std::begin(this->commands), std::end(this->commands), [&](auto c) {
						return c.name == *arg || c.alias == *arg;
					});
					if (it != std::end(this->commands)) {
						command_it = it;
					} else {
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
						} else {
							if (this->positional_args.size() > positional_index) {
								this->positional_args[positional_index](*arg);
								positional_index++;
							} else if (this->rest_args) {
								this->rest_args(*arg);
							}
						}
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
