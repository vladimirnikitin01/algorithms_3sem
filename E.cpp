/*Даны строки s и t. Постройте сжатое суффиксное дерево,
которое содержит все суффиксы строки s и строки t.
Найдите такое дерево, которое содержит минимальное количество вершин.*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <map>

struct Vertex {
	int64_t parent = -1;
	int64_t left_border = -1;
	int64_t right_border = -1;
	int64_t suffix_link = 0;
	std::map<char, uint64_t> edge;
	uint64_t parent_dfs = 0;//эти 2 переменные нужны для вывода в dfs в нужном порядке,
	uint64_t active_number_dfs = 0;// так как мы будем на ходу давать новый порядок
	Vertex(const int64_t parent_,const int64_t left, const int64_t right) : parent(parent_), left_border(left), right_border(right) {}
	Vertex() {}
};
class Gragh {
private:
	uint8_t const size_alphabet = 26;
	uint64_t vertex_now = 0;
	int64_t active_symbol = 0;
	//int64_t shipt_in_edge = 0;//position-Vertex.l it is for me
	uint64_t size = 0;
	uint64_t position_$;//нужно для вывода будет
	uint64_t index_string = 0;//
	uint64_t k_for_dfs = 0;
	std::string all_text;
	std::vector<Vertex> vertexes;
public:
	Gragh() {
		build_0_phase();
	}

	void build_0_phase() {
		vertexes.resize(2);
		//мы создали джокер и корень. Тепеь надо сделать все ребра от джокера(1) к корню(0). И ссылку для корня на Джокера
		vertexes[1].edge['#'] = 0;
		vertexes[1].edge['$'] = 0;
		vertexes[0].suffix_link = 1;
		//may be parent
		for (uint64_t i = 0; i < size_alphabet; ++i) {
			vertexes[1].edge['a' + i] = 0;
		}
	}

	void add_edge(const char s) {
		vertexes.emplace_back(Vertex(vertex_now, index_string, all_text.size() - 1));
		vertexes[vertex_now].edge[s] = (vertexes.size() - 1);
		vertex_now = vertexes[vertex_now].suffix_link;//теперь надо сделать это для других вершин, идем по ссылкам

		active_symbol = vertexes[vertex_now].right_border + 1;
		ukkonen(s);
	}

	void merge_and_split() {
		// добавим новую вершину, которую навверх пропихнем вместо нынешней
		auto information_vertex_now0 = vertexes[vertex_now];
		vertexes.emplace_back(Vertex(information_vertex_now0.parent, information_vertex_now0.left_border, active_symbol - 1));
		//а теперь надо менять ребра и т.д
		auto& new_vertex = vertexes[vertexes.size() - 1];
		auto& information_vertex_now1 = vertexes[vertex_now];
		new_vertex.edge[all_text[active_symbol]] = vertex_now;
		information_vertex_now1.left_border = active_symbol;
		vertexes[information_vertex_now1.parent].edge[all_text[new_vertex.left_border]] = vertexes.size() - 1;
		information_vertex_now1.parent = vertexes.size() - 1;
		vertexes.emplace_back(Vertex(vertexes.size() - 1, index_string, all_text.size() - 1));// проверить -2 и -1
		auto& old_vertex = vertexes[vertexes.size() - 2];
		old_vertex.edge[all_text[index_string]] = vertexes.size() - 1;
	}

	void do_new_link() {
		auto& old_vertex = vertexes[vertexes.size() - 2];
		vertex_now = vertexes[old_vertex.parent].suffix_link;
		active_symbol = old_vertex.left_border;
		//теперь будем идти маскимально вниз
		while (active_symbol <= old_vertex.right_border) {
			vertex_now = vertexes[vertex_now].edge[all_text[active_symbol]];
			active_symbol += (vertexes[vertex_now].right_border - vertexes[vertex_now].left_border) + 1;
		}
		if (active_symbol - 1 == old_vertex.right_border) {//вершинка
			old_vertex.suffix_link = vertex_now;
		}
		else {
			old_vertex.suffix_link = vertexes.size();//в англоязычной литературе было важное уточнение, что ссылка создается с опаздыванием, так что тут все нормально
		}
		active_symbol = vertexes[vertex_now].right_border + old_vertex.right_border + 2 - active_symbol;
	}

	void dfs(const int64_t i) {
		auto& vertex_i = vertexes[i];
		if (i != 0) {
			std::cout << vertex_i.parent_dfs << ' ';
			if (vertex_i.left_border > position_$) {
				std::cout << 1 << ' ' << vertex_i.left_border - position_$ - 1 << ' ' << vertex_i.right_border - position_$ << '\n';
			}
			else if (vertex_i.right_border <= position_$) {
				std::cout << 0 << ' ' << vertex_i.left_border << ' ' << vertex_i.right_border + 1 << '\n';
			}
			else {
				std::cout << 0 << ' ' << vertex_i.left_border << ' ' << position_$ + 1 << '\n';
			}
		}
		for (auto symbol : vertexes[i].edge) {
			vertexes[symbol.second].active_number_dfs = ++k_for_dfs;
			vertexes[symbol.second].parent_dfs = vertexes[i].active_number_dfs;
			dfs(symbol.second);
		}
	}

	void print_my_tree() {
		std::cout << vertexes.size() - 1 << '\n';
		dfs(0);
	}

	void ukkonen(const char s) {//тут будет, скорее всего, 3 варианта добавления,как на нирке
		//находимся ли в границах нашего ребра
		if (vertexes[vertex_now].right_border < active_symbol) {
			//сейчас надо пойти по след ребру, иначе создать его и пойти по нему
			if (vertexes[vertex_now].edge.find(s) != vertexes[vertex_now].edge.end()) {
				vertex_now = vertexes[vertex_now].edge[s];
				active_symbol = vertexes[vertex_now].left_border;
				// мы просто перешли по нужному ребру и затем будем его обрабатывать
			}
			else {
				add_edge(s);//вообще это 2 а) нирка
				// при добавлении ребра, мы должны еще это сделать и для других вершин тоже
				return;
			}
		}
		// еще нам надо обработать ситуации, когда после шага мы в корне или в Джокере
		if (vertex_now == 1 || vertex_now == 0) {
			active_symbol = 0;
			return;
		}
		// 3 вариант в нирке, когда нам ничего не надо делать
		if (all_text[active_symbol] == s) {
			++active_symbol;
			return;
		}
		//Теперь будет пункт 2б из нирка
		merge_and_split();
		do_new_link();
		ukkonen(s);
	}

	void algorithm(const std::string& first_string, const std::string& second_string) {
		std::string itog = first_string + second_string;
		all_text = itog;
		position_$ = first_string.size() - 1;
		for (auto s : itog) {
			ukkonen(s);
			++index_string;
		}
		print_my_tree();
	}
};

int main()
{
	std::ios::sync_with_stdio(0);
	std::cin.tie(0);
	std::string a, b;
	std::cin >> a >> b;
	Gragh c;
	c.algorithm(a, b);
}
