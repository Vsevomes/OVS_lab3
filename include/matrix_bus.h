#ifndef MATRIX_BUS_H
#define MATRIX_BUS_H

#include <systemc>
#include <functional>
#include "types.h"
#include "bus_if.h"

class MatrixBus : public sc_core::sc_module, public bus_if {
public:
    sc_core::sc_in<bool> clk_i; // Вход тактового сигнала

    // Линии шины (внутренние сигналы)
    sc_core::sc_signal<addr_t> addr_bo;
    sc_core::sc_signal<data_t> data_bo;
    sc_core::sc_signal<data_t> data_bi;
    sc_core::sc_signal<bool> wr_o;
    sc_core::sc_signal<bool> rd_o;

    // Колбеки на память
    void set_write_cb(std::function<void(addr_t, data_t)> cb) { write_cb = cb; }
    void set_read_cb(std::function<data_t(addr_t)> cb) { read_cb = cb; }

    // Методы интерфейса bus_if
    void write(addr_t addr, data_t value) override;
    data_t read(addr_t addr) override;

    // Методы мастера
    void master_write(addr_t addr, data_t value);
    data_t master_read(addr_t addr);

    sc_core::sc_time transfer_latency;
    sc_core::sc_time hold_time;

    SC_HAS_PROCESS(MatrixBus);
    MatrixBus(sc_core::sc_module_name name);

private:
    void bus_monitor();
    std::function<void(addr_t, data_t)> write_cb;
    std::function<data_t(addr_t)> read_cb;
};

#endif // MATRIX_BUS_H
