/*Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p ≤ 30000, n ≤ 300000.
Вариант 2. С помощью z-функции.*/


#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct array_z_and_l_r {
	std::vector<uint64_t> z;
	uint64_t l = 0, r = 0;
	array_z_and_l_r(std::vector<uint64_t> array, uint64_t L, uint64_t R) : z(std::move(array)), l(L), r(R) {}
};

array_z_and_l_r z_function(const std::string& s) {
	std::vector<uint64_t> z(s.size(), 0);
	uint64_t left = 0, right = 0;
	for (uint64_t i = 1; i < s.size(); ++i) {
		if (right < i - 1) {
			z[i] = 0;
		}
		else {
			z[i] = std::min(z[i - left], right - i + 1);
		}
		while (i + z[i] < s.size() && s[i + z[i]] == s[z[i]]) {
			++z[i];
		}
		if (z[i] + i - 1 > right && z[i] > 0) {
			left = i;
			right = z[i] + i - 1;
		}
	}

	return { z, left, right };
}

void z_function_main(const std::string& s, const  std::string& pattern, const array_z_and_l_r information_pattern) {
	uint64_t left = information_pattern.l, right = information_pattern.r;
	for (uint64_t i = 1 + information_pattern.z.size(); i < s.size() + information_pattern.z.size() + 1; ++i) {
		uint64_t z_now = 0;
		if (right > i - 1) {
			z_now = (std::min(information_pattern.z[i - left], right - i + 1));
		}
		while (i + z_now < s.size() + information_pattern.z.size() + 1 && s[i + z_now - information_pattern.z.size() - 1] == pattern[z_now]) {
			++z_now;
		}
		if (z_now + i - 1 > right && z_now > 0) {
			left = i;
			right = z_now + i - 1;
		}
		if (z_now == information_pattern.z.size()) {
			std::cout << i - 1 - information_pattern.z.size() << " ";
		}
	}
}

void algorithm(const std::string& pattern, const std::string& text) {
	auto z = z_function(pattern);
	z_function_main(text, pattern, z);
}

int main()
{
	std::string pattern;
	std::string text;
	std::cin >> pattern >> text;
	algorithm(pattern, text);
}
