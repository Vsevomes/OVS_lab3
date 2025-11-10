#include <systemc>
#include <fstream>
#include <sstream>
#include <vector>
#include <iostream>
#include "accelerator.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " <num_pe> <config_file>" << endl;
        return 1;
    }

    unsigned int num_pe = stoi(argv[1]);
    string config_file = argv[2];

    ifstream fin(config_file);
    if (!fin.is_open()) {
        cerr << "Error opening file: " << config_file << endl;
        return 1;
    }

    unsigned int I, L, N, O;
    fin >> I >> L >> N >> O;

    vector<float> weights(I*L + L*O);
    for (auto &w : weights)
        fin >> w;

    vector<float> data;
    float tmp;
    while (fin >> tmp)
        data.push_back(tmp);

    fin.close();
    cout << "Loaded configuration: I=" << I << " L=" << L << " N=" << N << " O=" << O << endl;

    Accelerator accel("accelerator", num_pe);

    sc_clock clk("clk", 10, SC_NS);
    accel.clk_i(clk);

    // -------------------------
    // VCD-трассировка
    // -------------------------
    sc_trace_file* tf = sc_create_vcd_trace_file("accelerator_trace");
    tf->set_time_unit(1, SC_NS);

    sc_trace(tf, accel.clk_i, "clk");
    sc_trace(tf, accel.ctrl.done_o, "ctrl_done");
    sc_trace(tf, accel.ctrl.activation_start_o, "activation_start");
    sc_trace(tf, accel.pe_act.done_o, "activation_done");

    // Все PE start/done сигналы
    for (unsigned int i = 0; i < num_pe; ++i) {
        stringstream ss_start, ss_done;
        ss_start << "PE" << i << "_start";
        ss_done << "PE" << i << "_done";

        // Разыменуем unique_ptr
        sc_trace(tf, *(accel.pe_start_signals[i]), ss_start.str().c_str());
        sc_trace(tf, *(accel.pe_done_signals[i]), ss_done.str().c_str());
    }

    // -------------------------
    // Загрузка данных через шину
    // -------------------------
    int addr = 0;
    accel.bus.write(addr++, I);
    accel.bus.write(addr++, L);
    accel.bus.write(addr++, N);
    accel.bus.write(addr++, O);

    for (float w : weights)
        accel.bus.write(addr++, w);

    for (float d : data)
        accel.bus.write(addr++, d);

    cout << "Configuration and input data loaded into bus." << endl;

    // -------------------------
    // Симуляция
    // -------------------------
    sc_start(1, SC_NS);
    while (!accel.ctrl.done_o.read()) {
        sc_start(10, SC_NS);
    }

    cout << "Computation finished!" << endl;

    // -------------------------
    // Вывод результата
    // -------------------------
    int result_addr = 0;
    cout << "Prediction:" << endl;
    for (unsigned int i = 0; i < O; ++i) {
        float val = accel.shared_mem.read(result_addr++);
        cout << "Output[" << i << "] = " << val << endl;
    }

    sc_close_vcd_trace_file(tf);
    cout << "VCD trace saved to accelerator_trace.vcd" << endl;

    return 0;
}
