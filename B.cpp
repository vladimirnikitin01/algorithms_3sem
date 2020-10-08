/*Строка называется палиндромом, если она одинаково читается как слева направо, так и справа налево. Например, «abba» – палиндром, а «roman» – нет.
Для заданной строки s длины n (1 ≤ n ≤ 105) требуется подсчитать число пар (i, j), 1 ≤ i < j ≤ n, таких что подстрока s[i..j] является палиндромом.*/

#include <iostream>
#include <vector>
#include <string>
#include<algorithm>

using namespace std;

vector<long long> algorithmManager(string& s) {
	vector<long long> quantity_in_i(s.size(), 1);
	int l = 0, r = -1;
	for (long long i = 0; i < s.size(); ++i) {
		long long k = 1;
		if (i <= r) {
			k = min(r - i + 1, quantity_in_i[l + r - i]);
		}
		while (i + k < s.size() && i - k >= 0 && s[i + k] == s[i - k]) {
			++k;
		}
		quantity_in_i[i] = k;
		if (k + i - 1 > r) {
			l = i - k + 1;
			r = k + i - 1;
		}
	}
	return quantity_in_i;
}

long long numberOfPalindromes(string& now, vector<long long>& quantity_array) {
	long long sum = 0;
	for (unsigned int i = 0; i < now.size(); ++i) {
		if (now[i] == '#') {
			sum += quantity_array[i] / 2;
		}
		else {
			sum += quantity_array[i] / 2 - 1;
		}
	}
	return sum;
}

long long algorithm(string& s) {
	string new_s = "#";
	for (unsigned int i = 0; i < s.size(); ++i) {
		new_s += s[i];
		new_s += "#";
	}
	vector<long long> quantity_array = algorithmManager(new_s);
	return (numberOfPalindromes(new_s, quantity_array));
}


int main() {
	string s;
	cin >> s;
	cout << algorithm(s);
}
