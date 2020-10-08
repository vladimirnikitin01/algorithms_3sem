/*Найдите все вхождения шаблона в строку. Длина шаблона – p, длина строки – n. Время O(n + p), доп. память – O(p).
p ≤ 30000, n ≤ 300000.
Вариант 2. С помощью z-функции.*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct array_z_and_l_r {
	vector<int> z;
	int l = 0, r = 0;
	array_z_and_l_r(vector<int> array, int L, int R) {
		z = array;
		l = L;
		r = R;
	}
};

array_z_and_l_r z_function(string& s) {
	vector<int> z(s.size(), 0);
	int left = 0, right = 0;
	for (int i = 1; i < s.size(); ++i) {
		z[i] = max(min(z[i - left], right - i + 1), 0);
		while (i + z[i] < s.size() && s[i + z[i]] == s[z[i]]) {
			++z[i];
		}
		if (z[i] + i - 1 > right && z[i] > 0) {
			left = i;
			right = z[i] + i - 1;
		}
	}

	return (array_z_and_l_r(z, left, right));
}

void z_function_main(string& s, string& pattern, array_z_and_l_r information_pattern) {
	int left = information_pattern.l, right = information_pattern.r;
	for (int i = 1 + information_pattern.z.size(); i < s.size() + information_pattern.z.size() + 1; ++i) {
		int z_now = 0;
		if (right - i + 1 > 0) {
			z_now = (min(information_pattern.z[i - left], right - i + 1));
		}
		while (i + z_now < s.size() + information_pattern.z.size() + 1 && s[i + z_now - information_pattern.z.size() - 1] == pattern[z_now]) {
			++z_now;
		}
		if (z_now + i - 1 > right && z_now > 0) {
			left = i;
			right = z_now + i - 1;
		}
		if (z_now == information_pattern.z.size()) {
			cout << i - 1 - information_pattern.z.size() << " ";
		}
	}
}

void algorithm(string& pattern, string& text) {
	auto z = z_function(pattern);
	z_function_main(text, pattern, z);
}

int main()
{
	string pattern;
	string text;
	cin >> pattern >> text;
	algorithm(pattern, text);
}
