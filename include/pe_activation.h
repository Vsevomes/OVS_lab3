#ifndef _PE_ACTIVATION_H_
#define _PE_ACTIVATION_H_

#include "systemc.h"

SC_MODULE(pe_activation)
{
    sc_in<bool> clk_i;
    sc_inout<float> act_data_io;
    sc_in<bool> act_start_i;

    size_t comm_time = 0;

    SC_HAS_PROCESS(pe_activation);

    pe_activation(sc_module_name nm);
    ~pe_activation() {};

    void process();
};

#endif