#include "io_controller.h"

#include <iostream>
#include <fstream>
#include <string>

io_controller::io_controller(sc_module_name nm)
    : sc_module(nm)
{
    SC_THREAD(mem_write);
    sensitive << clk_i.pos();

    SC_THREAD(mem_read);
    sensitive << clk_i.pos();
}

void io_controller::mem_write()
{
    while (1)
    {
        while (!ioc_wr_i.read())
            wait();
            comm_time++;

         std::cout << "[io_controller] Начало записи модели в память..." << std::endl;

        ioc_busy_o->write(1);

        std::ifstream ifs;
        ifs.open("input/model");

        size_t addr = 0;
        while (ifs.is_open() && !ifs.eof())
        {
            float cell;
            ifs >> cell;
            addr_o->write(addr++);
            data_io->write(cell);
            wr_o->write(1);
            wait();
            comm_time++;
            wr_o->write(0);
        }
        wait();
        ioc_busy_o->write(0);

        std::cout << "[io_controller] Модель успешно загружена." << std::endl;
    }
}

void io_controller::mem_read()
{
    while (1)
    {
        while (!ioc_rd_i.read())
            wait();
            comm_time++;
        ioc_busy_o.write(1);
        size_t addr = ioc_res_addr_i->read();
        printf("====RESULT====\n");
        addr_o->write(addr);
        rd_o->write(1);
        wait();
        comm_time++;
        rd_o->write(0);
        wait();
        wait(SC_ZERO_TIME);
        printf("Class circle -> %f\n", data_io->read());
        addr_o->write(addr + 1);
        rd_o->write(1);
        wait();
        rd_o->write(0);
        wait();
        wait(SC_ZERO_TIME);
        printf("Class square -> %f\n", data_io->read());
        addr_o->write(addr + 2);
        rd_o->write(1);
        wait();
        rd_o->write(0);
        wait();
        wait(SC_ZERO_TIME);
        printf("Class triangle -> %f\n", data_io->read());
        ioc_busy_o.write(0);
        sc_stop();
    }
}
