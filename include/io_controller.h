#ifndef _IO_CONTROLLER_H_
#define _IO_CONTROLLER_H_

#include "systemc.h"

SC_MODULE(io_controller)
{
  sc_in<bool> clk_i;
  sc_out<size_t> addr_o;
  sc_inout<float> data_io;
  sc_out<bool> wr_o;
  sc_out<bool> rd_o;
  sc_in<bool> ioc_wr_i;
  sc_in<bool> ioc_rd_i;
  sc_in<size_t> ioc_res_addr_i;
  sc_out<bool> ioc_busy_o;

  size_t comm_time = 0;

  SC_HAS_PROCESS(io_controller);

  io_controller(sc_module_name nm);
  ~io_controller() {};

  void mem_write();
  void mem_read();

};

#endif