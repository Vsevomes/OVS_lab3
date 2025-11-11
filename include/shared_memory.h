#ifndef _SHARED_MEMORY_H
#define _SHARED_MEMORY_H

#include "systemc.h"
#include "config.h"

SC_MODULE(shared_memory)
{
    sc_port<sc_signal_in_if<size_t>, shared_mem_pe_count> addr_i;
    sc_port<sc_signal_inout_if<float>, shared_mem_pe_count> data_io;
    sc_port<sc_signal_in_if<bool>, 1> wr_i;
    sc_port<sc_signal_in_if<bool>, shared_mem_pe_count> rd_i;
    sc_in<bool> clk_i;

    SC_HAS_PROCESS(shared_memory);

    shared_memory(sc_module_name nm);
    ~shared_memory(){};
    size_t used_cells() const {
        size_t count = 0;
        for (size_t i = 0; i < shared_mem_size; ++i) {
            if (used[i]) count++;
        }
        return count;
    }

    void bus_write();
    void bus_read();

private:
    float mem[shared_mem_size];
    bool used[shared_mem_size] = {false};
    std::pair<bool, float> prepared_write_queue[shared_mem_pe_count];
};

#endif