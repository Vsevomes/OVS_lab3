#include "pe_activation.h"
#include <sstream>

using namespace sc_core;
using namespace std;

PE_Activation::PE_Activation(sc_module_name name)
    : sc_module(name),
      clk_i("clk_i"),
      start_i("start_i"),
      done_o("done_o"),
      num_inputs(0)
{
    SC_THREAD(activation_fsm);
    sensitive << clk_i.pos();
    dont_initialize();
}

// ----------------------------------------------------------
// Сигмоидальная функция
// ----------------------------------------------------------
data_t PE_Activation::sigmoid(data_t x) {
    return 1.0 / (1.0 + exp(-x));
}

// ----------------------------------------------------------
// Основной FSM
// ----------------------------------------------------------
void PE_Activation::activation_fsm() {
    while (true) {
        wait(); // фронт такта

        if (start_i.read()) {
            SC_REPORT_INFO(this->name(), "Starting activation computation...");

            // Для примера считаем, что входные данные лежат в адресах 0..num_inputs-1
            data_t sum = 0.0f;
            for (unsigned int i = 0; i < num_inputs; ++i) {
                data_t val = bus_port->read(i);
                sum += val;
            }

            // применяем сигмоид
            data_t activated = sigmoid(sum);

            std::ostringstream msg;
            msg << "Activated result = " << activated;
            SC_REPORT_INFO(this->name(), msg.str().c_str());

            // записываем результат в память (например, адрес num_inputs)
            bus_port->write(num_inputs, activated);

            done_o.write(true);
            wait(1, SC_NS);
            done_o.write(false);
        }
    }
}
