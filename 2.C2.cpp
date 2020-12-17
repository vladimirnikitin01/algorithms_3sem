/*Арсений и Илья затеяли странную игру. Сначала Илья придумывает,
какие бы точки он хотел видеть внутри воображаемого яблока.
А затем он спрашивает Арсения, что если бы червячок находился в некоторой точке внутри такого минимально возможного яблока,
то сколько бы ему пришлось бы скушать мякоти, чтобы выбраться на поверхность.
Формат ввода
На первой строке число N (4 ≤ N ≤ 1,000). Далее N строк содержат координаты точек для яблока.
Никакие четыре из них не лежат в одной плоскости. Далее число запросов q (1 ≤ Q ≤ 105) и Q строк,
содержащих точки-запросы. Все точки запросы гарантированно внутри минимально возможного воображаемого яблока.
Все координаты целые, по модулю не более 10 000.
Формат вывода
Для каждого запроса выведите число с четырьмя знаками после запятой – расстояния
до поверхности минимально возможного воображаемого яблока.*/

#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <limits>
#include <iomanip>

const long double PI = 3.1415926535897932384626;

static uint64_t square(int64_t number) {
    return number * number;
}

struct Point {
    int64_t x;
    int64_t y;
    int64_t z;

    Point(int64_t a, int64_t b, int64_t c) : x(a), y(b), z(c) {}

    Point() : x(0), y(0), z(0) {}
};

std::istream &operator>>(std::istream &in, Point &point_now) {
    int64_t a, b, c;
    in >> a >> b >> c;
    point_now = Point(a, b, c);
    return (in);
}

bool operator==(const Point &a, const Point &b) {
    return a.x == b.x && a.y == b.y && a.z == b.z;
}

bool operator!=(const Point &a, const Point &b) {
    return !(a == b);
}

bool operator<(const Point &a, const Point &b) {
    return std::tie(a.z, a.x, a.y) < std::tie(b.z, b.x, b.y);
}


long double distance(const Point &a, const Point &b) {
    long double s = sqrtl(square(a.x - b.x) + square(a.y - b.y) + square(a.z - b.z));
    return (s);
}


struct Vector {
    int64_t x;
    int64_t y;
    int64_t z;

    Vector(int64_t a, int64_t b, int64_t c) : x(a), y(b), z(c) {}

    Vector() : x(0), y(0), z(0) {}

    Vector(const Point &start, const Point &end) : x(end.x - start.x), y(end.y - start.y), z(end.z - start.z) {}
};

struct Edge {
    Point start;
    Point end;
    Point vertex_3_in_face;
    int64_t A = 0, B = 0, C = 0, D = 0;//уравнения плоскости
    Edge(const Point &first, const Point &second, const Point &third) : start(first), end(second),
                                                                        vertex_3_in_face(third) {
        A = first.y * (second.z - third.z) + second.y * (third.z - first.z) + third.y * (first.z - second.z);
        B = first.z * (second.x - third.x) + second.z * (third.x - first.x) + third.z * (first.x - second.x);
        C = first.x * (second.y - third.y) + second.x * (third.y - first.y) + third.x * (first.y - second.y);
        D = -first.x * (second.y * third.z - third.y * second.z) - second.x * (third.y * first.z - first.y * third.z) -
            third.x * (first.y * second.z - second.y * first.z);
    }

    Edge() : start(Point()), end(Point()), vertex_3_in_face(Point()) {}
};

bool operator<(const Edge &a, const Edge &b) {//for map
    Point start_a = std::min(a.start, a.end);
    Point start_b = std::min(b.start, b.end);
    Point end_a = std::max(a.start, a.end);
    Point end_b = std::max(b.start, b.end);
    return std::tie(start_a, end_a) < std::tie(start_b, end_b);
}


struct Face {
    Point first;
    Point second;
    Point third;
    int64_t A, B, C, D;

    explicit Face(const Edge &edge_now) : first(edge_now.start), second(edge_now.end), third(edge_now.vertex_3_in_face),
                                          A(edge_now.A),
                                          B(edge_now.B), C(edge_now.C), D(edge_now.D) {}

    Face(const Point &first, const Point &second, const Point &third) : first(first), second(second),
                                                                        third(third) {
        A = first.y * (second.z - third.z) + second.y * (third.z - first.z) + third.y * (first.z - second.z);
        B = first.z * (second.x - third.x) + second.z * (third.x - first.x) + third.z * (first.x - second.x);
        C = first.x * (second.y - third.y) + second.x * (third.y - first.y) + third.x * (first.y - second.y);
        D = -first.x * (second.y * third.z - third.y * second.z) - second.x * (third.y * first.z - first.y * third.z) -
            third.x * (first.y * second.z - second.y * first.z);
    }
};

