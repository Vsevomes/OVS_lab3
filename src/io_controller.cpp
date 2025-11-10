#include "io_controller.h"
#include <sstream>

using namespace sc_core;
using namespace std;

IOController::IOController(sc_module_name name)
    : sc_module(name),
      clk_i("clk_i"),
      io_ready_o("io_ready_o"),
      read_done_i("read_done_i"),
      bus_port("bus_port"),
      state(IDLE),
      addr_ptr(0)
{
    SC_THREAD(io_fsm);
    sensitive << clk_i.pos();
    dont_initialize();
}

// ---------------------------------------------------------
// Основной автомат состояний контроллера I/O
// ---------------------------------------------------------
void IOController::io_fsm() {
    while (true) {
        wait(); // ждём фронт такта

        switch (state) {
        case IDLE:
            SC_REPORT_INFO(this->name(), "State: IDLE");
            addr_ptr = 0;
            state = READ_CONFIG;
            break;

        case READ_CONFIG:
            SC_REPORT_INFO(this->name(), "Reading configuration...");
            read_from_bus();
            state = READ_INPUT;
            break;

        case READ_INPUT:
            SC_REPORT_INFO(this->name(), "Reading input data...");
            read_from_bus();
            io_ready_o.write(true);  // сигнал блоку управления, что данные готовы
            state = WAIT_DONE;
            break;

        case WAIT_DONE:
            if (read_done_i.read()) {
                io_ready_o.write(false);
                SC_REPORT_INFO(this->name(), "Computation done, sending result...");
                state = SEND_RESULT;
            }
            break;

        case SEND_RESULT:
            send_result();
            state = IDLE;
            break;
        }
    }
}

// ---------------------------------------------------------
// Имитация чтения конфигурации и данных из шины
// ---------------------------------------------------------
void IOController::read_from_bus() {
    // Для примера считаем, что 8 слов конфигурации и 16 слов данных
    unsigned int words = (state == READ_CONFIG) ? 8 : 16;

    for (unsigned int i = 0; i < words; ++i) {
        uint32_t data = bus_port->read(addr_ptr++);
        if (state == READ_CONFIG)
            config_data.push_back(data);
        else
            image_data.push_back(data);

        std::ostringstream msg;
        msg << "Read word[" << i << "] = " << data;
        SC_REPORT_INFO(this->name(), msg.str().c_str());
    }
}

// ---------------------------------------------------------
// Заглушка для передачи результата обратно по шине
// ---------------------------------------------------------
void IOController::send_result() {
    SC_REPORT_INFO(this->name(), "Sending results back via bus...");
    // Для примера: просто записываем 4 результата
    for (int i = 0; i < 4; ++i) {
        bus_port->write(addr_ptr++, 100 + i); // фиктивные данные
    }
}
