#ifndef PE_ACTIVATION_H
#define PE_ACTIVATION_H

#include <systemc>
#include <vector>
#include "bus_if.h"
#include "types.h"
#include <cmath>

using namespace sc_core;

// Processing Element для активации нейронов
class PE_Activation : public sc_module {
public:
    // Порты
    sc_in<bool> clk_i;
    sc_in<bool> start_i;
    sc_out<bool> done_o;

    sc_port<bus_if> bus_port;       // доступ к общей памяти

    SC_HAS_PROCESS(PE_Activation);

    PE_Activation(sc_module_name name);

private:
    std::vector<data_t> inputs;     // входные значения слоя
    unsigned int num_inputs;         // количество входов для активации
    void activation_fsm();           // основной процесс
    data_t sigmoid(data_t x);        // сигмоидальная функция
};

#endif // PE_ACTIVATION_H