class ConvexHull_3D {
private:
    std::map<Edge, int> map_edge;
    //множество незакрытых ребер, а также количество которое мы их трогали, int сколько раз мы их трогали
    std::vector<Point> array_points;
    //массив всех точек, нужно для поиска новых граней
    std::vector<Face> array_face;
    //массив всех граней

    Point search_first_point_();

    Point search_second_point_(const Point &first);

    Point search_third_point_(const Point &a, const Point &b);

    static long double do_correct_corner_(const Edge &edge_active, const Point &point_now);

    static bool check_on_obtuse_angle_(const Edge &edge_active, const Point &point_now);

    static long double cos_between_the_planes_(const Face &first);

    static long double cos_between_the_planes_(const Face &first, const Face &second);

    static long double distance_from_point_to_face_(const Face &face_now, const Point &point_now);

    void insert_new_edges_(const Point &Point_now, const Edge &edge_now);

    void build_first_face_();

    void build_farther_face_();

public:
    explicit ConvexHull_3D(uint64_t n);

    void add_point(const Point &a);

    void build();

    void algorithm(uint64_t k);

    static Point projection_point(const Edge &edge_active, const Point &point_now);
};

ConvexHull_3D::ConvexHull_3D(uint64_t n) {
    array_points.reserve(n);
}

void ConvexHull_3D::add_point(const Point &a) {
    array_points.push_back(a);
}

Point ConvexHull_3D::search_first_point_() {
    uint64_t index_min_point = 0;
    for (uint64_t i = 1; i < array_points.size(); ++i) {
        Point &point_now = array_points[i];
        if (point_now < array_points[index_min_point]) {
            index_min_point = i;
        }
    }
    return array_points[index_min_point];
}

Point ConvexHull_3D::search_second_point_(const Point &first) {
    long double min_tan = std::numeric_limits<long double>::max();
    Point result;
    for (auto &point_now : array_points) {
        int64_t relative_x = point_now.x - first.x;
        int64_t relative_y = point_now.y - first.y;
        int64_t relative_z = point_now.z - first.z;
        if (relative_x == 0 && relative_y == 0) {
            continue;//чтобы не было прямой || оси z
        }
        long double tan = relative_z / sqrtl(relative_x * relative_x + relative_y * relative_y);
        if (tan < min_tan) {
            min_tan = tan;
            result = point_now;
        }
    }
    return result;
}

long double ConvexHull_3D::cos_between_the_planes_(const Face &first, const Face &second) {
    long double numerator = std::abs(first.A * second.A + first.B * second.B + first.C * second.C);
    numerator /= sqrtl(first.A * first.A + first.B * first.B + first.C * first.C);
    numerator /= sqrtl(second.A * second.A + second.B * second.B + second.C * second.C);
    return numerator;
}

long double ConvexHull_3D::cos_between_the_planes_(const Face &first) {
    const Vector normal_vector_Oxy(0, 0, 1);//направляющий вектор для плоскости Oxy
    long double numerator = std::abs(
            first.A * normal_vector_Oxy.x + first.B * normal_vector_Oxy.y + first.C * normal_vector_Oxy.z);
    numerator /= sqrtl(first.A * first.A + first.B * first.B + first.C * first.C);
    numerator /= sqrtl(normal_vector_Oxy.x * normal_vector_Oxy.x + normal_vector_Oxy.y * normal_vector_Oxy.y +
                       normal_vector_Oxy.z * normal_vector_Oxy.z);
    return numerator;
}

Point ConvexHull_3D::search_third_point_(const Point &a, const Point &b) {
    long double cos = -2;
    Point result;
    for (auto &point_now : array_points) {
        if (point_now != a && point_now != b) {
            Face edge_now(a, b, point_now);
            long double cos_now = cos_between_the_planes_(edge_now);
            if (cos_now > cos) {
                cos = cos_now;
                result = point_now;
            }
        }
    }
    return result;
}

long double ConvexHull_3D::do_correct_corner_(const Edge &edge_active, const Point &point_now) {
    long double cos_now = cos_between_the_planes_(Face(edge_active),
                                                  Face(edge_active.start, edge_active.end, point_now));
    //но тут cos острого угла, а у нас может быть тупой на самом деле

    long double corner = acosl(cos_now);
    if (check_on_obtuse_angle_(edge_active, point_now)) {//значит тупой угол
        return PI - corner;
    }
    return corner;
}

