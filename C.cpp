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


struct pair_start_index_and_size_pattern {
    int start_index;
    int size_pattern;

    pair_start_index_and_size_pattern(int start_index_, int size_pattern_) : start_index(start_index_),
        size_pattern(size_pattern_) {};
};

struct Vertex {
    const int the_power_of_the_alphabet = 26;
    int the_nearest_terminal_vertex = -1;
    std::vector<int> edge;//size 26
    int parent = -1;
    int last_edge = -1;//'symbol'-'a'
    int suffix_link = -1;
    std::vector<int> transfer_through_i;//size 26
    bool terminal = false;
    std::vector<pair_start_index_and_size_pattern> pattern_number;

    Vertex() : edge(the_power_of_the_alphabet, -1), transfer_through_i(the_power_of_the_alphabet, -1) {}
};


void ContinueConstructionOfTheForest(std::vector<Vertex>& forest, int& size, const std::string& pattern,
    const int start_index, const int size_pattern) {
    int vertex_now = 0;
    for (size_t i = 0; i < pattern.size(); ++i) {
        int now_edge = pattern[i] - 'a';
        if (forest[vertex_now].edge[now_edge] == -1) {
            forest[size].parent = vertex_now;
            forest[size].last_edge = now_edge;
            forest[vertex_now].edge[now_edge] = size;
            ++size;
        }
        vertex_now = forest[vertex_now].edge[now_edge];
    }
    forest[vertex_now].terminal = true;
    forest[vertex_now].pattern_number.emplace_back(start_index, size_pattern);
}

int Link(std::vector<Vertex>& forest, int now_vertex);

int Go(std::vector<Vertex>& forest, const int now_vertex, const int symbol) {
    if (forest[now_vertex].transfer_through_i[symbol] == -1) {
        if (forest[now_vertex].edge[symbol] == -1) {
            if (now_vertex != 0) {
                forest[now_vertex].transfer_through_i[symbol] = Go(forest, Link(forest, now_vertex), symbol);
            }
            else {
                forest[now_vertex].transfer_through_i[symbol] = 0;
            }
        }
        else {
            forest[now_vertex].transfer_through_i[symbol] = forest[now_vertex].edge[symbol];
        }
    }
    return forest[now_vertex].transfer_through_i[symbol];
}

int Link(std::vector<Vertex>& forest, const int now_vertex) {
    if (forest[now_vertex].suffix_link == -1) {
        if (forest[now_vertex].parent == 0 || now_vertex == 0) {
            forest[now_vertex].suffix_link = 0;
        }
        else {
            forest[now_vertex].suffix_link =
                Go(forest, Link(forest, forest[now_vertex].parent), forest[now_vertex].last_edge);
        }
    }
    return forest[now_vertex].suffix_link;
}

int TheNearestTerminalVertex(std::vector<Vertex>& forest, const int now_vertex) {
    if (forest[now_vertex].the_nearest_terminal_vertex == -1) {
        int suffix_link_now = Link(forest, now_vertex);
        if (forest[suffix_link_now].terminal || suffix_link_now == 0) {
            forest[now_vertex].the_nearest_terminal_vertex = suffix_link_now;
        }
        else {
            forest[now_vertex].the_nearest_terminal_vertex = TheNearestTerminalVertex(forest, suffix_link_now);
        }
    }
    return forest[now_vertex].the_nearest_terminal_vertex;
}


int ProcessingAndConstruction(const std::string& pattern, std::vector<Vertex>& forest) {
    std::string s;
    int size = 1;
    int start_index = 0;
    int count = 0;
    for (size_t i = 0; i < pattern.size(); ++i) {
        if (pattern[i] != '?' && (i == 0 || (i >= 1 && pattern[i - 1] == '?'))) {//start
            start_index = i;
            s += pattern[i];
            ++count;
        }
        else if (pattern[i] != '?') {
            s += pattern[i];
        }
        if ((pattern[i] == '?' && !s.empty()) || (i == pattern.size() - 1 && pattern[i] != '?')) {
            ContinueConstructionOfTheForest(forest, size, s, start_index, s.size());
            s = "";
        }
    }
    return count;
}

std::vector<int> AhoCorasick(const std::string& pattern, const std::string& text) {
    std::vector<Vertex> forest(pattern.size() + 1);
    int count = ProcessingAndConstruction(pattern, forest);
    //processing
    int now_vertex = 0;
    std::vector<int> result(text.size());
    for (int i = 0; i < text.size(); ++i) {
        now_vertex = Go(forest, now_vertex, text[i] - 'a');
        if (forest[now_vertex].terminal) {
            for (size_t j = 0; j < forest[now_vertex].pattern_number.size(); ++j) {
                auto& terminal_now = forest[now_vertex].pattern_number;
                if (i - terminal_now[j].size_pattern - terminal_now[j].start_index + 1 >= 0)
                    ++result[i - terminal_now[j].size_pattern - terminal_now[j].start_index + 1];
            }
        }
        int the_next_terminal = TheNearestTerminalVertex(forest, now_vertex);
        while (the_next_terminal != 0) {
            for (size_t j = 0; j < forest[the_next_terminal].pattern_number.size(); ++j) {
                auto& terminal_now = forest[the_next_terminal].pattern_number;
                if (i - terminal_now[j].size_pattern - terminal_now[j].start_index + 1 >= 0)
                    ++result[i - terminal_now[j].size_pattern - terminal_now[j].start_index + 1];
            }
            the_next_terminal = TheNearestTerminalVertex(forest, the_next_terminal);
        }
    }
    std::vector<int> output;
    for (size_t i = 0; i < result.size(); ++i) {
        if (result[i] == count && i + pattern.size() <= text.size()) {
            output.push_back(i);
        }
    }
    return output;
}

int main() {
    std::string a, b;
    std::cin >> a >> b;
    for (auto& result : AhoCorasick(a, b)) {
        std::cout << result << " ";
    }
}
