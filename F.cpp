/*Заданы две строки s, t и целое число k. Рассмотрим множество всех таких непустых строк,
которые встречаются как подстроки в s и t одновременно.
 Найдите k-ую в лексикографическом порядке строку из этого множества.*/

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
    uint64_t number_class = 0;

    void buildSuffixArrayFirstStep();

    void buildSuffixArraySecondStep();

    void buildClassesFirst();

    void buildClassesSecond(std::vector<uint64_t> &class_symbols_new, uint64_t k);

    void sortWithRadix(std::vector<uint64_t> &suffix_array_second_part, uint64_t k);

public:
    SuffixArray(const std::string &s1_);

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
    buildClassesFirst();
}

void SuffixArray::sortWithRadix(std::vector<uint64_t> &suffix_array_second_part, uint64_t k) {
    for (uint64_t i = 0; i < s.size(); ++i) {
        if (suffix_array[i] < k) {
            suffix_array_second_part[i] = s.size() + suffix_array[i] - k;
        } else {
            suffix_array_second_part[i] = 0 + suffix_array[i] - k;
        }
    }
    std::vector<int64_t> count(s.size(), 0);
    for (uint64_t i = 0; i < s.size(); ++i) {
        ++count[class_of_symbol[suffix_array_second_part[i]]];
    }
    for (uint64_t i = 0; i < number_class; ++i) {
        count[i + 1] += count[i];
    }
    for (int64_t i = s.size() - 1; i >= 0; --i) {
        --count[class_of_symbol[suffix_array_second_part[i]]];
        suffix_array[count[class_of_symbol[suffix_array_second_part[i]]]] = suffix_array_second_part[i];
    }
}

void SuffixArray::buildSuffixArraySecondStep() {
    std::vector<uint64_t> suffix_array_for_second_part(s.size());
    std::vector<uint64_t> class_symbols_new(s.size());
    for (uint64_t k = 1;; k <<= 1) {
        sortWithRadix(suffix_array_for_second_part, k);

        buildClassesSecond(class_symbols_new, k);
        if (number_class + 1 == s.size()) {
            break;
        }
        class_of_symbol = class_symbols_new;
    }
}


std::vector<uint64_t> SuffixArray::buildLcp() {
    // do reverse suffix_array
    std::vector<uint64_t> reverse(suffix_array.size());
    for (uint64_t i = 0; i < suffix_array.size(); ++i) {
        reverse[suffix_array[i]] = i;
    }
    std::vector<uint64_t> lcp(s1.size(), 0);
    int64_t lcp_now = 0;
    for (uint64_t i = 0; i < s1.size(); ++i) {
        if (reverse[i] != 0) {
            int64_t previous_object_in_suffix_array = suffix_array[reverse[i] - 1];
            while (i + lcp_now < s1.size() && previous_object_in_suffix_array + lcp_now < s1.size()
                   && s1[i + lcp_now] == s1[previous_object_in_suffix_array + lcp_now]) {
                ++lcp_now;
            }
            lcp[reverse[i]] = lcp_now;
            lcp_now = std::max(int64_t(0), lcp_now - 1);
        } else {
            lcp_now = 0;
        }
    }
    return lcp;
}

SuffixArray::SuffixArray(const std::string &s1_) : s(s1_ + "$"), s1(s1_) {
    suffix_array.resize(s.size());
    class_of_symbol.resize(s.size());
    buildSuffixArrayFirstStep();
    //second step
    buildSuffixArraySecondStep();
    //second step
    suffix_array.erase(suffix_array.begin());
}

void SuffixArray::buildClassesSecond(std::vector<uint64_t> &class_symbols_new, const uint64_t k) {
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

void SuffixArray::buildClassesFirst() {
    class_of_symbol[suffix_array[0]] = 0;
    for (uint64_t i = 0; i + 1 < s.size(); ++i) {
        if (s[suffix_array[i + 1]] != s[suffix_array[i]]) {
            ++number_class;
        }
        class_of_symbol[suffix_array[i + 1]] = number_class;
    }
}

std::vector<uint64_t> SuffixArray::giveSuffix() {
    return suffix_array;
}

std::string algorithm(const std::string &a, const std::string &b, const uint64_t k) {
    std::string s = a + "%" + b;
    SuffixArray suffix_array(s);
    auto lcp = suffix_array.buildLcp();
    uint64_t sum = 0;
    uint64_t min_lcp = 0;
    uint64_t position_$ = a.size();
    std::string result;
    auto suffix = suffix_array.giveSuffix();
    for (uint64_t i = 0; i < s.size(); ++i) {
        min_lcp = std::min(min_lcp, lcp[i]);
        if (lcp[i] > 0 && ((suffix[i] < position_$ && suffix[i - 1] > position_$) ||
                           (suffix[i] > position_$ && suffix[i - 1] < position_$))) {
            sum += lcp[i];
            sum -= min_lcp;
            min_lcp = lcp[i];
            if (sum >= k) {
                for (int64_t j = 0; j < k - (sum - lcp[i]); ++j) {
                    result += s[suffix[i] + j];
                }
                break;
            }
        }

    }
    return result;
}

int main() {
    std::ios::sync_with_stdio(0);
    std::cin.tie(0);
    uint64_t k;
    std::string a, b;
    std::cin >> a >> b >> k;
    auto s = algorithm(a, b, k);
    if (s.empty()) {
        std::cout << -1;
    } else {
        std::cout << s;
    }
}