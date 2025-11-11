#include "control_unit.h"

control_unit::control_unit(sc_module_name nm)
    : sc_module(nm),
      neuron_addr_link(pe_count),
      local_layer(0),
      local_neuron(0),
      device_load(0),
      device_busy(0)
{
    for (size_t i = 0; i < pe_count; ++i)
        device_busy.push_back(false);

    SC_THREAD(load_data);
    sensitive << clk_i.pos();

    SC_THREAD(process);
    sensitive << clk_i.pos();

    SC_THREAD(result);
    sensitive << clk_i.pos();

    SC_THREAD(read_mem_mapped);
    sensitive << clk_i.pos();

    SC_THREAD(free_device);
    sensitive << clk_i.pos();
}

void control_unit::start()
{
    state = LOAD_DATA;
    for(size_t i = 0; i < pe_count; ++i)
        load_o[i]->write(0);
}

inline void control_unit::write_mem(float data, size_t addr)
{
    addr_o->write(addr);
    data_io->write(data);
    wr_o->write(1);
    wait();
    comm_time++;
    wr_o->write(0);
}

inline float control_unit::read_mem(size_t addr)
{
    addr_o->write(addr);
    rd_o->write(1);
    wait();
    comm_time++;
    rd_o->write(0);
    wait();
    comm_time++;
    wait(SC_ZERO_TIME);
    float value = data_io->read();
    return value;
}

void control_unit::load_data()
{
    while (1)
    {
        while (state != LOAD_DATA)
            wait();

        std::cout << "[control_unit] Начало загрузки данных конфигурации..." << std::endl;

        // Запрос к памяти конфигурации
        ioc_wr_o->write(1);
        wait();
        ioc_wr_o->write(0);

        while (!ioc_busy_i.read())
            wait();
            comm_time++;
        while (ioc_busy_i.read())
            wait();
            comm_time++;

        // --- Чтение конфигурации ---
        input_size = read_mem(0);           // I
        size_t hidden_layers = read_mem(1); // N
        size_t hidden_neurons = read_mem(2); // H
        output_size = read_mem(3);          // O

        std::cout << "[control_unit] Конфигурация прочитана: input=" << input_size
                  << ", output=" << output_size << std::endl;

        // --- Настройка внутреннего вектора слоёв ---
        inner_size.clear();
        inner_size.reserve(hidden_layers + 2);
        inner_size.resize(hidden_layers + 2, 0);

        inner_size[0] = input_size;
        for (size_t i = 0; i < hidden_layers; ++i)
            inner_size[i + 1] = hidden_neurons;
        inner_size[hidden_layers + 1] = output_size;

        // --- Расчёт адресов весов и значений ---
        weight_local_addr = 4; // веса идут после 4-х чисел конфигурации
        value_local_addr = weight_local_addr;

        for (size_t layer = 0; layer < inner_size.size() - 1; ++layer)
        {
            value_local_addr += inner_size[layer] * inner_size[layer + 1];
        }

        state = PROCESS;
        std::cout << "[control_unit] Переход в состояние PROCESS" << std::endl;
    }
}

void control_unit::process_mem_mapped()
{
    if (!mem_mapped.empty())
    {
        size_t device_to_save = mem_mapped.front().first;
        float value_to_save = mem_mapped.front().second;
        mem_mapped.pop();
        act_data_io->write(value_to_save);
        act_start_o->write(1);
        wait();
        comm_time++;
        act_start_o->write(0);
        wait();
        comm_time++;
        wait(SC_ZERO_TIME);
        value_to_save = act_data_io->read();
        write_mem(value_to_save, neuron_addr_link[device_to_save]);
    }
}

void control_unit::process()
{
    while (1)
    {
        while (state != PROCESS)
        {
            wait();
        }

        std::cout << "[control_unit] Запуск вычислений..." << std::endl;

        while (local_layer != inner_size.size() - 1)
        {
            size_t device_to_start = pe_count;
                for (size_t i = 0; i < pe_count; ++i)
                    if (!device_busy[i])
                        device_to_start = i;
            if (device_to_start != pe_count)
            {

                std::cout << "[control_unit] Назначен PE #" << device_to_start
                          << " для слоя " << local_layer << std::endl;

                data_cnt_o[device_to_start]->write(inner_size[local_layer]);
                w_start_addr_o[device_to_start]->write(weight_local_addr);
                v_start_addr_o[device_to_start]->write(value_local_addr);
                load_o[device_to_start]->write(1);

                neuron_addr_link[device_to_start] = value_local_addr + inner_size[local_layer] + local_neuron;

                weight_local_addr += inner_size[local_layer];

                if (++local_neuron == inner_size[local_layer + 1])
                {
                    value_local_addr += inner_size[local_layer];
                    local_neuron = 0;
                    ++local_layer;
                }

                wait();
                comm_time++;
                ++device_load;
                device_busy[device_to_start] = true;
                wait();
                comm_time++;
                load_o[device_to_start]->write(0);
                
            }

            wait();

            process_mem_mapped();

            wait();
        }

        while (device_load)
        {
            process_mem_mapped();
            wait();
        }

        std::cout << "[control_unit] Все слои обработаны, переход к результату." << std::endl;

        state = RESULT;
    }
}

void control_unit::result()
{
    while (1)
    {
        ioc_rd_o->write(0);
        while (state != RESULT)
            wait();
        
        std::cout << "[control_unit] Отправка результата на IO..." << std::endl;

        ioc_res_addr_o->write(value_local_addr);
        ioc_rd_o->write(1);
        wait();
        while (ioc_busy_i)
            wait();
    }
}

void control_unit::read_mem_mapped()
{
    while (1)
    {
        for (size_t i = 0; i < pe_count; ++i)
            if (core_wr_i[i]->read()) {
                wait();
                mem_mapped.push({i, core_data_io[i]->read()});
            }
        wait();
    }
}

void control_unit::free_device()
{
    while (1)
    {
        // printf("free_device\n");
        for (size_t i = 0; i < pe_count; ++i)
            if (!core_busy_i[i]->read() && device_busy[i])
            {
                device_busy[i] = false;
                --device_load;

                std::cout << "[control_unit] PE #" << i << " освободился." << std::endl;
            }
        wait();
    }
}
