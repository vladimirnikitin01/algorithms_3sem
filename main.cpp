#include <cmath>
#include <cstdio>
#include <iostream>
#include <vector>
#include <complex>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string>

const uint64_t base_256 = 256;
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

uint64_t from256to10(std::vector<uint64_t> &one_block, std::vector<uint64_t> &degree) {
    // оптимизация, сразу создать один массив такой

    uint64_t sum = 0;
    for (uint64_t i = 0; i < one_block.size(); ++i) {
        sum += degree[i] * one_block[i];
    }
    return sum;
}

std::vector<uint64_t> from10to256(uint64_t number, uint64_t base = 256) {
    std::vector<uint64_t> block;
    while (number != 0) {
        block.push_back(number % base);
        number /= base;
    }
    return block;
}

class CompressingMusic {
private:
    std::vector<uint64_t> new_data;
    std::vector<uint64_t> degree;
    std::vector<std::complex<long double>> used_array;
    WAVHEADER header;
    char *path;
public:
    CompressingMusic(const WAVHEADER &header_,
                     const unsigned char *data_input,
                     char *path_);

    void preprocessing_before_FFT();

    static void FFT(std::vector<std::complex<long double>> &a, bool is_invert = false);

    void compress();

    void algo();

    unsigned char *fill_memory_new_data();

    void write_file(const unsigned char *data_memory);

};

CompressingMusic::CompressingMusic(const WAVHEADER &header_,
                                   const unsigned char *data_input,
                                   char *path_) : header(header_) {
    path = path_;
    std::vector<uint64_t> all_block(header_.subchunk2Size / header_.blockAlign);
    uint64_t blockAlign = header_.blockAlign;
    std::vector<uint64_t> one_block(blockAlign);

    //проинициализируем массив степеней, чтобы каждый раз не создавать новый
    degree.resize(blockAlign);
    degree[0] = 1;
    for (uint64_t i = 1; i < one_block.size(); ++i) {
        degree[i] = base_256 * degree[i - 1];
    }

    for (uint64_t i = 0; i < header_.subchunk2Size; i += blockAlign) {
        for (uint64_t j = 0; j < blockAlign; ++j) {
            one_block[j] = data_input[i + j];
        }
        all_block[i / blockAlign] = from256to10(one_block, degree);
    }
    new_data.resize(all_block.size());
    used_array.resize(all_block.size());
    for (uint64_t i = 0; i < all_block.size(); ++i) {
        used_array[i] = all_block[i];
    }
}

void CompressingMusic::preprocessing_before_FFT() {
    // сначала делаем длину кратной степени 2
    uint64_t n = 1;
    while (n < used_array.size()) {
        n <<= 1;
    }
    used_array.resize(n);
}

void CompressingMusic::FFT(std::vector<std::complex<long double>> &a, bool is_invert) {
    auto n = a.size();
    if (n == 1) return;

    std::vector<std::complex<long double>> a_0(n / 2), a_1(n / 2);
    for (uint64_t i = 0, j = 0; i < n; i += 2, ++j) {
        a_0[j] = a[i];
        a_1[j] = a[i + 1];
    }
    FFT(a_0, is_invert);
    FFT(a_1, is_invert);

    long double angle = 2 * PI / n;
    if (is_invert) {
        angle *= -1;
    }

    std::complex<long double> w(1), w_n(cos(angle), sin(angle));
    for (uint64_t i = 0; i < n / 2; ++i) {
        a[i] = a_0[i] + w * a_1[i];
        a[i + n / 2] = a_0[i] - w * a_1[i];
        if (is_invert) {
            a[i] /= 2;
            a[i + n / 2] /= 2;
        }
        w *= w_n;
    }
}

void CompressingMusic::compress() {
    double long percent;
    std::cout << "Введите на сколько процентов сжать\n";
    std::cin >> percent;
    uint64_t first_element_change = used_array.size() * (1 - percent / 100);
    for (uint64_t i = first_element_change; i < used_array.size(); ++i) {
        used_array[i] = 0;
    }
}

void CompressingMusic::algo() {
    preprocessing_before_FFT();
    FFT(used_array);
    compress();
    FFT(used_array, true);
    for (uint64_t i = 0; i < new_data.size(); ++i) {
        new_data[i] = round(used_array[i].real());
    }
    auto memory_to_record = fill_memory_new_data();
    write_file(memory_to_record);
}

unsigned char *CompressingMusic::fill_memory_new_data() {
    // запись в data_memory
    uint64_t blockAlign = header.blockAlign;

    auto data_memory = new unsigned char[header.subchunk2Size];
    for (uint64_t i = 0; i < header.subchunk2Size; i += blockAlign) {
        auto block_now = from10to256(new_data[i / blockAlign]);
        if (blockAlign < block_now.size()) {
            for (uint64_t j = 0; j < block_now.size(); ++j) {
                data_memory[i + j] = 255;
                //cout << "AAaaaa ";
            }
        } else {
            uint64_t start_index = i;
            for (uint64_t j = 0; j < block_now.size(); ++j) {
                data_memory[i + j] = block_now[j];
                start_index = i + j + 1;
            }
            for (uint64_t j = 0; j < blockAlign - block_now.size(); ++j) {
                data_memory[start_index] = 0;
                ++start_index;
                //cout<<start_index<<" ";

            }
        }
    }
    return data_memory;
}

void CompressingMusic::write_file(const unsigned char *data_memory) {
    // вывод data2 and hearder
    char path_[4096];
    std::cout << "Введите полный путь для нового файла\n";
    std::cin >> path_;
    auto file_record = open(path_, O_CREAT | O_RDWR | O_TRUNC, 0640);
    //auto file_record = open("/home/vova/Documents/MIPT/algo/3A/speech_new.wav", O_CREAT | O_RDWR | O_TRUNC, 0640);
    //write(file_record, &header, sizeof(WAVHEADER));
    uint64_t count = 0;
    while (true) {
        int count_now = write(file_record, &(header) + count, sizeof(WAVHEADER) - count);
        count += count_now;
        if (count_now <= 0) {
            break;
        }
    }
    count = 0;
    while (true) {
        int count_now = write(file_record, data_memory + count, header.subchunk2Size - count);
        count += count_now;
        if (count_now <= 0) {
            break;
        }
    }
    close(file_record);

    delete[] data_memory;
}

int main() {
    char path[4096];
    std::cout << "Введите полный путь, но лучше используйте файл speech.wav от лектора\n";
    std::cin >> path;
    //FILE *file = fopen("/home/vova/Documents/MIPT/algo/3A/speech.wav", "rd");
    FILE *file = fopen(path, "rd");
    if (!file) {
        std::cout << "Failed open file";
        return 0;
    }

    WAVHEADER header;
    fread(&header, sizeof(WAVHEADER), 1, file);
    auto *data = new unsigned char[header.subchunk2Size];
    fread(data, header.subchunk2Size, 1, file);
    fclose(file);
    CompressingMusic compress_WAV(header, data, path);
    delete[] data;
    compress_WAV.algo();
    return 0;

}