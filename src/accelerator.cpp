#include "accelerator.h"
#include <sstream>

using namespace sc_core;
using namespace std;

Accelerator::Accelerator(sc_module_name name, unsigned int pe_count)
    : sc_module(name),
      num_pe(pe_count),
      bus("bus"),
      shared_mem("shared_mem", 1024),
      ctrl("control_unit", pe_count),
      io_ctrl("io_controller"),
      pe_act("activation_unit")
{
    // Инициализация сигналов PE
    pe_start_signals.reserve(num_pe);
    pe_done_signals.reserve(num_pe);

    for (unsigned int i = 0; i < num_pe; ++i) {
        std::stringstream ss_start, ss_done;
        ss_start << "pe_start_" << i;
        ss_done << "pe_done_" << i;

        pe_start_signals.push_back(std::make_unique<sc_signal<bool>>(ss_start.str().c_str()));
        pe_done_signals.push_back(std::make_unique<sc_signal<bool>>(ss_done.str().c_str()));
    }

    setup_pe_signals();
    connect_modules();
}

// Привязка сигналов PE к ControlUnit
void Accelerator::setup_pe_signals() {
    for (unsigned int i = 0; i < num_pe; ++i) {
        ctrl.pe_start_signals[i](*pe_start_signals[i]);
        ctrl.pe_done_signals[i](*pe_done_signals[i]);
    }
}

// Подключение модулей
void Accelerator::connect_modules() {
    ctrl.clk_i(clk_i);
    io_ctrl.clk_i(clk_i);
    shared_mem.clk_i(clk_i);
    pe_act.clk_i(clk_i);

    ctrl.bus_port(bus);
    io_ctrl.bus_port(bus);
    pe_act.bus_port(bus);
    for (auto& pe : pe_macs)
        pe->bus_port(bus);

    // Связь шины с памятью
    bus.set_read_cb(std::bind(&SharedMemory::read, &shared_mem, std::placeholders::_1));
    bus.set_write_cb(std::bind(&SharedMemory::write, &shared_mem, std::placeholders::_1, std::placeholders::_2));

    // CU ↔ IOController
    ctrl.done_o.write(io_ctrl.read_done_i);
    ctrl.io_ready_i(io_ctrl.io_ready_o);

    // CU ↔ PE Activation
    ctrl.activation_start_o(activation_start_sig);
    ctrl.activation_done_i(activation_done_sig);
    pe_act.start_i(activation_start_sig);
    pe_act.done_o(activation_done_sig);

    // CU ↔ PE MAC
    for (unsigned int i = 0; i < num_pe; ++i) {
        ctrl.pe_start_signals[i](*pe_start_signals[i]);
        ctrl.pe_done_signals[i](*pe_done_signals[i]);
    }
}
