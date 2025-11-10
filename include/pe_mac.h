#ifndef PE_MAC_H
#define PE_MAC_H

#include <systemc>
#include <vector>
#include "bus_if.h"
#include "types.h"

using namespace sc_core;

// Processing Element (MAC)
class PE_MAC : public sc_module {
public:
    // Порты
    sc_in<bool> clk_i;              // такт
    sc_in<bool> start_i;            // сигнал запуска
    sc_out<bool> done_o;            // сигнал окончания вычислений

    sc_port<bus_if> bus_port;       // доступ к общей памяти

    // Параметры
    unsigned int num_inputs;        // количество входов для MAC

    SC_HAS_PROCESS(PE_MAC);

    PE_MAC(sc_module_name name, unsigned int inputs);

    // Методы
    void load_weights(const std::vector<data_t>& w); // загрузка весов в локальную память

private:
    std::vector<data_t> local_weights; // локальная память весов
    void mac_fsm();                     // основной процесс вычислений
};

#endif // PE_MAC_H
