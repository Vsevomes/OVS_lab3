#include "accelerator.h"

accelerator::accelerator(sc_module_name nm)
    : sc_module(nm),
    clk_i("accelerator_clk"),
    peAct("act"),
    controlUnit("cu"),
    ioController("ioc"),
    sharedMem("lm"),
    macArray("nc", pe_count),
    addr(shared_mem_pe_count),
    data(shared_mem_pe_count),
    wr(shared_mem_pe_count),
    rd(shared_mem_pe_count),
    data_cnt(pe_count),
    w_start_addr(pe_count),
    v_start_addr(pe_count),
    load(pe_count),
    busy(pe_count)
{
    // printf("accelerator constructor\n");
    for (size_t i = 0; i < pe_count; ++i) {
        macArray[i].clk_i(clk_i);
    }

    peAct.clk_i(clk_i);
    controlUnit.clk_i(clk_i);
    ioController.clk_i(clk_i);
    sharedMem.clk_i(clk_i);

    ioController.addr_o(addr[0]);
    controlUnit.addr_o(addr[0]);
    sharedMem.addr_i(addr[0]);
    ioController.data_io(data[0]);
    controlUnit.data_io(data[0]);
    sharedMem.data_io(data[0]);
    ioController.wr_o(wr[0]);
    controlUnit.wr_o(wr[0]);
    sharedMem.wr_i(wr[0]);
    ioController.rd_o(rd[0]);
    controlUnit.rd_o(rd[0]);
    sharedMem.rd_i(rd[0]);

    controlUnit.ioc_res_addr_o(ioc_res_addr);
    ioController.ioc_res_addr_i(ioc_res_addr);
    controlUnit.ioc_busy_i(ioc_busy);
    ioController.ioc_busy_o(ioc_busy);
    controlUnit.ioc_wr_o(ioc_wr);
    ioController.ioc_wr_i(ioc_wr);
    controlUnit.ioc_rd_o(ioc_rd);
    ioController.ioc_rd_i(ioc_rd);

    peAct.act_data_io(act_data);
    controlUnit.act_data_io(act_data);
    peAct.act_start_i(act_start);
    controlUnit.act_start_o(act_start);

    for (size_t i = 0; i < pe_count; ++i) {
        macArray[i].data_cnt_i(data_cnt[i]);
        controlUnit.data_cnt_o(data_cnt[i]);
        macArray[i].w_start_addr_i(w_start_addr[i]);
        controlUnit.w_start_addr_o(w_start_addr[i]);
        macArray[i].v_start_addr_i(v_start_addr[i]);
        controlUnit.v_start_addr_o(v_start_addr[i]);
        macArray[i].load_i(load[i]);
        controlUnit.load_o(load[i]);
        macArray[i].busy_o(busy[i]);
        controlUnit.core_busy_i(busy[i]);
        macArray[i].addr_o(addr[i + 1]);
        sharedMem.addr_i(addr[i + 1]);
        macArray[i].data_io(data[i + 1]);
        sharedMem.data_io(data[i + 1]);
        controlUnit.core_data_io(data[i + 1]);
        macArray[i].wr_o(wr[i + 1]);
        controlUnit.core_wr_i(wr[i + 1]);
        macArray[i].rd_o(rd[i + 1]);
        sharedMem.rd_i(rd[i + 1]);
    }
    
    SC_METHOD(start);
}

void accelerator::start()
{
    // printf("Starting\n");
    controlUnit.start();
}

void accelerator::report_stats()
{
    size_t total_comm_time = 0;
    size_t total_compute_time = 0;

    // Считаем PE
    for (size_t i = 0; i < macArray.size(); ++i)
    {
        total_comm_time += macArray[i].comm_time;
        total_compute_time += macArray[i].cycles;

        std::cout << "[PE " << i << "] "
                  << "Compute: " << macArray[i].cycles
                  << ", Comm: " << macArray[i].comm_time << std::endl;
    }

    total_comm_time += controlUnit.comm_time;
    total_comm_time += ioController.comm_time;
    total_comm_time += peAct.comm_time;

    std::cout << "=== Accelerator Stats ===" << std::endl;
    std::cout << "Communication cycles: " << total_comm_time << std::endl;
    std::cout << "Computation cycles:   " << total_compute_time << std::endl;
    std::cout << "Total simulated time:  " << sc_time_stamp() << std::endl;
    std::cout << "Used memory: " << sharedMem.used_cells() * sizeof(float) << " bytes" << std::endl;
}


