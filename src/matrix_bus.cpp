#include "matrix_bus.h"
#include <iostream>
using namespace sc_core;

MatrixBus::MatrixBus(sc_module_name name)
    : sc_module(name),
      clk_i("clk_i"),
      addr_bo("addr_bo"),
      data_bo("data_bo"),
      data_bi("data_bi"),
      wr_o("wr_o"),
      rd_o("rd_o"),
      transfer_latency(sc_time(10, SC_NS)),
      hold_time(SC_ZERO_TIME)
{
    wr_o.write(false);
    rd_o.write(false);
    data_bi.write(0);

    SC_THREAD(bus_monitor);
    sensitive << clk_i.pos();
    dont_initialize();
}

// Монитор шины (можно расширить для отладки)
void MatrixBus::bus_monitor() {
    while (true) {
        wait();
        // Здесь можно печатать состояние линий
    }
}

// Интерфейс bus_if
void MatrixBus::write(addr_t addr, data_t value) {
    master_write(addr, value);
}

data_t MatrixBus::read(addr_t addr) {
    return master_read(addr);
}

// Мастер методы
void MatrixBus::master_write(addr_t addr, data_t value) {
    addr_bo.write(addr);
    data_bo.write(value);
    wr_o.write(true);

    wait(clk_i->posedge_event());
    wait(transfer_latency);

    if (write_cb)
        write_cb(addr, value);
    else
        SC_REPORT_INFO(this->name(), "write_cb not set");

    if (hold_time > SC_ZERO_TIME)
        wait(hold_time);

    wr_o.write(false);
}

data_t MatrixBus::master_read(addr_t addr) {
    addr_bo.write(addr);
    rd_o.write(true);

    wait(clk_i->posedge_event());
    wait(transfer_latency);

    data_t value = read_cb ? read_cb(addr) : 0;
    data_bi.write(value);

    if (hold_time > SC_ZERO_TIME)
        wait(hold_time);

    rd_o.write(false);
    return value;
}
