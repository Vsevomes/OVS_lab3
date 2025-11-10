#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <systemc>
#include "bus_if.h"
#include <vector>

using namespace sc_core;
using namespace sc_dt;

// SharedMemory — модель общей памяти, доступной всем узлам
class SharedMemory : public sc_module, public bus_if {
public:
    sc_in<bool> clk_i;  // Такт
    sc_mutex mem_mutex; // Для моделирования конфликтов доступа (если нужно)

    SC_HAS_PROCESS(SharedMemory);

    SharedMemory(sc_module_name name, unsigned int size);

    // Методы интерфейса bus_if
    void write(addr_t addr, data_t data);
    data_t read(addr_t addr);

private:
    std::vector<data_t> mem_array; // Массив данных
    unsigned int mem_size;
};

#endif // SHARED_MEMORY_H
