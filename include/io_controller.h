#ifndef IO_CONTROLLER_H
#define IO_CONTROLLER_H

#include <systemc>
#include "bus_if.h"

using namespace sc_core;
using namespace sc_dt;

// IOController — модуль контроллера ввода/вывода
class IOController : public sc_module {
public:
    // Порты интерфейса
    sc_in<bool> clk_i;          // такт
    sc_out<bool> io_ready_o;    // сигнал готовности данных
    sc_in<bool> read_done_i;    // сигнал от управляющего блока, что данные считаны

    // Интерфейс шины
    sc_port<bus_if> bus_port;

    // Процессы
    SC_HAS_PROCESS(IOController);

    IOController(sc_module_name name);

private:
    // Локальные буферы для конфигурации и данных
    std::vector<uint32_t> config_data;
    std::vector<uint32_t> image_data;

    // Методы
    void io_fsm();              // основная FSM
    void read_from_bus();       // чтение конфигурации и входных данных
    void send_result();         // передача результата обратно (пока заглушка)

    // Состояния
    enum State {
        IDLE,
        READ_CONFIG,
        READ_INPUT,
        WAIT_DONE,
        SEND_RESULT
    } state;

    unsigned int addr_ptr;      // текущий адрес чтения
};

#endif // IO_CONTROLLER_H
