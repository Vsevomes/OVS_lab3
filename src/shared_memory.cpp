#include "shared_memory.h"
#include <sstream>

using namespace sc_core;
using namespace std;

SharedMemory::SharedMemory(sc_module_name name, unsigned int size)
    : sc_module(name),
      clk_i("clk_i"),
      mem_size(size)
{
    mem_array.resize(mem_size, 0);
}

// ----------------------------------------------------------
// Запись в память
// ----------------------------------------------------------
void SharedMemory::write(addr_t addr, data_t data) {
    if (addr >= mem_size) {
        SC_REPORT_ERROR(this->name(), "Write out of memory bounds");
        return;
    }

    mem_mutex.lock(); // моделируем эксклюзивный доступ
    mem_array[addr] = data;
    mem_mutex.unlock();

    std::ostringstream msg;
    msg << "WRITE: addr=" << addr << " data=" << data;
    SC_REPORT_INFO(this->name(), msg.str().c_str());
}

// ----------------------------------------------------------
// Чтение из памяти
// ----------------------------------------------------------
data_t SharedMemory::read(addr_t addr) {
    if (addr >= mem_size) {
        SC_REPORT_ERROR(this->name(), "Read out of memory bounds");
        return 0;
    }

    mem_mutex.lock();
    data_t val = mem_array[addr];
    mem_mutex.unlock();

    std::ostringstream msg;
    msg << "READ: addr=" << addr << " data=" << val;
    SC_REPORT_INFO(this->name(), msg.str().c_str());

    return val;
}
