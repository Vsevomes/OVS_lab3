#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

#include <systemc>
#include <vector>
#include "bus_if.h"
#include "types.h"

using namespace sc_core;
using namespace sc_dt;

class ControlUnit : public sc_module {
public:
    sc_in<bool> clk_i;
    sc_in<bool> io_ready_i;
    sc_out<bool> done_o;

    sc_port<bus_if> bus_port;

    // Сигналы для PE и Activation
    std::vector<sc_out<bool>> pe_start_signals; // исправлено: обычный порт, не указатель
    std::vector<sc_in<bool>>  pe_done_signals;
    sc_out<bool> activation_start_o;
    sc_in<bool> activation_done_i;

    SC_HAS_PROCESS(ControlUnit);

    ControlUnit(sc_module_name name, unsigned int num_pe);

private:
    enum State { IDLE, COMPUTE, ACTIVATE, DONE } state;

    // Конфигурация сети
    struct NetConfig { unsigned int I, L, N, O; } config;
    unsigned int current_layer;
    unsigned int next_neuron_idx;
    std::vector<bool> pe_busy;
    std::vector<unsigned int> neuron_mapping;

    void control_fsm();

    unsigned int neurons_in_layer() { return config.N; }
    bool all_pe_free();
    void load_configuration();
};

#endif // CONTROL_UNIT_H
