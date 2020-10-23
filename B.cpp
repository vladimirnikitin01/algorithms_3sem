/*Строка называется палиндромом, если она одинаково читается как слева направо, так и справа налево. Например, «abba» – палиндром, а «roman» – нет.
Для заданной строки s длины n (1 ≤ n ≤ 105) требуется подсчитать число пар (i, j), 1 ≤ i < j ≤ n, таких что подстрока s[i..j] является палиндромом.*/


#include <iostream>
#include <vector>
#include <string>
#include<algorithm>

char separating_character = '#';
std::vector<uint64_t> algorithmManager(const std::string& s) {
	std::vector<uint64_t> quantity_in_i(s.size(), 1);
	int64_t l = 0, r = -1;
	for (uint64_t i = 0; i < s.size(); ++i) {
		uint64_t k = 1;
		if (r >= 0 && i <= r) {
			k = std::min(r - i + 1, quantity_in_i[l + r - i]);
		}
		while (i + k < s.size() && i >= k && s[i + k] == s[i - k]) {
			++k;
		}
		quantity_in_i[i] = k;
		if (k + i > r + 1) {
			l = 1 + i - k;
			r = k + i - 1;
		}
	}
	return quantity_in_i;
}

uint64_t numberOfPalindromes(const std::string& now, const std::vector<uint64_t>& quantity_array) {
	int64_t sum = 0;
	for (uint64_t i = 0; i < now.size(); ++i) {
		if (now[i] == separating_character) {
			sum += quantity_array[i] / 2;
		}
		else {
			sum += quantity_array[i] / 2 - 1;
		}
	}
	return sum;
}

uint64_t algorithm(const std::string& s) {
	std::string new_s;
	new_s.reserve(2 * s.size());
	new_s += separating_character;
	for (uint64_t i = 0; i < s.size(); ++i) {
		new_s += s[i];
		new_s += separating_character;
	}
	std::vector<uint64_t> quantity_array = algorithmManager(new_s);
	return (numberOfPalindromes(new_s, quantity_array));
}


int main() {
	std::string s;
	std::cin >> s;
	std::cout << algorithm(s);
}
