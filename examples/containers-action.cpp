#include <vector>
#include <map>
#include <utility>
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
	auto vec = std::vector<int>{};
	auto m = std::map<std::string, std::string>{};

	auto p = args::parser{}
		.option<int>("-v", "--vector", [&](auto v){ vec.push_back(v); })
		.option<std::pair<std::string, std::string>>("-m", "--map", [&](auto v){ m.insert(v); });

	p.parse(argc, argv);

	std::cout << std::boolalpha;
	std::cout << "v=" << vec << std::endl;
	std::cout << "m=" << m << std::endl;
}
