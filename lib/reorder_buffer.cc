#include "reorder_buffer.h"

ROB::ROB() {
    buf.clear();
    for (int i = 0; i < REG_NUM; ++i) vregs[i] = 0;
}

void ROB::reset(reg_t regs[]) {
    buf.clear();
    for (int i = 0; i < REG_NUM; ++i) {
        vregs[i] = regs[i].value;
    }
}

void ROB::show() {
    printf("ReorderBuffer size(%lu):\n", buf.size());
    printf("\tOp  \tWReg\tReady\tResult\tPResult\tRS  \tInst\n");
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
        printf("\t%4s", it->rs->get_name().c_str());
        printf("\t%d\n", it->inst_idx);
    }

    printf("Reorder Virtual Regs:\n");
    for (int i = 0; i < REG_NUM; i += 8) {
        for (int j = i; j < i + 8 && j < REG_NUM; ++j) {
            printf("\t(R%2d)%4d", j, vregs[j]);
        }
        printf("\n");
    }
}

rob_iter ROB::push(nel::op_t op, int write_reg) {
    ReorderBuffer rob;
    rob.op = op;
    rob.write_reg = write_reg;
    rob.ready = false;
    rob.result = -1;
    rob.rs = nullptr; //set by outside
    rob.inst_idx = -1; //set by outside
    buf.push_back(rob);
    rob_iter it = buf.end();
    it--;
    return it;
}

int ROB::get_reg_value(int reg_idx) {
    assert(reg_idx != PC);
    return vregs[reg_idx];
}

void ROB::update_vregs(int reg_idx, int value) {
    assert(reg_idx >= 0 && reg_idx < REG_NUM);
    assert(reg_idx != PC);
    //std::cout << "update vregs for " << reg_idx << ", value: " << value << std::endl;
    vregs[reg_idx] = value;
}

bool ROB::commit(reg_t regs[], bool& predicting, bool& stall) {
    while (buf.size() > 0) {
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
                    //std::cout << "commit for " << nel::op_name[op] << ", (wr, result) = " << write_reg << ", " << result << std::endl;

                    //actually it should has been set nullptr by "write_back_bp"
                    //regs[write_reg].value = vregs[write_reg];
                    regs[write_reg].value = result;
                    /*
                    if (regs[write_reg].status == rs) {
                        regs[write_reg].status = nullptr;
                    }
                    */
                    break;
                }
                case nel::Jump: {
                    predicting = false;
                    stall = false;
                    if (result != predict_result) {
                        //prediction fail
                        Record::get_instance().predict_fail++;
                        //std::cout << "rob jump prediction fail, new pc: " << result << std::endl;

                        //rewrite PC
                        assert(write_reg == PC);
                        int new_pc = result;

                        //clear rob
                        this->reset(regs);

                        //clear regs
                        for (int i = 0; i < REG_NUM; ++i) {
                            regs[i].status = nullptr;
                        }
                        regs[PC].value = new_pc;

                        return false;
                    } else {
                        //std::cout << "rob jump prediction succ" << std::endl;
                        //prediction succ
                        Record::get_instance().predict_succ++;
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
        } else break;

    }

    return true;
}