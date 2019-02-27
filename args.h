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
			std::stringstream ss(value);
			ss >> *destination;
			if (ss.fail()) {
				throw std::logic_error("Can't parse \""s + value + "\"."s);
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
				throw std::invalid_argument("Value \""s
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
			std::stringstream ss(value);
			typename T::value_type c;
			ss >> c;
			if (ss.fail()) {
				throw std::logic_error("Can't parse \""s + value + "\"."s);
			}
			destination->insert(destination->end(), c);
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
			throw std::logic_error("Value \""s + value
					+ "\" is not key=value pair.");
		}

		auto k_str = value.substr(0, eq_pos);
		typename T::key_type k;
		std::stringstream k_stream(k_str);
		k_stream >> k;
		if (k_stream.fail()) {
			throw std::logic_error("Can't parse key in pair \""s + value + "\"."s);
		}

		auto v_str = value.substr(eq_pos + 1);
		typename T::mapped_type v;
		std::stringstream v_stream(v_str);
		v_stream >> v;
		if (v_stream.fail()) {
			throw std::logic_error("Can't parse value in pair \""s + value + "\"."s);
		}

		(*destination)[k] = v;
	}
}

namespace args {
	struct option {
		const std::string short_name;
		const std::string long_name;
		const std::string non_conventional;
		bool is_flag = false;
		std::function<void (std::string)> parse;

		option(const option&) = default;

		option(std::string name)
			: short_name(is_short_option(name) ? name : ""),
			long_name(is_long_option(name) ? name : ""),
			non_conventional(is_non_conventional(name) ? name : ""),
			parse([=](std::string value) {}) {}

		template<typename T>
		option(std::string name, T* destination)
			: short_name(is_short_option(name) ? name : ""),
			long_name(is_long_option(name) ? name : ""),
			non_conventional(is_non_conventional(name) ? name : ""),
			is_flag(std::is_same<T, bool>::value),
			parse([=](std::string value) { parse_value(value, destination); }) {}

		option(std::string short_name, std::string long_name)
			: short_name(is_short_option(short_name) ? short_name : ""),
			long_name(is_long_option(long_name) ? long_name : ""),
			parse([=](std::string value) {}) {}

		template<typename T>
		option(std::string short_name, std::string long_name, T* destination)
			: short_name(is_short_option(short_name) ? short_name : ""),
			long_name(is_long_option(long_name) ? long_name : ""),
			is_flag(std::is_same<T, bool>::value),
			parse([=](std::string value) { parse_value(value, destination); }) {}
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

		template<typename T, typename F>
		command_internal& positional(F handler) {
			positional_args.push_back([=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			});
			return *this;
		}

		template<typename T>
		command_internal& rest(T* destination) {
			rest_args = [=](std::string value) { parse_value(value, destination); };
			return *this;
		}

		template<typename T, typename F>
		command_internal& rest(F handler) {
			rest_args = [=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			};
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

		template<typename T, typename F>
		command_internal& option(std::string name, F handler) {
			auto option = args::option{name};
			option.is_flag = std::is_same<T, bool>::value;
			option.parse = [=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			};
			this->options.push_back(option);
			return *this;
		}

		template<typename T>
		command_internal& option(std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(short_name, long_name, destination);
			return *this;
		}

		template<typename T, typename F>
		command_internal& option(std::string short_name, std::string long_name, F handler) {
			auto option = args::option{short_name, long_name};
			option.is_flag = std::is_same<T, bool>::value;
			option.parse = [=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			};
			this->options.push_back(option);
			this->options.push_back(option);
			return *this;
		}

		command_internal& action(std::function<void (void)> action) {
			this->action_fun = action;
			return *this;
		}
	};
}

namespace args {
	using options = std::vector<option>;

	class invalid_option : public std::logic_error {
		public:
		const std::string option;

		invalid_option(std::string option)
			: logic_error("Invalid option \""s + option + "\"."),
			option(option) {}
	};

