/*Даны строки s и t. Постройте сжатое суффиксное дерево,
которое содержит все суффиксы строки s и строки t.
Найдите такое дерево, которое содержит минимальное количество вершин.*/

#include <iostream>
#include <vector>
#include <string>
#include <map>

struct Vertex {
    int64_t parent = -1;
    int64_t left_border = -1;
    int64_t right_border = -1;
    int64_t suffix_link = 0;
    std::map<char, uint64_t> edge;
    uint64_t parent_dfs = 0;//эти 2 переменные нужны для вывода в dfs в нужном порядке,
    uint64_t active_number_dfs = 0;// так как мы будем на ходу давать новый порядок
    Vertex(const int64_t parent_, const int64_t left, const int64_t right) : parent(parent_), left_border(left),
                                                                             right_border(right) {}

    Vertex() = default;
};

class Ukkonen {
private:
    uint8_t const SIZE_ALPHABET_ = 26;
    uint8_t const HASH_ = '#';
    uint8_t const DOLLAR_ = '$';
    uint8_t const FIRST_LETTER_ = 'a';
    uint64_t vertex_now_ = 0;
    int64_t active_symbol_ = 0;
    uint64_t position_$_;//нужно для вывода будет
    uint64_t index_string_ = 0;//
    uint64_t k_for_dfs_ = 0;
    std::string all_text_;
    std::vector<Vertex> vertexes_;

    void add_edge(char s);

    void merge_and_split();

    void do_new_link();

    void dfs(int64_t i);

public:
    Ukkonen();

    void print_my_tree();

    void add_symbol(char s);

    void algorithm(const std::string &first_string, const std::string &second_string);
};

Ukkonen::Ukkonen() {
    vertexes_.resize(2);
    //мы создали джокер и корень. Тепеь надо сделать все ребра от джокера(1) к корню(0). И ссылку для корня на Джокера
    vertexes_[1].edge[HASH_] = 0;
    vertexes_[1].edge[DOLLAR_] = 0;
    vertexes_[0].suffix_link = 1;
    //may be parent
    for (uint64_t i = 0; i < SIZE_ALPHABET_; ++i) {
        vertexes_[1].edge[FIRST_LETTER_ + i] = 0;
    }
}

void Ukkonen::algorithm(const std::string &first_string, const std::string &second_string) {
    std::string itog = first_string + second_string;
    all_text_ = itog;
    position_$_ = first_string.size() - 1;
    for (auto s : itog) {
        add_symbol(s);
        ++index_string_;
    }
    print_my_tree();
}

void Ukkonen::add_symbol(const char s) {
//тут будет, скорее всего, 3 варианта добавления,как на нирке
    //находимся ли в границах нашего ребра
    if (vertexes_[vertex_now_].right_border < active_symbol_) {
        //сейчас надо пойти по след ребру, иначе создать его и пойти по нему
        if (vertexes_[vertex_now_].edge.find(s) != vertexes_[vertex_now_].edge.end()) {
            vertex_now_ = vertexes_[vertex_now_].edge[s];
            active_symbol_ = vertexes_[vertex_now_].left_border;
            // мы просто перешли по нужному ребру и затем будем его обрабатывать
        } else {
            add_edge(s);//вообще это 2 а) нирка
            // при добавлении ребра, мы должны еще это сделать и для других вершин тоже
            return;
        }
    }
    // еще нам надо обработать ситуации, когда после шага мы в корне или в Джокере
    if (vertex_now_ == 1 || vertex_now_ == 0) {
        active_symbol_ = 0;
        return;
    }
    // 3 вариант в нирке, когда нам ничего не надо делать
    if (all_text_[active_symbol_] == s) {
        ++active_symbol_;
        return;
    }
    //Теперь будет пункт 2б из нирка
    merge_and_split();
    do_new_link();
    add_symbol(s);
}

void Ukkonen::print_my_tree() {
    std::cout << vertexes_.size() - 1 << '\n';
    dfs(0);
}

void Ukkonen::add_edge(const char s) {
    vertexes_.emplace_back(Vertex(vertex_now_, index_string_, all_text_.size() - 1));
    vertexes_[vertex_now_].edge[s] = (vertexes_.size() - 1);
    vertex_now_ = vertexes_[vertex_now_].suffix_link;
    //теперь надо сделать это для других вершин, идем по ссылкам

    active_symbol_ = vertexes_[vertex_now_].right_border + 1;
    add_symbol(s);
}

void Ukkonen::dfs(const int64_t i) {
    auto &vertex_i = vertexes_[i];
    if (i != 0) {
        std::cout << vertex_i.parent_dfs << ' ';
        if (vertex_i.left_border > position_$_) {
            std::cout << 1 << ' ' << vertex_i.left_border - position_$_ - 1 << ' '
                      << vertex_i.right_border - position_$_ << '\n';
        } else if (vertex_i.right_border <= position_$_) {
            std::cout << 0 << ' ' << vertex_i.left_border << ' ' << vertex_i.right_border + 1 << '\n';
        } else {
            std::cout << 0 << ' ' << vertex_i.left_border << ' ' << position_$_ + 1 << '\n';
        }
    }
    for (auto symbol : vertexes_[i].edge) {
        vertexes_[symbol.second].active_number_dfs = ++k_for_dfs_;
        vertexes_[symbol.second].parent_dfs = vertexes_[i].active_number_dfs;
        dfs(symbol.second);
    }
}

void Ukkonen::do_new_link() {
    auto &old_vertex = vertexes_[vertexes_.size() - 2];
    vertex_now_ = vertexes_[old_vertex.parent].suffix_link;
    active_symbol_ = old_vertex.left_border;

    //теперь будем идти маскимально вниз
    while (active_symbol_ <= old_vertex.right_border) {
        vertex_now_ = vertexes_[vertex_now_].edge[all_text_[active_symbol_]];
        active_symbol_ += (vertexes_[vertex_now_].right_border - vertexes_[vertex_now_].left_border) + 1;
    }
    if (active_symbol_ - 1 == old_vertex.right_border) {//вершинка
        old_vertex.suffix_link = vertex_now_;
    } else {
        old_vertex.suffix_link = vertexes_.size();
        //в англоязычной литературе было важное уточнение, что ссылка создается с опаздыванием, так что тут все нормально
    }
    active_symbol_ = vertexes_[vertex_now_].right_border + old_vertex.right_border + 2 - active_symbol_;
}

void Ukkonen::merge_and_split() {
// добавим новую вершину, которую навверх пропихнем вместо нынешней
    auto information_vertex_now0 = vertexes_[vertex_now_];
    vertexes_.emplace_back(
            Vertex(information_vertex_now0.parent, information_vertex_now0.left_border, active_symbol_ - 1));

    //а теперь надо менять ребра и т.д
    auto &new_vertex = vertexes_[vertexes_.size() - 1];
    auto &information_vertex_now1 = vertexes_[vertex_now_];
    new_vertex.edge[all_text_[active_symbol_]] = vertex_now_;
    information_vertex_now1.left_border = active_symbol_;
    vertexes_[information_vertex_now1.parent].edge[all_text_[new_vertex.left_border]] = vertexes_.size() - 1;
    information_vertex_now1.parent = vertexes_.size() - 1;
    vertexes_.emplace_back(Vertex(vertexes_.size() - 1, index_string_, all_text_.size() - 1));// проверить -2 и -1

    auto &old_vertex = vertexes_[vertexes_.size() - 2];
    old_vertex.edge[all_text_[index_string_]] = vertexes_.size() - 1;
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    std::string a, b;
    std::cin >> a >> b;
    Ukkonen c;
    c.algorithm(a, b);
}
