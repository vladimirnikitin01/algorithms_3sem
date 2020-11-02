/*Заданы две строки s, t и целое число k. Рассмотрим множество всех таких непустых строк, которые встречаются как подстроки в s и t одновременно. Найдите k-ую в лексикографическом порядке строку из этого множества.*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct value_and_index {
	int64_t value = 0;
	uint64_t index = 0;
	value_and_index(const int64_t value_, const uint64_t index_) : value(value_), index(index_) {};
	value_and_index() {};
};

bool comp(const value_and_index& a, const value_and_index& b) {
	return a.value < b.value;
}

void BuildSuffixArrayFirstStep(std::vector<uint64_t>& suffix_array, std::vector<uint64_t>& class_symbols, std::string& s, uint64_t& number_class) {
	std::vector<value_and_index> array_of_symbols(s.size());
	for (uint64_t i = 0; i < s.size(); ++i) {
		array_of_symbols[i] = { s[i], i };
	}
	std::sort(array_of_symbols.begin(), array_of_symbols.end(), comp);
	for (uint64_t i = 0; i < s.size(); ++i) {
		suffix_array[i] = array_of_symbols[i].index;
	}
	class_symbols[suffix_array[0]] = 0;
	for (uint64_t i = 0; i + 1 < s.size(); ++i) {
		if (s[suffix_array[i + 1]] != s[suffix_array[i]]) {
			++number_class;
		}
		class_symbols[suffix_array[i + 1]] = number_class;
	}
}


void BuildSuffixArraySecondStep(std::vector<uint64_t>& suffix_array, std::vector<uint64_t>& class_symbols, const std::string& s, uint64_t& class_of_number) {
	std::vector<uint64_t> suffix_array_for_second_part(s.size());
	std::vector<uint64_t> class_symbols_new(s.size());
	uint64_t k = 1;
	while (true) {
		for (uint64_t i = 0; i < s.size(); ++i) {
			if (suffix_array[i] < k) {
				suffix_array_for_second_part[i] = s.size() + suffix_array[i] - k;
			}
			else {
				suffix_array_for_second_part[i] = 0 + suffix_array[i] - k;
			}
		}
		std::vector<int64_t> count(s.size(), 0);
		for (uint64_t i = 0; i < s.size(); ++i) {
			++count[class_symbols[suffix_array_for_second_part[i]]];
		}
		for (uint64_t i = 0; i < class_of_number; ++i) {
			count[i + 1] += count[i];
		}
		for (int64_t i = s.size() - 1; i >= 0; --i) {
			--count[class_symbols[suffix_array_for_second_part[i]]];
			suffix_array[count[class_symbols[suffix_array_for_second_part[i]]]] = suffix_array_for_second_part[i];
		}
		class_of_number = 0;
		class_symbols_new[suffix_array[0]] = class_of_number;
		for (uint64_t i = 0; i + 1 < s.size(); ++i) {
			if (class_symbols[suffix_array[i + 1]] != class_symbols[suffix_array[i]] || class_symbols[(suffix_array[i + 1] + k) % s.size()] != class_symbols[(suffix_array[i] + k) % s.size()]) {
				++class_of_number;
			}
			class_symbols_new[suffix_array[i + 1]] = class_of_number;
		}
		if (class_of_number + 1 == s.size()) {
			break;
		}
		class_symbols = class_symbols_new;
		k *= 2;
	}
}

std::vector<uint64_t> BuildSuffixArray(const std::string& s) {
	std::string s1 = s + "$";
	std::vector<uint64_t> suffix_array(s1.size());
	std::vector<uint64_t> class_symbols(s1.size());
	uint64_t number_class = 0;
	BuildSuffixArrayFirstStep(suffix_array, class_symbols, s1, number_class);
	//second step
	BuildSuffixArraySecondStep(suffix_array, class_symbols, s1, number_class);
	//second step
	suffix_array.erase(suffix_array.begin());
	return suffix_array;
}

std::vector<uint64_t> BuildLcp(const std::string& s, const std::vector<uint64_t>& suffix_array) {
	// do the reverse suffix_array
	std::vector<uint64_t> reserve_suffix_array(suffix_array.size());
	for (uint64_t i = 0; i < suffix_array.size(); ++i) {
		reserve_suffix_array[suffix_array[i]] = i;
	}
	std::vector<uint64_t> lcp(s.size(), 0);
	int64_t lcp_now = 0;
	for (uint64_t i = 0; i < s.size(); ++i) {
		if (reserve_suffix_array[i] != 0) {
			int64_t the_previous_object_in_suffix_array = suffix_array[reserve_suffix_array[i] - 1];
			while (i + lcp_now < s.size() && the_previous_object_in_suffix_array + lcp_now < s.size() && s[i + lcp_now] == s[the_previous_object_in_suffix_array + lcp_now]) {
				++lcp_now;
			}
			lcp[reserve_suffix_array[i]] = lcp_now;
			lcp_now = std::max(int64_t(0), lcp_now - 1);
		}
		else {
			lcp_now = 0;
		}
	}
	return lcp;
}
std::string Algorithm(const std::string& a, const std::string& b, const uint64_t k) {
	std::string s = a + "%" + b;
	auto suffix = BuildSuffixArray(s);
	auto lcp = BuildLcp(s, suffix);
	uint64_t sum = 0;
	uint64_t min_lcp = 0;
	uint64_t position_$ = a.size();
	std::string result = "";
	for (uint64_t i = 0; i < s.size(); ++i) {
		min_lcp = std::min(min_lcp, lcp[i]);
		if (lcp[i] > 0 && ((suffix[i]<position_$ && suffix[i - 1] > position_$) || (suffix[i] > position_$ && suffix[i - 1] < position_$))) {
			sum += lcp[i];
			sum -= min_lcp;
			min_lcp = lcp[i];
			if (sum >= k) {
				for (int64_t j = 0; j < k - (sum - lcp[i]); ++j) {
					result += s[suffix[i] + j];
				}
				break;
			}
		}

	}
	return result;
}

int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	uint64_t k;
	std::string a, b;
	std::cin >> a >> b >> k;
	auto s = Algorithm(a, b, k);
	if (s == "") {
		std::cout << -1;
	}
	else {
		std::cout << s;
	}
}
