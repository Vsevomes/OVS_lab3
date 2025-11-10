#include "control_unit.h"
#include <sstream>

using namespace sc_core;
using namespace std;

ControlUnit::ControlUnit(sc_module_name name, unsigned int num_pe)
    : sc_module(name),
      clk_i("clk_i"),
      io_ready_i("io_ready_i"),
      done_o("done_o"),
      state(IDLE),
      current_layer(0),
      next_neuron_idx(0)
{
    pe_busy.resize(num_pe, false);
    neuron_mapping.resize(num_pe, 0);

    SC_THREAD(control_fsm);
    sensitive << clk_i.pos();
    dont_initialize();
}

// ----------------------------------------------------------
// Проверка, свободны ли все PE
// ----------------------------------------------------------
bool ControlUnit::all_pe_free() {
    for (bool busy : pe_busy)
        if (busy) return false;
    return true;
}

// ----------------------------------------------------------
// Загрузка конфигурации из памяти через шину
// ----------------------------------------------------------
void ControlUnit::load_configuration() {
    int i_val = static_cast<int>(bus_port->read(0));
    int l_val = static_cast<int>(bus_port->read(1));
    int n_val = static_cast<int>(bus_port->read(2));
    int o_val = static_cast<int>(bus_port->read(3));

    config.I = i_val;
    config.L = l_val;
    config.N = n_val;
    config.O = o_val;

    std::ostringstream msg;
    msg << "Config loaded: I=" << config.I
        << ", L=" << config.L
        << ", N=" << config.N
        << ", O=" << config.O;
    SC_REPORT_INFO(this->name(), msg.str().c_str());
}

// ----------------------------------------------------------
// Основная FSM
// ----------------------------------------------------------
void ControlUnit::control_fsm() {
    while (true) {
        wait(); // фронт такта

        switch (state) {
        case IDLE:
            if (io_ready_i.read()) {
                current_layer = 0;
                next_neuron_idx = 0;
                pe_busy.assign(pe_busy.size(), false);
                load_configuration();
                state = COMPUTE;
            }
            break;

        case COMPUTE:
            // Запуск свободных PE
            for (unsigned int i = 0; i < pe_start_signals.size(); ++i) {
                if (!pe_busy[i] && next_neuron_idx < neurons_in_layer()) {
                    pe_start_signals[i]->write(true);
                    pe_busy[i] = true;
                    neuron_mapping[i] = next_neuron_idx;
                    next_neuron_idx++;
                }
            }

            // Проверка завершения PE
            for (unsigned int i = 0; i < pe_done_signals.size(); ++i) {
                if (pe_busy[i] && pe_done_signals[i]->read()) {
                    pe_busy[i] = false;
                    pe_start_signals[i]->write(false);
                }
            }

            // Если все нейроны слоя рассчитаны, запускаем активацию
            if (next_neuron_idx >= neurons_in_layer() && all_pe_free()) {
                activation_start_o.write(true);
                state = ACTIVATE;
            }
            break;

        case ACTIVATE:
            if (activation_done_i.read()) {
                activation_start_o.write(false);
                current_layer++;
                if (current_layer < config.L) {
                    next_neuron_idx = 0;
                    pe_busy.assign(pe_busy.size(), false);
                    state = COMPUTE;
                } else {
                    done_o.write(true);
                    state = DONE;
                }
            }
            break;

        case DONE:
            done_o.write(false);
            state = IDLE;
            break;
        }
    }
}
