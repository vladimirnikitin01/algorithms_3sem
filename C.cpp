/*Шаблон поиска задан строкой длины m, в которой кроме обычных символов могут встречаться символы “?”.
 * Найти позиции всех вхождений шаблона в тексте длины n. Каждое вхождение шаблона предполагает,
 * что все обычные символы совпадают с соответствующими из текста,
 * а вместо символа “?” в тексте встречается произвольный символ. Гарантируется, что сам “?” в тексте не встречается.
Время работы - O(n + m + Z), где Z - общее число вхождений подстрок шаблона “между вопросиками” в исходном тексте.
 m ≤ 5000, n ≤ 2000000*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

struct Vertex {
	int the_nearest_terminal_vertex = -1;
	vector<int> ribs;//size 26
	int parent = -1;
	int last_rib = -1;//'symbol'-'a'
	int suffix_link = -1;
	vector<int> transfer_through_i;//size 26
	bool terminal = false;
	vector<std::pair<int, int>> pattern_number;
	Vertex() {
		ribs.assign(26, -1);
		transfer_through_i.assign(26, -1);
	}
};

void Build_forest(vector<Vertex>& forest, int& size, string& pattern, int start_index, int size_pattern) {
	int vertex_now = 0;
	for (int i = 0; i < pattern.size(); ++i) {
		int now_rib = pattern[i] - 'a';
		if (forest[vertex_now].ribs[now_rib] == -1) {
			forest[size].parent = vertex_now;
			forest[size].last_rib = now_rib;
			forest[vertex_now].ribs[now_rib] = size;
			++size;
		}
		vertex_now = forest[vertex_now].ribs[now_rib];
	}
	forest[vertex_now].terminal = true;
	forest[vertex_now].pattern_number.emplace_back(start_index, size_pattern);
}

int Link(vector<Vertex>& forest, int now_vertex);

int Go(vector<Vertex>& forest, int now_vertex, int symbol) {
	if (forest[now_vertex].transfer_through_i[symbol] == -1) {
		if (forest[now_vertex].ribs[symbol] == -1) {
			if (now_vertex != 0) {
				forest[now_vertex].transfer_through_i[symbol] = Go(forest, Link(forest, now_vertex), symbol);
			}
			else {
				forest[now_vertex].transfer_through_i[symbol] = 0;
			}
		}
		else {
			forest[now_vertex].transfer_through_i[symbol] = forest[now_vertex].ribs[symbol];
		}
	}
	return (forest[now_vertex].transfer_through_i[symbol]);
}

int Link(vector<Vertex>& forest, int now_vertex) {
	if (forest[now_vertex].suffix_link == -1) {
		if (forest[now_vertex].parent == 0 || now_vertex == 0) {
			forest[now_vertex].suffix_link = 0;
		}
		else {
			forest[now_vertex].suffix_link =
				Go(forest, Link(forest, forest[now_vertex].parent), forest[now_vertex].last_rib);
		}
	}
	return (forest[now_vertex].suffix_link);
}

int TheNearestTerminalVertex(vector<Vertex>& forest, int now_vertex) {
	if (forest[now_vertex].the_nearest_terminal_vertex == -1) {
		int suffix_link_now = Link(forest, now_vertex);
		if (forest[suffix_link_now].terminal || suffix_link_now == 0) {
			forest[now_vertex].the_nearest_terminal_vertex = suffix_link_now;
		}
		else {
			forest[now_vertex].the_nearest_terminal_vertex = TheNearestTerminalVertex(forest, suffix_link_now);
		}
	}
	return (forest[now_vertex].the_nearest_terminal_vertex);
}

vector<int> AhoCorasick(string& pattern, string& text) {
	vector<Vertex> forest(pattern.size() + 1);
	string s;
	int size = 1;
	int start_index = 0;
	int count = 0;
	for (int i = 0; i < pattern.size(); ++i) {
		if (pattern[i] != '?' && (i == 0 || (i >= 1 && pattern[i - 1] == '?'))) {//start
			start_index = i;
			s += pattern[i];
			++count;
		}
		else if (pattern[i] != '?') {
			s += pattern[i];
		}
		if ((pattern[i] == '?' && !s.empty()) || (i == pattern.size() - 1 && pattern[i] != '?')) {
			Build_forest(forest, size, s, start_index, s.size());
			s = "";
		}
	}
	int now_vertex = 0;
	vector<int> result(text.size());
	for (int i = 0; i < text.size(); ++i) {
		now_vertex = Go(forest, now_vertex, text[i] - 'a');
		if (forest[now_vertex].terminal) {
			for (int j = 0; j < forest[now_vertex].pattern_number.size(); ++j) {
				auto& terminal_now = forest[now_vertex].pattern_number;
				if (i - terminal_now[j].second - terminal_now[j].first + 1 >= 0)
					++result[i - terminal_now[j].second - terminal_now[j].first + 1];
			}
		}
		int the_next_terminal = TheNearestTerminalVertex(forest, now_vertex);
		while (the_next_terminal != 0) {
			for (int j = 0; j < forest[the_next_terminal].pattern_number.size(); ++j) {
				auto& terminal_now = forest[the_next_terminal].pattern_number;
				if (i - terminal_now[j].second - terminal_now[j].first + 1 >= 0)
					++result[i - terminal_now[j].second - terminal_now[j].first + 1];
			}
			the_next_terminal = TheNearestTerminalVertex(forest, the_next_terminal);
		}
	}
	vector<int> output;
	for (int i = 0; i < result.size(); ++i) {
		if (result[i] == count && i + pattern.size() <= text.size()) {
			output.push_back(i);
		}
	}
	return (output);
}

int main() {
	string a, b;
	cin >> a >> b;
	auto result = AhoCorasick(a, b);
	for (int i = 0; i < result.size(); ++i) {
		cout << result[i] << " ";
	}
}
