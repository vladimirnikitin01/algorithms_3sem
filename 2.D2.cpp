/*Арсений настолько потерялся в своих снах, что попал на передел земельных участков на планете Ка-Пэкс.
Тут Арсений предложил делить по-научному. Во-первых, устроить аукцион,
на котором будут распределяться существующие участки, по два на каждого делегата.А после чего определить новые участки так.
Пусть A и B — два участка.
Для любых положительных u и v (цен, заявленных за каждый из участков на аукционе) рассмотрим фигуру u*A + v*B,
образованную точками uP + vQ, где P пробегает все точки из A, а Q — из B. Несложно видеть,
что построенная так фигура будет снова выпуклой и ограниченной, зависит от выбора начала системы координат,
и более того, при переносе начала координат в другую точку этот участок также подвергается параллельному переносу на некоторый вектор,
и потому его площадь S(u; v) корректно определена. Так как никто из жителей планеты Ка-Пэкс ничего толком не понял,
то решение возражений не вызвало. Аукцион прошёл быстро. Только с подсчётом итоговой площади вышли проблемы.
Арсений помнил, что функция S(u; v) всегда является однородным квадратным многочленом: S(u; v) = Xu2+2Yuv + Zv2 для всех u; v > 0,
причем коэффициент X всегда равен площади S(A), а Z — площади B.
А вот коэффициент Y — так называемую смешанную площадь S(A; B) выпуклых фигур A и B — Арсений подсчитать не мог. А вы сможете?
Формат ввода
Входной файл состоит из описаний двух выпуклых многоугольников — A и B. Описание каждого из них начинается со строки,
содержащей число вершин N (2 < N ≤ 105). Затем следуют N строк, каждая содержит координаты очередной вершины (xi; yi),
разделенные пробелами. Вершины задаются в порядке обхода против часовой стрелки, начиная с любой из них;
все координаты целые и не превосходят по модулю 108. Последовательные три вершины не могут лежать на одной прямой.
Формат вывода
Единственная строка выходного файла должна содержать искомую смешанную площадь
 без лидирующих нулей с шестью цифрами после десятичной точки.*/

#include <iostream>
#include <cmath>
#include <vector>
#include <iomanip>

const long double PI = 3.1415926535897932384626;

struct Point {
    int64_t x;
    int64_t y;

    Point(const int64_t a, const int64_t b) : x(a), y(b) {}

    Point() : x(0), y(0) {}
};

std::istream &operator>>(std::istream &in, Point &point_now) {
    int64_t a, b;
    in >> a >> b;
    point_now = Point(a, b);
    return (in);
}

bool operator==(const Point &a, const Point &b) {
    return a.x == b.x && a.y == b.y;
}

bool operator!=(const Point &a, const Point &b) {
    return (!(a == b));
}

struct Vector {
    int64_t x;
    int64_t y;

    Vector(const int64_t &a, const int64_t &b) : x(a), y(b) {}

    Vector() : x(0), y(0) {}

    Vector(const Point &start, const Point &end) : x(end.x - start.x), y(end.y - start.y) {}
};

long double polar_angle(const Vector &now) {
    long double f = atan2l(now.y, now.x);
    if (f < 0) {
        f += 2 * PI;
    }
    return f;
}

bool comp(const Vector &a, const Vector &b) {
    return polar_angle(a) < polar_angle(b);
}


class MinkowskiAddition {
private:
    std::vector<Vector> array_vector_;
    Point position_now_;

    std::vector<Point> first_polygon_;
    std::vector<Point> second_polygon_;
    std::vector<Point> array_vertex_;
    uint64_t index_start_first_ = 0;
    uint64_t index_start_second_ = 0;
    std::vector<Vector> array_vector_first_;
    std::vector<Vector> array_vector_second_;

    static uint64_t search_start_(const std::vector<Point> &now, uint64_t &index);

    static void do_array_vector_of_polygon_(std::vector<Vector> &array_vector_now, const std::vector<Point> &polygon_now,
                                            uint64_t index_start);

    void do_array_vectors_();

public:
    MinkowskiAddition(const std::vector<Point> &first, const std::vector<Point> &second);

    std::vector<Point> give_polygon();
};


