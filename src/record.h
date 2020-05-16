#ifndef __RECORD_H
#define __RECORD_H

#include "nel.h"
#include <vector>

class Record {
private:
    static Record instance;
    Record() {}
    ~Record() {}
    Record(const Record&) = delete;
    Record& operator=(const Record&) = delete;
public:
    std::vector<nel::inst_status_t> inst_status;
    static Record& get_instance() {
        return instance;
    }

    void reset(int size) {
        inst_status.clear();
        inst_status.resize(size);
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
        printf("\t\tIssue\tExecComp\tWriteResult\n");
        for (int i = 0; i < inst_status.size(); ++i) {
            printf("\t%d:\t%d\t%d\t\t%d\n", i, inst_status[i].issue, inst_status[i].exec_comp, inst_status[i].write_result);
        }
    }
};


#endif