Point ConvexHull_3D::projection_point(const Edge &edge_active, const Point &point_now) {
    Vector direction_vector(edge_active.start, edge_active.end);
    int64_t x2 = direction_vector.x;
    int64_t y2 = direction_vector.y;
    int64_t z2 = direction_vector.z;
    //это координаты направляюещго вектора
    int64_t x0 = point_now.x;
    int64_t y0 = point_now.y;
    int64_t z0 = point_now.z;
    //координаты точки
    int64_t x1 = edge_active.start.x;
    int64_t y1 = edge_active.start.y;
    int64_t z1 = edge_active.start.z;
    //эта точка лежит на прямой L
    // теперь будем пользовать уравнением прямой в пространстве и найдем проекции точки point_now на прямую L, которая задается напр вектором и точкой
    long double t = (static_cast<long double>(x2 * x0 + y2 * y0 + z2 * z0 - x2 * x1 - y2 * y1 - z2 * z1)) /
                    (x2 * x2 + y2 * y2 + z2 * z2);
    long double x_pr = x2 * t + x1;
    long double y_pr = y2 * t + y1;
    long double z_pr = z2 * t + z1;
    return Point(x_pr, y_pr, z_pr);
}


bool ConvexHull_3D::check_on_obtuse_angle_(const Edge &edge_active, const Point &point_now) {
    Point pr = projection_point(edge_active, point_now);
    //получили координаты точки пересечения
    //теперь строим треугольник abc
    long double a = distance(point_now, Point(pr.x, pr.y, pr.z));
    long double b = distance(edge_active.vertex_3_in_face, Point(pr.x, pr.y, pr.z));
    long double c = distance(edge_active.vertex_3_in_face, point_now);
    return a * a + b * b < c * c;
}

void ConvexHull_3D::build_first_face_() {
    Point a = search_first_point_();
    Point b = search_second_point_(a);
    Point c = search_third_point_(a, b);
    //мы смогли построить первую базовую плоскость
    array_face.emplace_back(a, b, c);
    map_edge.insert({Edge(a, b, c), 1});
    map_edge.insert({Edge(a, c, b), 1});
    map_edge.insert({Edge(b, c, a), 1});
}

void ConvexHull_3D::insert_new_edges_(const Point &Point_now, const Edge &edge_now) {
    array_face.emplace_back(Point_now, edge_now.start, edge_now.end);
    Edge new_edge1(edge_now.start, Point_now, edge_now.end);
    Edge new_edge2(edge_now.end, Point_now, edge_now.start);
    if (map_edge[new_edge1] >= 1) {
        map_edge.erase(new_edge1);
    } else {
        ++map_edge[new_edge1];
    }
    if (map_edge[new_edge2] >= 1) {
        map_edge.erase(new_edge2);
    } else {
        ++map_edge[new_edge2];
    }
}

void ConvexHull_3D::build_farther_face_() {//строим еще одну грань
    auto it = map_edge.begin();
    Edge edge_now = (*it).first;
    long double corner = -2;
    Point result;
    for (auto &point_now : array_points) {
        //ищем точку, чтобы сделать еще одну грань для этого ребра
        if (point_now != edge_now.start && point_now != edge_now.end && point_now != edge_now.vertex_3_in_face) {
            long double corner_now = do_correct_corner_(edge_now, point_now);
            if (corner_now > corner) {
                corner = corner_now;
                result = point_now;
            }
        }
    }
    map_edge.erase(it);
    insert_new_edges_(result, edge_now);
}

void ConvexHull_3D::build() {
    build_first_face_();
    while (!map_edge.empty()) {
        build_farther_face_();
    }
}

long double ConvexHull_3D::distance_from_point_to_face_(const Face &face_now, const Point &point_now) {
    long double numerator = std::abs(
            face_now.A * point_now.x + face_now.B * point_now.y + face_now.C * point_now.z + face_now.D);
    numerator /= sqrtl(face_now.A * face_now.A + face_now.B * face_now.B + face_now.C * face_now.C);
    return numerator;
}

void ConvexHull_3D::algorithm(uint64_t k) {
    Point point_now;
    for (uint64_t i = 0; i < k; ++i) {
        std::cin >> point_now;
        long double min_distance = std::numeric_limits<long double>::max();
        for (auto &face_now : array_face) {
            auto distance_now = distance_from_point_to_face_(face_now, point_now);
            if (distance_now < min_distance) {
                min_distance = distance_now;
            }
        }
        std::cout << std::fixed << std::setprecision(4) << min_distance << '\n';
    }
}

int main() {
    std::ios::sync_with_stdio(false);
    std::cin.tie(nullptr);
    uint64_t n;
    Point point_input;
    std::cin >> n;
    ConvexHull_3D little_hull(n);
    for (uint64_t i = 0; i < n; ++i) {
        std::cin >> point_input;
        little_hull.add_point(point_input);
    }
    little_hull.build();
    std::cin >> n;
    little_hull.algorithm(n);
}