uint64_t MinkowskiAddition::search_start_(const std::vector<Point> &now, uint64_t &index) {
    index = 0;
    int64_t min_x = now.front().x;
    int64_t min_y = now.front().y;

    for (uint64_t i = 1; i < now.size(); ++i) {
        if (std::tie(now[i].y, now[i].x) < std::tie(min_y, min_x)) {
            min_y = now[i].y;
            min_x = now[i].x;
            index = i;
        }
    }
    return index;
}

void MinkowskiAddition::do_array_vector_of_polygon_(std::vector<Vector> &array_vector_now,
                                                    const std::vector<Point> &polygon_now,
                                                    uint64_t index_start) {
    array_vector_now.reserve(polygon_now.size());
    for (uint64_t i = index_start; i + 1 < polygon_now.size(); ++i) {
        array_vector_now.emplace_back(Vector(polygon_now[i], polygon_now[i + 1]));
    }
    array_vector_now.emplace_back(Vector(polygon_now.back(), polygon_now.front()));
    for (uint64_t i = 0; i + 1 <= index_start; ++i) {
        array_vector_now.emplace_back(Vector(polygon_now[i], polygon_now[i + 1]));
    }
}

void MinkowskiAddition::do_array_vectors_() {
    do_array_vector_of_polygon_(array_vector_first_, first_polygon_, index_start_first_);
    do_array_vector_of_polygon_(array_vector_second_, second_polygon_, index_start_second_);
    uint64_t i = 0;
    uint64_t j = 0;

    while (i < array_vector_first_.size() && j < array_vector_second_.size()) {
        if (comp(array_vector_first_[i], array_vector_second_[j])) {
            array_vector_.emplace_back(array_vector_first_[i]);
            ++i;
        } else {
            array_vector_.emplace_back(array_vector_second_[j]);
            ++j;
        }
    }

    while (j < array_vector_second_.size()) {
        array_vector_.emplace_back(array_vector_second_[j]);
        ++j;
    }

    while (i < array_vector_first_.size()) {
        array_vector_.emplace_back(array_vector_first_[i]);
        ++i;
    }
}


MinkowskiAddition::MinkowskiAddition(const std::vector<Point> &first, const std::vector<Point> &second) {
    first_polygon_ = first;
    second_polygon_ = second;

    array_vector_.reserve(first_polygon_.size() + second_polygon_.size());
    Point start_first = first_polygon_[search_start_(first_polygon_, index_start_first_)];
    Point start_second = second_polygon_[search_start_(second_polygon_, index_start_second_)];
    do_array_vectors_();

    position_now_ = Point(start_first.x + start_second.x, start_first.y + start_second.y);
    array_vertex_.reserve(first_polygon_.size() + second_polygon_.size());
    array_vertex_.emplace_back(position_now_);

    for (uint64_t i = 0; i < array_vector_.size() - 1; ++i) {
        position_now_.x += array_vector_[i].x;
        position_now_.y += array_vector_[i].y;
        array_vertex_.emplace_back(position_now_);
    }
}

std::vector<Point> MinkowskiAddition::give_polygon() {
    return array_vertex_;
}


std::vector<Point> do_polygon(uint64_t n) {
    std::vector<Point> now_polygon(n);
    for (uint64_t i = 0; i < n; ++i) {
        std::cin >> now_polygon[i];
    }
    return now_polygon;
}

std::vector<Point> operator+(const std::vector<Point> &x, const std::vector<Point> &y) {
    MinkowskiAddition new_polygon(x, y);
    return (new_polygon.give_polygon());
}

long double area(const std::vector<Point> &array_vertex_now) {
    long double sum = 0;
    for (uint64_t i = 0; i < array_vertex_now.size() - 1; ++i) {
        sum += array_vertex_now[i].x * array_vertex_now[i + 1].y;
        sum -= array_vertex_now[i].y * array_vertex_now[i + 1].x;
    }
    sum += array_vertex_now.back().x * array_vertex_now[0].y;
    sum -= array_vertex_now.back().y * array_vertex_now[0].x;
    return std::abs(sum) / 2;
}

long double algorithm(const std::vector<Point> &first, const std::vector<Point> &second) {
    long double mixed_area = area(first + second) - area(second) - area(first);
    return mixed_area / 2;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    uint64_t n;
    std::cin >> n;
    auto first_polygon = do_polygon(n);
    std::cin >> n;
    auto second_polygon = do_polygon(n);
    std::cout << std::fixed << std::setprecision(6) << algorithm(first_polygon, second_polygon);
}
