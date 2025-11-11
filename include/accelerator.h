#ifndef _ACCELERATOR_H
#define _ACCELERATOR_H

#include "pe_activation.h"
#include "control_unit.h"
#include "io_controller.h"
#include "shared_memory.h"
#include "pe_mac.h"
#include "config.h"

SC_MODULE(accelerator)
{
    sc_in<bool> clk_i;

    pe_activation peAct;
    control_unit controlUnit;
    io_controller ioController;
    shared_memory sharedMem;
    sc_vector<pe_mac> macArray;

    SC_HAS_PROCESS(accelerator);

    accelerator(sc_module_name nm);
    ~accelerator(){};
    void report_stats();

    void start();

    sc_signal<float> act_data;
    sc_signal<bool> act_start;

    sc_signal<bool> ioc_wr;
    sc_signal<bool> ioc_rd;
    sc_signal<size_t> ioc_res_addr;
    sc_signal<bool> ioc_busy;

    std::vector<sc_signal<size_t>> addr;
    std::vector<sc_signal<float>> data;
    std::vector<sc_signal<bool>> wr;
    std::vector<sc_signal<bool>> rd;

    std::vector<sc_signal<size_t>> data_cnt;
    std::vector<sc_signal<size_t>> w_start_addr;
    std::vector<sc_signal<size_t>> v_start_addr;
    std::vector<sc_signal<bool>> load;
    std::vector<sc_signal<bool>> busy;
};

#endif