	class invalid_value : public std::logic_error {
		public:
		const std::string option;
		const std::string value;

		invalid_value(std::string option, std::string value, std::string what_arg)
			: logic_error("Invalid value for option \""s + option + "\". " + what_arg),
			option(option),
			value(value) {}
	};

	class unexpecter_arg : public std::logic_error {
		public:
		const std::string arg;

		unexpecter_arg(std::string arg)
			: logic_error("Unexpected argument \""s + arg + "\"."s),
			arg(arg) {}
	};

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

		template<typename T, typename F>
		parser& positional(F handler) {
			positional_args.push_back([=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			});
			return *this;
		}

		template<typename T>
		parser& rest(T* destination) {
			rest_args = [=](std::string value) { parse_value(value, destination); };
			return *this;
		}

		template<typename T, typename F>
		parser& rest(F handler) {
			rest_args = [=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			};
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

		template<typename T, typename F>
		parser& option(std::string name, F handler) {
			auto option = args::option{name};
			option.is_flag = std::is_same<T, bool>::value;
			option.parse = [=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			};
			this->options.push_back(option);
			return *this;
		}

		template<typename T>
		parser& option(std::string short_name, std::string long_name, T* destination) {
			this->options.emplace_back(short_name, long_name, destination);
			return *this;
		}

		template<typename T, typename F>
		parser& option(std::string short_name, std::string long_name, F handler) {
			auto option = args::option{short_name, long_name};
			option.is_flag = std::is_same<T, bool>::value;
			option.parse = [=](std::string value) {
				T destination;
				parse_value(value, &destination);
				handler(destination);
			};
			this->options.push_back(option);
			this->options.push_back(option);
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

			auto command_it = std::end(this->commands);
			auto find_option_if = [&](const std::function<bool (args::option o)> pred) {
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
			auto positional_index = 0;
			auto command_positional_index = 0;
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
						throw invalid_option(*arg);
					}

					if (option_it != std::end(this->options)) {
						if (*arg == option_it->short_name || *arg == option_it->long_name
								|| *arg == option_it->non_conventional) {

							if (option_it->is_flag) {
								auto next = std::next(arg);
								if (next != std::end(args) && is_valid_flag_value(*next)) {
									try {
										option_it->parse(*next);
									} catch (const std::logic_error& err) {
										throw invalid_value(*arg, *next, err.what());
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
									} catch (const std::logic_error& err) {
										throw invalid_value(*arg, *next, err.what());
									}
									arg++;
								} else {
									auto name = *arg;
									throw invalid_value(name, "", "Value is empty.");
								}
							}
						} else if ((!option_it->short_name.empty() && arg->starts_with(option_it->short_name + "="))
								|| (!option_it->long_name.empty() && arg->starts_with(option_it->long_name + "="))
								|| (!option_it->non_conventional.empty() && arg->starts_with(option_it->non_conventional + "="))) {

							auto value = arg->substr(arg->find("=") + 1);
							try {
								option_it->parse(value);
							} catch (const std::logic_error& err) {
								auto name = arg->substr(0, arg->find("="));
								throw invalid_value(name, value, err.what());
							}
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
									option_it->parse("1");
								});
							} else if (is_valid_flag_value(arg->substr(2))) {
								option_it->parse(arg->substr(2));
							} else {
								throw invalid_option(*arg);
							}
						} else if (!option_it->short_name.empty() && arg->starts_with(option_it->short_name)
								&& !option_it->is_flag) {

							auto value = arg->substr(2);
							try {
								option_it->parse(value);
							} catch (const std::logic_error& err) {
								auto name = arg->substr(0, 2);
								throw invalid_value(name, value, err.what());
							}
						} else if (arg->starts_with("--no-")) {
							option_it->parse("0");
						}

						continue;
					}
				}

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
					throw unexpecter_arg(*arg);
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
