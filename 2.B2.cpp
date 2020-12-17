/*Арсений продолжает спать. Теперь ему снится кроличья ферма на планете Ка-Пэкс.
Правда, ферма у него так себе — одни кроличьи норы в пустом поле. Чтобы её хоть как-то облагородить,
Арсений решил поставить забор так, чтобы все норки оказались внутри огражденной территории,
а досок он потратил как можно меньше. Помогите Арсению найти длину забора, чтобы он мог уже заказывать стройматериалы!
Формат ввода
В первой строке вводится число N (3 ≤ N ≤ 105).
Следующие N строк содержат пары целых чисел x и y (-109 ≤ x, y ≤ 109) — координаты кроличьих нор.

Формат вывода
Одно вещественное число — длину забора — с максимально возможной точностью.*/

#include <iostream>
#include <cmath>
#include <algorithm>
#include <vector>
#include <iomanip>

static uint64_t square(int64_t number) {
    return number * number;
}

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
    return !(a == b);
}

bool comp(const Point &a, const Point &b) {
    return std::tie(a.x, a.y) < std::tie(b.x, b.y);
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

double distance(const Point &a, const Point &b) {
    double s = sqrt(square(a.x - b.x) + square(a.y - b.y));
    return s;
}

class ConvexHull {
private:
    uint64_t size = 0;
    std::vector<Point> all_points_;
    std::vector<Point> upper_part_;
    std::vector<Point> lower_part_;
public:

    explicit ConvexHull(const std::vector<Point> &all_points_);

    ConvexHull() = default;

    void insert_upper_part(const Point &now_point);

    void insert_lower_part(const Point &now_point);

    double total_distance();

    double algorithm();
};

ConvexHull::ConvexHull(const std::vector<Point> &all_points_) : all_points_(all_points_), size(all_points_.size()) {}

void ConvexHull::insert_upper_part(const Point &now_point) {
    while (upper_part_.size() >= 2) {
        uint64_t i = upper_part_.size() - 1;
        if (sign_cross_product(upper_part_[i - 1], upper_part_[i], now_point) >= 0) {
            upper_part_.pop_back();
        } else {
            break;
        }
    }
    upper_part_.emplace_back(now_point);
}

void ConvexHull::insert_lower_part(const Point &now_point) {
    while (lower_part_.size() >= 2) {
        uint64_t i = lower_part_.size() - 1;
        if (sign_cross_product(lower_part_[i - 1], lower_part_[i], now_point) <= 0) {
            lower_part_.pop_back();
        } else {
            break;
        }
    }
    lower_part_.emplace_back(now_point);
}

double ConvexHull::total_distance() {
    double sum = 0;
    for (uint64_t i = 0; i <= upper_part_.size() - 2; ++i) {
        sum += distance(upper_part_[i], upper_part_[i + 1]);
    }
    for (uint64_t i = 0; i <= lower_part_.size() - 2; ++i) {
        sum += distance(lower_part_[i], lower_part_[i + 1]);
    }
    return sum;
}

double ConvexHull::algorithm() {
    std::sort(all_points_.begin(), all_points_.end(), comp);
    auto last_element = std::unique(all_points_.begin(), all_points_.end());
    all_points_.erase(last_element, all_points_.end());
    size = all_points_.size();

    upper_part_.reserve(size);
    lower_part_.reserve(size);

    upper_part_.emplace_back(all_points_[0]);
    lower_part_.emplace_back(all_points_[0]);
    Point A = all_points_[0];
    Point B = all_points_[all_points_.size() - 1];

    for (uint64_t i = 0; i + 1 < size; ++i) {
        if (sign_cross_product(A, B, all_points_[i]) == 1) {//upper
            insert_upper_part(all_points_[i]);
        } else if (sign_cross_product(A, B, all_points_[i]) == -1) {//lower
            insert_lower_part(all_points_[i]);
        }
    }
    insert_upper_part(B);
    insert_lower_part(B);

    return total_distance();
}


int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    uint64_t n;
    std::cin >> n;
    Point first;
    std::vector<Point> all_points(n);
    for (uint64_t i = 0; i < n; ++i) {
        std::cin >> first;
        all_points[i] = first;
    }
    ConvexHull convex_hull(all_points);
    std::cout << std::setprecision(17) << convex_hull.algorithm();
}
