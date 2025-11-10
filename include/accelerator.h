#ifndef ACCELERATOR_H
#define ACCELERATOR_H

#include <systemc>
#include <vector>
#include <memory>
#include "types.h"
#include "bus_if.h"
#include "matrix_bus.h"
#include "shared_memory.h"
#include "control_unit.h"
#include "io_controller.h"
#include "pe_mac.h"
#include "pe_activation.h"

using namespace sc_core;

class Accelerator : public sc_module {
public:
    sc_in<bool> clk_i;

    unsigned int num_pe; // количество PE

    SC_HAS_PROCESS(Accelerator);
    Accelerator(sc_module_name name, unsigned int pe_count = 4);

    // Подмодули
    MatrixBus bus;
    SharedMemory shared_mem;
    ControlUnit ctrl;
    IOController io_ctrl;
    PE_Activation pe_act;
    std::vector<PE_MAC*> pe_macs;

    // Сигналы управления PE
    std::vector<std::unique_ptr<sc_signal<bool>>> pe_start_signals;
    std::vector<std::unique_ptr<sc_signal<bool>>> pe_done_signals;
    sc_signal<bool> activation_start_sig;
    sc_signal<bool> activation_done_sig;

    void connect_modules();
    void setup_pe_signals();
};

#endif // ACCELERATOR_H
