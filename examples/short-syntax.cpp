#include <vector>
#include <map>
#include <iostream>

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

template<typename T, typename V>
std::ostream& operator<<(std::ostream &ss, const std::map<T, V>& v) {
	return print_map(ss, v);
}

int main(int argc, const char** argv) {
	auto b = false;
	auto i = 0;
	auto d = 0.0;
	std::string str = ""s;
	auto v = std::vector<int>{};
	auto m = std::map<std::string, std::string>{};

	args::options options = {
		{"-b", &b},
		{"-i", &i},
		{"-d", &d},
		{args::required, "-s", "--str", &str},
		{"-v", &v},
		{"-m", &m}
	};

	args::parse(argc, argv, options);

	std::cout << std::boolalpha;
	std::cout << "b=" << b << std::endl;
	std::cout << "i=" << i << std::endl;
	std::cout << "d=" << d << std::endl;
	std::cout << "str=" << str << std::endl;
	std::cout << "v=" << v << std::endl;
	std::cout << "m=" << m << std::endl;
}
