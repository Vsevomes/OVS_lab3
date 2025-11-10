#ifndef BUS_IF_H
#define BUS_IF_H

#include <systemc>
#include "types.h"

// Интерфейс матричной шины
struct bus_if : public sc_core::sc_interface {
    virtual void write(addr_t addr, data_t data) = 0;
    virtual data_t read(addr_t addr) = 0;
};

// Порты интерфейса (сигнальные линии)
struct bus_signals {
    sc_core::sc_in<bool> clk_i;      // Сигнал синхронизации
    sc_core::sc_out<addr_t> addr_bo; // Линия адреса
    sc_core::sc_out<data_t> data_bo; // Линия передачи данных (от ведущего)
    sc_core::sc_in<data_t> data_bi;  // Линия приема данных (к ведущему)
    sc_core::sc_out<bool> wr_o;      // Сигнал записи
    sc_core::sc_out<bool> rd_o;      // Сигнал чтения

    bus_signals(const char* name)
        : clk_i((std::string(name) + "_clk_i").c_str()),
          addr_bo((std::string(name) + "_addr_bo").c_str()),
          data_bo((std::string(name) + "_data_bo").c_str()),
          data_bi((std::string(name) + "_data_bi").c_str()),
          wr_o((std::string(name) + "_wr_o").c_str()),
          rd_o((std::string(name) + "_rd_o").c_str())
    {}
};

#endif // BUS_IF_H
