#include "pe_activation.h"

pe_activation::pe_activation(sc_module_name nm)
    : sc_module(nm),
      clk_i("pe_activation_clk"),
      act_data_io("pe_activation_data"),
      act_start_i("pe_activation_start")
{
    SC_THREAD(process);
    sensitive << clk_i.pos();
}

void pe_activation::process()
{
    while (1)
    {
        if (act_start_i.read())
        {
            float value = act_data_io->read();

            std::cout << "[pe_activation] Применение функции активации" << std::endl;

            wait();
            comm_time++;
            act_data_io->write(1.0 / (exp(-value) + 1));
        }
        wait();
    }
}
