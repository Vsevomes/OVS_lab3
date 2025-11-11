#ifndef _PE_MAC_H_
#define _PE_MAC_H_

#include "systemc.h"
#include <queue>

SC_MODULE(pe_mac)
{
    sc_in<bool> clk_i;
    sc_in<size_t> data_cnt_i;
    sc_in<size_t> w_start_addr_i;
    sc_in<size_t> v_start_addr_i;
    sc_in<bool> load_i;
    sc_out<bool> busy_o;

    sc_out<size_t> addr_o;
    sc_inout<float> data_io;
    sc_out<bool> wr_o;
    sc_out<bool> rd_o;

    sc_event process_event;
    sc_event write_result_event;

    std::queue<float> weight_queue;
    std::queue<float> value_queue;

    size_t cycles;
    size_t comm_time = 0;

    SC_HAS_PROCESS(pe_mac);
    pe_mac();
    pe_mac(sc_module_name nm);
    ~pe_mac(){};

    void process();
    void write_result();
    void load_data();

private:
    enum
    {
        LOAD_DATA = 0,
        PROCESS = 1,
        RESULT = 2,
    } state;
    float accumulator;
};

#endif