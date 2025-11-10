#include "pe_mac.h"
#include <sstream>

using namespace sc_core;
using namespace std;

PE_MAC::PE_MAC(sc_module_name name, unsigned int inputs)
    : sc_module(name),
      clk_i("clk_i"),
      start_i("start_i"),
      done_o("done_o"),
      num_inputs(inputs)
{
    SC_THREAD(mac_fsm);
    sensitive << clk_i.pos();
    dont_initialize();
}

void PE_MAC::load_weights(const std::vector<data_t>& w) {
    if (w.size() != num_inputs) {
        SC_REPORT_ERROR(this->name(), "Weight vector size mismatch");
        return;
    }
    local_weights = w;
}

// ----------------------------------------------------------
// FSM MAC
// ----------------------------------------------------------
void PE_MAC::mac_fsm() {
    while (true) {
        wait(); // ждём фронт такта

        if (start_i.read()) {
            SC_REPORT_INFO(this->name(), "Starting MAC computation...");

            data_t acc = 0.0f;

            // Для примера считаем, что входные данные лежат в адресах 0..num_inputs-1
            for (unsigned int i = 0; i < num_inputs; ++i) {
                data_t input = bus_port->read(i);      // чтение из общей памяти
                acc += input * local_weights[i];       // MAC
            }

            std::ostringstream msg;
            msg << "MAC result = " << acc;
            SC_REPORT_INFO(this->name(), msg.str().c_str());

            done_o.write(true); // сигнал окончания
            wait(1, SC_NS);     // короткая задержка
            done_o.write(false);
        }
    }
}
