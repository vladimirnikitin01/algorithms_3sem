/*Штирлиц и Мюллер стреляют по очереди. В очереди n человек, стоящих друг за другом.
 * Каждым выстрелом убивается один из стоящих. Кроме того, если у кого-то из стоящих в очереди убиты все его соседи,
 * то этот человек в ужасе убегает. Проигрывает тот, кто не может ходить. Первым стреляет Штирлиц.
 * Требуется определить, кто выиграет при оптимальной игре обеих сторон, и если победителем будет Штирлиц,
 * то найти все возможные первые ходы, ведущие к его победе.*/
#include <iostream>
#include <vector>

class SpragaGrande {
private:
    std::vector<uint64_t> sg_;

    static uint64_t mex_(const std::vector<bool> &used);

public:
    explicit SpragaGrande(uint64_t n);

    std::vector<uint64_t> give_sg();
};

SpragaGrande::SpragaGrande(uint64_t n) {
    //базовые значения, ручками определяем их
    sg_.resize(n + 1);
    sg_[2] = 1;
    if (n >= 3) {
        sg_[3] = 2;
    }

    for (uint64_t i = 5; i < n + 1; ++i) {
        std::vector<bool> used(n + 1, false);
        used[sg_[i - 2]] = true;
        used[sg_[i - 1]] = true;
        for (uint64_t j = 2; j <= i - 3; ++j) {
            used[sg_[j] ^ sg_[i - 1 - j]] = true;
        }
        sg_[i] = mex_(used);
    }
}

uint64_t SpragaGrande::mex_(const std::vector<bool> &used) {
    for (uint64_t i = 0; i < used.size(); ++i) {
        if (used[i] == false) {
            return i;
        }
    }
    return used.size();
}

std::vector<uint64_t> SpragaGrande::give_sg() {
    return sg_;
}

void algorithm(uint64_t n) {
    SpragaGrande cruel_game(n);
    auto sg_now = cruel_game.give_sg();
    if (sg_now.back() == 0) {
        std::cout << "Mueller";
        return;
    }

    std::cout << "Schtirlitz" << "\n";
    for (uint64_t i = 1; i < sg_now.size(); ++i) {
        if ((sg_now[i - 1] ^ sg_now[sg_now.size() - 1 - i]) == 0) {
            std::cout << i << "\n";
        }
    }
}

int main() {
    uint64_t n;
    std::cin >> n;
    algorithm(n);
}