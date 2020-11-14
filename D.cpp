/*Дана строка длины n. Найти количество ее различных подстрок. Используйте суффиксный массив.
Построение суффиксного массива выполняйте за O(n log n). Вычисление количества различных подстрок выполняйте за O(n).*/

#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

struct Information {
    int64_t value = 0;
    uint64_t index = 0;

    Information(const int64_t value_in, const uint64_t index_in) : value(value_in), index(index_in) {}

    Information() = default;
};

bool operator<(const Information &a, const Information &b) {
    return a.value < b.value;
}

class SuffixArray {
private:
    const std::string s;
    const std::string s1;
    //s1=s+"$"
    //s1 for lcp
    std::vector<uint64_t> suffix_array;
    std::vector<uint64_t> class_of_symbol;
    std::vector<uint64_t> suffix;
    uint64_t number_class = 0;

    void buildSuffixArrayFirstStep();

    void buildSuffixArraySecondStep();

    void buildClasses(std::vector<uint64_t> &class_symbols_new, uint64_t k);

    void sortWithRadix(std::vector<uint64_t> &suffix_array_for_second_part, uint64_t k);

public:
    SuffixArray(const std::string &s1_);

    void buildSuffixArray();


    std::vector<uint64_t> buildLcp();

    std::vector<uint64_t> giveSuffix();

};


void SuffixArray::buildSuffixArrayFirstStep() {
    std::vector<Information> array_of_symbols(s.size());
    for (uint64_t i = 0; i < s.size(); ++i) {
        array_of_symbols[i] = {s[i], i};
    }
    std::sort(array_of_symbols.begin(), array_of_symbols.end());
    for (uint64_t i = 0; i < s.size(); ++i) {
        suffix_array[i] = array_of_symbols[i].index;
    }
    class_of_symbol[suffix_array[0]] = 0;
    for (uint64_t i = 0; i + 1 < s.size(); ++i) {
        if (s[suffix_array[i + 1]] != s[suffix_array[i]]) {
            ++number_class;
        }
        class_of_symbol[suffix_array[i + 1]] = number_class;
    }
}

void SuffixArray::sortWithRadix(std::vector<uint64_t> &suffix_array_for_second_part, uint64_t k) {
    for (uint64_t i = 0; i < s.size(); ++i) {
        if (suffix_array[i] < k) {
            suffix_array_for_second_part[i] = s.size() + suffix_array[i] - k;
        } else {
            suffix_array_for_second_part[i] = 0 + suffix_array[i] - k;
        }
    }
    std::vector<int64_t> count(s.size(), 0);
    for (uint64_t i = 0; i < s.size(); ++i) {
        ++count[class_of_symbol[suffix_array_for_second_part[i]]];
    }
    for (uint64_t i = 0; i < number_class; ++i) {
        count[i + 1] += count[i];
    }
    for (int64_t i = s.size() - 1; i >= 0; --i) {
        --count[class_of_symbol[suffix_array_for_second_part[i]]];
        suffix_array[count[class_of_symbol[suffix_array_for_second_part[i]]]] = suffix_array_for_second_part[i];
    }
}

void SuffixArray::buildSuffixArraySecondStep() {
    std::vector<uint64_t> suffix_array_for_second_part(s.size());
    std::vector<uint64_t> class_symbols_new(s.size());
    for (uint64_t k = 1;; k <<= 1) {
        sortWithRadix(suffix_array_for_second_part, k);

        buildClasses(class_symbols_new, k);
        if (number_class + 1 == s.size()) {
            break;
        }
        class_of_symbol = class_symbols_new;
    }
}

void SuffixArray::buildSuffixArray() {
    suffix_array.resize(s.size());
    class_of_symbol.resize(s.size());
    buildSuffixArrayFirstStep();
    //second step
    buildSuffixArraySecondStep();
    //second step
    suffix_array.erase(suffix_array.begin());
    suffix = std::move(suffix_array);
}


std::vector<uint64_t> SuffixArray::buildLcp() {
    // do the reverse suffix_array
    std::vector<uint64_t> reserve_suffix_array(suffix.size());
    for (uint64_t i = 0; i < suffix.size(); ++i) {
        reserve_suffix_array[suffix[i]] = i;
    }
    std::vector<uint64_t> lcp(s1.size(), 0);
    int64_t lcp_now = 0;
    for (uint64_t i = 0; i < s1.size(); ++i) {
        if (reserve_suffix_array[i] != 0) {
            int64_t the_previous_object_in_suffix_array = suffix[reserve_suffix_array[i] - 1];
            while (i + lcp_now < s1.size() && the_previous_object_in_suffix_array + lcp_now < s1.size()
                   && s1[i + lcp_now] == s1[the_previous_object_in_suffix_array + lcp_now]) {
                ++lcp_now;
            }
            lcp[reserve_suffix_array[i]] = lcp_now;
            lcp_now = std::max(int64_t(0), lcp_now - 1);
        } else {
            lcp_now = 0;
        }
    }
    return lcp;
}

SuffixArray::SuffixArray(const std::string &s1_) : s(s1_ + "$"), s1(s1_) {}

void SuffixArray::buildClasses(std::vector<uint64_t> &class_symbols_new, const uint64_t k) {
    number_class = 0;
    class_symbols_new[suffix_array[0]] = number_class;
    for (uint64_t i = 0; i + 1 < s.size(); ++i) {
        if (class_of_symbol[suffix_array[i + 1]] != class_of_symbol[suffix_array[i]]
            || class_of_symbol[(suffix_array[i + 1] + k) % s.size()]
               != class_of_symbol[(suffix_array[i] + k) % s.size()]) {
            ++number_class;
        }
        class_symbols_new[suffix_array[i + 1]] = number_class;
    }
}

std::vector<uint64_t> SuffixArray::giveSuffix() {
    return suffix;
}

uint64_t algorithm(const std::string &s) {
    SuffixArray a(s);
    a.buildSuffixArray();
    auto lcp = a.buildLcp();
    uint64_t sum = 0;
    auto suffix = a.giveSuffix();
    for (uint64_t i = 0; i < s.size(); ++i) {
        sum += (s.size() - suffix[i] - lcp[i]);
    }
    return sum;
}

int main() {
    std::string s;
    std::cin >> s;
    std::cout << algorithm(s);
}
