#ifndef TYPES_H
#define TYPES_H

#include <systemc>
#include <vector>

// Структура конфигурации нейросети
struct NetConfig {
    int I;  // количество входов
    int L;  // количество слоев
    int N;  // количество нейронов в каждом слое
    int O;  // количество выходов
};

// Структура весов (для одного нейрона или слоя)
struct Weight {
    std::vector<float> w;
};

// Входные данные (все изображения в одном векторе)
struct InputData {
    std::vector<float> d;
};

// Типы для адреса и данных в шине
typedef unsigned int addr_t;
typedef float data_t;

#endif // TYPES_H
