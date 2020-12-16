#include <cmath>
#include <cstdio>
#include <iostream>
#include <utility>
#include <vector>
#include <complex>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;
const int64_t base_256 = 256;
const long double PI = 3.1415926535897932384626;

// Структура, описывающая заголовок WAV файла.
struct WAVHEADER {
    char chunkId[4];
    unsigned int chunkSize;
    char format[4];
    char subchunk1Id[4];
    unsigned int subchunk1Size;
    unsigned short audioFormat;
    unsigned short numChannels;
    unsigned int sampleRate;
    unsigned int byteRate;
    unsigned short blockAlign;
    unsigned short bitsPerSample;
    char subchunk2Id[4];
    unsigned int subchunk2Size;
    // Далее следуют непосредственно Wav данные.
};

int64_t from256to10(vector<int64_t> &one_block) {
    // оптимизация, сразу создать один массив такой
    vector<int64_t> degree(one_block.size());
    degree[0] = 1;
    for (int64_t i = 1; i < one_block.size(); ++i) {
        degree[i] = base_256 * degree[i - 1];
    }

    int64_t sum = 0;
    for (uint64_t i = 0; i < one_block.size(); ++i) {
        sum += degree[i] * one_block[i];
    }
    return sum;
}

vector<int64_t> from10to256(int64_t number, int64_t base = 256) {
    vector<int64_t> block;
    while (number != 0) {
        block.push_back(number % base);
        number /= base;
    }
    return block;
}

class ChangeData {
private:
    typedef complex<long double> base;
    vector<int64_t> data;
    vector<base> after_FFT;
    WAVHEADER header;
public:
    ChangeData(const WAVHEADER &header_, const unsigned char *data_input) : header(header_) {
        vector<int64_t> all_block(header_.subchunk2Size / header_.blockAlign);

        int64_t blockAlign = header_.blockAlign;
        vector<int64_t> one_block(blockAlign);
        for (uint64_t i = 0; i < header_.subchunk2Size; i += blockAlign) {
            for (uint64_t j = 0; j < blockAlign; ++j) {
                one_block[j] = data_input[i + j];
            }
            all_block[i / blockAlign] = from256to10(one_block);
        }
        data = all_block;
        after_FFT.reserve(all_block.size());
        //прописать в цикле это самому
        for (uint64_t i = 0; i < all_block.size(); ++i) {
            after_FFT[i] = all_block[i];
        }
    };

    vector<int64_t> give_new_data();

    void preprocessing_before_FFT();

    void FFT(vector<base> &a, bool invert);

    void compress();

    void algo();

};

void ChangeData::preprocessing_before_FFT() {

    // сначала делаем длину кратной степени 2
    uint64_t n = 1;
    while (n < after_FFT.size()) {
        n <<= 1;
    }
    after_FFT.resize(n);
}

void ChangeData::FFT(vector<base> &a, bool invert) {
    auto n = (int64_t) a.size();
    if (n == 1) return;

    vector<base> a0(n / 2), a1(n / 2);
    for (int64_t i = 0, j = 0; i < n; i += 2, ++j) {
        a0[j] = a[i];
        a1[j] = a[i + 1];
    }
    FFT(a0, invert);
    FFT(a1, invert);

    long double ang = 2 * PI / n * (invert ? -1 : 1);
    base w(1), wn(cos(ang), sin(ang));
    for (int64_t i = 0; i < n / 2; ++i) {
        a[i] = a0[i] + w * a1[i];
        a[i + n / 2] = a0[i] - w * a1[i];
        if (invert) {
            a[i] /= 2, a[i + n / 2] /= 2;
        }
        w *= wn;
    }
}

void ChangeData::compress() {
    double long percent;
    cin >> percent;
    int64_t first_element_change = after_FFT.size() * (1 - percent / 100);
    for (uint64_t i = first_element_change; i < after_FFT.size(); ++i) {
        after_FFT[i] = 0;
    }
}

void ChangeData::algo() {
    preprocessing_before_FFT();
    FFT(after_FFT, false);
    compress();
    FFT(after_FFT, true);
    for (uint64_t i = 0; i < data.size(); ++i) {
        data[i] = round(after_FFT[i].real());
    }
}

vector<int64_t> ChangeData::give_new_data() {
    return data;
}

int main() {
    FILE *file = fopen(R"(C:\Users\Vova\Documents\Clion_project(Documents)\3 contest\speech.wav)", "r");
    if (!file) {
        std::cout << "Failed open file";
        return 0;
    }

    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, file);
    auto *data = new unsigned char[header.subchunk2Size];
    fread(data, header.subchunk2Size, 1, file);
    fclose(file);
    //вот тут мы получили data.
    // надо теперь ее обработать и переобразовать в вектор
    ChangeData compress_WAV(header, data);
    delete[] data;
    compress_WAV.algo();

    // запись в data2
    int64_t blockAlign = header.blockAlign;

    auto data2 = new unsigned char[header.subchunk2Size];
    auto new_data = compress_WAV.give_new_data();
    for (uint64_t i = 0; i < header.subchunk2Size; i += blockAlign) {
        auto block_now = from10to256(new_data[i / blockAlign]);
        if (blockAlign < block_now.size()) {
            for (uint64_t j = 0; j < block_now.size(); ++j) {
                data2[i + j] = 255;
                cout << "AAaaaa ";
            }
        } else {
            uint64_t start_index = i;
            for (uint64_t j = 0; j < block_now.size(); ++j) {
                data2[i + j] = block_now[j];
                start_index = i + j + 1;
            }
            for (int64_t j = 0; j < blockAlign - block_now.size(); ++j) {
                data2[start_index] = 0;
                ++start_index;
                //cout<<start_index<<" ";

            }
        }
    }


    // вывод data2 and hearder

    auto file_record = open("/home/vova/Documents/MIPT/algo/test/speech_new.wav", O_CREAT | O_RDWR | O_TRUNC, 0640);
    //write(file_record, &header, sizeof(WAVHEADER));
    int64_t count = 0;
    while (true) {
        int count_now = write(file_record, &(header) + count, sizeof(WAVHEADER) - count);
        count += count_now;
        if (count_now <= 0) {
            break;
        }
    }
    count = 0;
    while (true) {
        int count_now = write(file_record, data2 + count, header.subchunk2Size - count);
        count += count_now;
        if (count_now <= 0) {
            break;
        }
    }
    close(file_record);

    delete[] data2;
    return 0;

}