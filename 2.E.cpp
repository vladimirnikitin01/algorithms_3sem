/*Арсений открыл эзотерический салон в свои снах на планете Ка-Пэкс. У Арсения всё будет хорошо.
А вот у его клиентов — не факт. Если хотя бы какие-нибудь их палочки в гадании "Мокусо Дзэй" пересеклись,
то день точно сложится удачно. А если нет — то стоит ждать беды. Чтобы точнее сказать, что конкретно пойдет хорошо в этот день,
нужно знать, какие именно палочки пересекаются. Помогите Арсению узнать, как пройдет день у его клиентов.
Формат ввода
Палочка представляется как отрезок прямой. В первой строке входного файла записано число N (1 ≤ N ≤ 125   000)
 — количество палочек в гадании.
Следующие N строк содержат описания палочек: (i + 1)-я строка содержит координаты концов i-й палочки —
 целые числа x1, y1, x2, y2 (-10   000 ≤ x1, y1, x2, y2 ≤ 10   000).
Формат вывода
В первой строке выходного файла выведите слово "YES", если день сложится удачно.
Вторая строка должна содержать числа i и j — номера палочек, которые пересекаются.
Если день пройдет плохо, выведите в единственной строке выходного файла "NO".*/

#include <iostream>
#include <algorithm>
#include <vector>
#include <set>
#include <tuple>
#include <optional>

struct MyOutput {
    uint64_t first_segment;
    uint64_t second_segment;

    explicit MyOutput(const uint64_t first_segment_ = 0, const uint64_t second_segment_ = 0) : first_segment(
            first_segment_ + 1), second_segment(second_segment_ + 1) {
        if (first_segment_ > second_segment_) {
            std::swap(first_segment, second_segment);
        }
    }
};

