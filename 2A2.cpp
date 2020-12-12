/*В своем сне Арсений добился многих успехов в строительстве.
Теперь ему вручили в руки карту и сказали построить трассу от точки A до точки B.
Арсений бы и рад помочь, но есть небольшая проблема — реки,
 которые опять же протекают строго напрямую от одной до другой точки,
так как чтобы провести трассу через них, придется строить мосты.
Помогите Арсению посчитать, сколько мостов ему придется построить.
Формат ввода
В первой строке вводятся координаты точек A и B — начала и конца трассы. Во второй строке вводится количество рек N.
В следующих N строках записаны координаты начала и конца реки. Все координаты целые и лежат от 0 до 105, рек не более 30000.
Формат вывода
Выведите количество рек, которые пересекаются с трассой.*/

#include <iostream>

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

int64_t cross_product(const Point &Z, const Point &F, const Point &C) {
    //относительно точки Z строятся два вектора ZF, ZC
    int64_t a_x = F.x - Z.x;//a первый вектор, b второй
    int64_t a_y = F.y - Z.y;
    int64_t b_x = C.x - Z.x;
    int64_t b_y = C.y - Z.y;
    return (a_x * b_y - a_y * b_x);
}

int sign_cross_product(const Point &Z, const Point &F, const Point &C) {
    auto result_cross_product = cross_product(Z, F, C);
    if (result_cross_product > 0) {
        return 1;
    } else if (result_cross_product < 0) {
        return -1;
    } else {
        return 0;
    }
}

bool intersection_two_segments(const Point &a, const Point &b, const Point &c, const Point &d) {
    //AB and CD
    return sign_cross_product(a, b, c) * sign_cross_product(a, b, d) <= 0 &&
           sign_cross_product(d, c, a) * sign_cross_product(d, c, b) <= 0;
}

uint64_t algorithm(const Point &A, const Point &B, const uint64_t n) {
    Point first, second;
    uint64_t sum = 0;
    for (uint64_t i = 0; i < n; ++i) {
        std::cin >> first >> second;
        sum += intersection_two_segments(A, B, first, second);
    }
    return sum;
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    Point first, second;
    uint64_t n;
    std::cin >> first >> second >> n;
    std::cout << algorithm(first, second, n);
}