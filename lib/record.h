#ifndef __RECORD_H
#define __RECORD_H

#include <fstream>
#include <iostream>
#include <vector>
#include <sys/time.h>
#include "nel.h"

class Record {
private:
    static Record instance;
    Record() {}
    ~Record() {}
    Record(const Record&) = delete;
    Record& operator=(const Record&) = delete;
public:
    std::vector<nel::inst_status_t> inst_status;
    double start, end;
    int predict;
    int predict_succ, predict_fail;
    static Record& get_instance() {
        return instance;
    }

    void reset(int size) {
        start = end = 0.0;
        predict = predict_succ = predict_fail = 0;
        inst_status.clear();
        inst_status.resize(size);
    }

    void show_predict() {
        printf("Prediction accuracy: %lf(succ: %d, fail: %d)\n", (double)predict_succ / (predict_succ + predict_fail) , predict_succ, predict_fail);
    }

    void start_clock() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        start = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
    }

    void end_clock() {
        struct timeval tv;
        gettimeofday(&tv, NULL);
        end = tv.tv_sec * 1000.0 + tv.tv_usec / 1000.0;
    }

    void update_issue(int idx, int cycle) {
        if (!inst_status[idx].done) {
            inst_status[idx].issue = cycle;
        }
    }
    void update_exec(int idx, int cycle) {
        if (!inst_status[idx].done) {
            inst_status[idx].exec_comp = cycle;
        }
    }
    void update_write(int idx, int cycle) {
        if (!inst_status[idx].done) {
            inst_status[idx].write_result = cycle;
            inst_status[idx].done = true;
        }
    }

    void show_inst_status() {
        printf("Instruction status:\n");
        printf("\t\t%8s\t%8s\t%s\n", "Issue", "ExecComp", "WriteResult");
        for (int i = 0; i < inst_status.size(); ++i) {
            printf("\t%d:\t%8d\t%8d\t%8d\n", i, inst_status[i].issue, inst_status[i].exec_comp, inst_status[i].write_result);
        }
    }

    void show_time() {
        printf("Simulation Time: %lf ms\n", end - start);
    }

    void output(const std::string& log) {
        std::ofstream out(log);
        if (!out.is_open()) {
            std::cout << log << " cannot open" << std::endl;
            return;
        }

        for (int i = 0; i < inst_status.size(); ++i) {
            assert(inst_status[i].done);
            out << inst_status[i].issue << " " << inst_status[i].exec_comp << " " << inst_status[i].write_result << std::endl;
        }

        out.close();
    }
};


#endif