std::ostream &operator<<(std::ostream &out, const MyOutput &output) {
    out << output.first_segment << " " << output.second_segment;
    return (out);
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

struct LineSegment {
    uint64_t number = 0;
    Point start;
    Point end;

    LineSegment(const Point &a, const Point &b, const uint64_t number_ = 0) : start(a), end(b), number(number_) {}

    LineSegment() : start(Point()), end(Point()) {}

    long double get_y_from_x(const int64_t x) const {
        if (x == start.x) {
            return start.y;
        }
        if (x == end.x) {
            return end.y;
        }
        if (end.x == start.x) {
            return start.y;
        }
        long double y = start.y + static_cast<long double>((x - start.x)) * (end.y - start.y) / (end.x - start.x);
        return y;
    }
};

struct EventPoint {
    int64_t x = 0;
    int64_t y = 0;
    uint64_t number_of_line_segment = 0;
    bool is_start = true;//1-start; 0-end
    EventPoint(const Point &a, uint64_t number_, const bool status_) : x(a.x), y(a.y), number_of_line_segment(number_),
                                                                       is_start(status_) {}
};

int64_t cross_product(const Point &Z, const Point &F, const Point &C) {
    //относительно точки Z строятся два вектора ZF, ZC
    //a первый вектор, b второй
    int64_t a_x = F.x - Z.x;
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

bool checking_for_straight_line(const Point &a, const Point &b, const Point &c, const Point &d) {
    //AB and CD
    //проверка по оси x and y
    //отрезки буду подаваться уже слева направо //
    auto a_y = a.y;
    auto b_y = b.y;
    auto c_y = c.y;
    auto d_y = d.y;
    if (a_y > b_y) {
        std::swap(a_y, b_y);
    }
    if (c_y > d_y) {
        std::swap(c_y, d_y);
    }

    return (std::max(a.x, c.x) <= std::min(b.x, d.x) && std::max(a_y, c_y) <= std::min(b_y, d_y));
}


bool intersection_two_segments(const Point &a, const Point &b, const Point &c, const Point &d) {
    //AB and CD
    return sign_cross_product(a, b, c) * sign_cross_product(a, b, d) <= 0 &&
           sign_cross_product(d, c, a) * sign_cross_product(d, c, b) <= 0 && checking_for_straight_line(a, b, c, d);
}

bool cmp_event_point(const EventPoint &a, const EventPoint &b) {
    return std::tie(a.x, b.is_start, a.y) < std::tie(b.x, a.is_start, b.y);
}


struct cmp_line_segment {
    bool operator()(const LineSegment &a, const LineSegment &b) const {
        int64_t x = std::max(a.start.x, b.start.x);//теперь надо вычислить y
        long double y_a = a.get_y_from_x(x);
        long double y_b = b.get_y_from_x(x);
        return y_a < y_b;
    }
};

class ScanningLine {
private:
    std::vector<EventPoint> all_event_point_;
    std::vector<LineSegment> all_line_segments_;
    std::set<LineSegment, cmp_line_segment> set_line_segments_;
    uint64_t n;

    std::optional<MyOutput> processing_start_(uint64_t number_segment);

    std::optional<MyOutput> processing_end_(uint64_t number_segment);

public:
    explicit ScanningLine(uint64_t size);

    std::optional<MyOutput> algorithm();
};

ScanningLine::ScanningLine(const uint64_t size) : n(size) {
    all_event_point_.reserve(2 * size);
    all_line_segments_.reserve(size);

    for (uint64_t i = 0; i < n; ++i) {
        Point first, second;
        std::cin >> first >> second;

        if (first.x > second.x) {//наши отрезки всегда идут слева направо
            std::swap(first, second);
        }

        LineSegment now_segment(first, second, i);
        all_line_segments_.emplace_back(now_segment);
        all_event_point_.emplace_back(first, i, 1);
        all_event_point_.emplace_back(second, i, 0);
    }
}


std::optional<MyOutput> ScanningLine::algorithm() {
    sort(all_event_point_.begin(), all_event_point_.end(), cmp_event_point);
    std::optional<MyOutput> result;
    for (auto &event_point_now : all_event_point_) {
        uint64_t number_segment = event_point_now.number_of_line_segment;
        if (event_point_now.is_start) {
            result = processing_start_(number_segment);
        } else {
            result = processing_end_(number_segment);
        }
        if (result != std::nullopt)
            return result;
    }
    return std::nullopt;
}

std::optional<MyOutput> ScanningLine::processing_start_(uint64_t number_segment) {
    auto it = set_line_segments_.lower_bound(all_line_segments_[number_segment]);
    if (it != set_line_segments_.end()) {// значит есть правее
        bool result = intersection_two_segments(all_line_segments_[number_segment].start,
                                                all_line_segments_[number_segment].end,
                                                it->start,
                                                it->end);
        if (result) {
            return MyOutput(it->number, number_segment);
        }
    }
    if (it != set_line_segments_.begin()) {//значит есть левее
        auto prev_it = std::prev(it, 1);
        bool result = intersection_two_segments(prev_it->start, prev_it->end,
                                                all_line_segments_[number_segment].start,
                                                all_line_segments_[number_segment].end);
        if (result) {
            return MyOutput(number_segment, prev_it->number);
        }
    }
    set_line_segments_.insert(all_line_segments_[number_segment]);
    return std::nullopt;
}

std::optional<MyOutput> ScanningLine::processing_end_(uint64_t number_segment) {
    auto it = set_line_segments_.find(all_line_segments_[number_segment]);
    if (it != set_line_segments_.begin() &&
        it != prev(set_line_segments_.end(), 1)) {// значит есть левее и правее
        auto next_it = std::next(it, 1);
        auto prev_it = std::prev(it, 1);
        bool result = intersection_two_segments(prev_it->start, prev_it->end, next_it->start,
                                                next_it->end);
        if (result) {
            return MyOutput(next_it->number, prev_it->number);
        }
        set_line_segments_.erase(it);
    }
    return std::nullopt;
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    uint64_t n;
    std::cin >> n;
    ScanningLine search_intersecting_segments(n);
    auto result = search_intersecting_segments.algorithm();

    if (result.has_value())
        std::cout << "YES" << "\n" << result.value();
    else {
        std::cout << "NO";
    }

}