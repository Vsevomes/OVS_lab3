#include "accelerator.h"

int sc_main(int, char*[]) {
  sc_core::sc_clock clk("clk", sc_time(100, SC_NS));
  accelerator accelerator("accelerator");
  accelerator.clk_i(clk);
  sc_start(10, sc_core::SC_SEC);
  accelerator.report_stats();
  return 0;
}
