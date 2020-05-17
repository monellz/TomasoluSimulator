#include "reorder_buffer.h"

ROB::ROB() {
    reset();
}

void ROB::reset() {
    buf.clear();
}

void ROB::show() {
    printf("ReorderBuffer size(%lu):\n", buf.size());
    printf("\tOp  \tWReg\tReady\tResult\tPResult\tRS\n");
    for (auto it = buf.begin(); it != buf.end(); ++it) {
        printf("\t%4s", nel::op_name[it->op]);
        printf("\tR%-3d", it->write_reg);
        if (it->ready) printf("\tYes ");
        else printf("\tNo  ");
        printf("\t%-4d", it->result);
        if (it->op == nel::Jump) {
            printf("\t%-4d", it->predict_result);
        } else {
            printf("\t?   ");
        }
        printf("\t%s\n", it->rs->get_name().c_str());
    }
}

rob_iter ROB::push(nel::op_t op, int write_reg) {
    ReorderBuffer rob;
    rob.op = op;
    rob.write_reg = write_reg;
    rob.ready = false;
    rob.result = -1;
    rob.rs = nullptr; //set by outside
    buf.push_back(rob);
    rob_iter it = buf.end();
    it--;
    return it;
}

bool ROB::commit(reg_t regs[]) {
    if (buf.size() == 0) return true;

    rob_iter it = buf.begin();

    bool ready = it->ready;
    nel::op_t op = it->op;
    int result = it->result;
    int write_reg = it->write_reg;
    ReservationStation* rs = it->rs;
    int predict_result = it->predict_result;
    assert(rs != nullptr);


    if (ready) {
        buf.pop_front();
        switch (op) {
            case nel::Add:
            case nel::Sub:
            case nel::Mul:
            case nel::Div:
            case nel::Load: {
                //write back
                regs[write_reg].value = result;
                if (regs[write_reg].status == rs) {
                    regs[write_reg].status = nullptr;
                }

                break;
            }
            case nel::Jump: {
                if (result != predict_result) {
                    //prediction fail

                    //rewrite PC
                    assert(write_reg == PC);
                    int new_pc = result;

                    //clear rob
                    buf.clear();

                    //clear regs
                    for (int i = 0; i < REG_NUM; ++i) {
                        regs[i].status = nullptr;
                    }
                    regs[PC].value = new_pc;

                    return false;
                } else {
                    //prediction succ
                    if (regs[write_reg].status == rs) {
                        regs[write_reg].status = nullptr;
                    }
                }
                break;
            }
            default: {
                printf("unknown op: %d\n", op);
                return false;
            }
        }
    }
    return true;
}