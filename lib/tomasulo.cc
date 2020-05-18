#include "tomasulo.h"
#include <unistd.h>
Tomasulo::Tomasulo(): ars("Ars"), mrs("Mrs"), load_buffer("LB"), add_fus("Add"), mult_fus("Mult"), load_fus("Load") {
    reset();
}

void Tomasulo::regs_show() {
    printf("Regs:\n");
    for (int reg_start = 0; reg_start < 32; reg_start += 8) {
        printf("\t");
        for (int i = reg_start; i < reg_start + 8; ++i) {
            if (regs[i].status != nullptr) {
                printf("(R%d)%x/%s\t", i, regs[i].value, regs[i].status->get_name().c_str());
            } else {
                printf("(R%d)%x\t", i, regs[i].value);
            }
        }
        printf("\n");
    }
    //pc
    if (regs[PC].status != nullptr) {
        printf("\t(R32, PC): %x/%s\n", regs[PC].value, regs[PC].status->get_name().c_str());
    } else {
        printf("\t(R32, PC): %x\n", regs[PC].value);
    }
}

void Tomasulo::reset(int inst_num) {
    CycleCounter::get_instance().reset();
    Record::get_instance().reset(inst_num);
    for (int i = 0; i < REG_NUM; ++i) {
        regs[i].status = nullptr;
        regs[i].value = 0;
    }

    stall = false;

    ars.reset();
    mrs.reset();
    load_buffer.reset();
    add_fus.reset();
    mult_fus.reset();
    load_fus.reset();

    btb.reset();
    rob.reset(regs);
}

void Tomasulo::run_bp(std::vector<nel::inst_t>& insts, int print_cycle) {
    reset(insts.size());

    Record::get_instance().start_clock();

    while (true) {
        int cycle = ++CycleCounter::get_instance().counter;
        //if (cycle % 10000 == 0) std::cout << "cycle : " << cycle << std::endl;
        //write back from functional units
        add_fus.write_back_bp(regs, btb, rob);
        mult_fus.write_back_bp(regs, btb, rob);
        load_fus.write_back_bp(regs, btb, rob);

        if (!rob.commit(regs)) {
            //reset
            ars.reset();
            mrs.reset();
            load_buffer.reset();
            add_fus.reset();
            mult_fus.reset();
            load_fus.reset();
        }

        int inst_idx = regs[PC].value;

        if (inst_idx < insts.size() && !stall) {
            //try issue
            //check reservation station status
            switch (insts[inst_idx].opr) {
                case nel::Add:
                case nel::Sub: {
                    ReservationStation* q0 = regs[insts[inst_idx].regs[1]].status;
                    ReservationStation* q1 = regs[insts[inst_idx].regs[2]].status;
                    //int v0 = regs[insts[inst_idx].regs[1]].value;
                    //int v1 = regs[insts[inst_idx].regs[2]].value;
                    int v0 = rob.get_reg_value(insts[inst_idx].regs[1]);
                    int v1 = rob.get_reg_value(insts[inst_idx].regs[2]);
                    int write_reg = insts[inst_idx].regs[0];

                    int target_idx = ars.available();
                    ReservationStation* target = nullptr;
                    if (target_idx >= 0) {
                        rob_iter it = rob.push(insts[inst_idx].opr, write_reg);
                        target = ars.push_at(target_idx, insts[inst_idx].opr, inst_idx, v0, v1, q0, q1, it);
                        it->rs = target;
                        it->inst_idx = inst_idx;

                        //issue succ
                        Record::get_instance().update_issue(inst_idx, cycle);

                        //add it to the waiting queue of rs
                        if (q0 != nullptr) {
                            q0->waiting_rs.push_back(target);
                        }
                        if (q1 != nullptr) {
                            q1->waiting_rs.push_back(target);
                        }
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;
                    }
                    break;
                }
                case nel::Mul:
                case nel::Div: {
                    ReservationStation* q0 = regs[insts[inst_idx].regs[1]].status;
                    ReservationStation* q1 = regs[insts[inst_idx].regs[2]].status;
                    //int v0 = regs[insts[inst_idx].regs[1]].value;
                    //int v1 = regs[insts[inst_idx].regs[2]].value;
                    int v0 = rob.get_reg_value(insts[inst_idx].regs[1]);
                    int v1 = rob.get_reg_value(insts[inst_idx].regs[2]);
                    int write_reg = insts[inst_idx].regs[0];

                    int target_idx = mrs.available();
                    ReservationStation* target = nullptr;
                    if (target_idx >= 0) {
                        rob_iter it = rob.push(insts[inst_idx].opr, write_reg);
                        target = mrs.push_at(target_idx, insts[inst_idx].opr, inst_idx, v0, v1, q0, q1, it);
                        it->rs = target;
                        it->inst_idx = inst_idx;

                        //issue succ
                        Record::get_instance().update_issue(inst_idx, cycle);

                        //add it to the waiting queue of rs
                        if (q0 != nullptr) {
                            q0->waiting_rs.push_back(target);
                        }
                        if (q1 != nullptr) {
                            q1->waiting_rs.push_back(target);
                        }
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;
                    }
                    break;
                }
                case nel::Load: {
                    int addr = insts[inst_idx].ints[0];
                    int write_reg = insts[inst_idx].regs[0];
                    int target_idx = load_buffer.available();
                    ReservationStation* target = nullptr;
                    if (target_idx >= 0) {
                        //issue secc
                        rob_iter it = rob.push(insts[inst_idx].opr, write_reg);
                        target = load_buffer.push_at(target_idx, inst_idx, addr, write_reg, it);
                        it->rs = target;
                        it->inst_idx = inst_idx;

                        Record::get_instance().update_issue(inst_idx, cycle);
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;

                    }
                    break;
                }
                case nel::Jump: {
                    ReservationStation* q0 = regs[insts[inst_idx].regs[0]].status;
                    ReservationStation* q1 = nullptr;
                    //int v0 = regs[insts[inst_idx].regs[0]].value;
                    int v0 = rob.get_reg_value(insts[inst_idx].regs[0]);
                    int v1 = insts[inst_idx].ints[0];
                    int write_reg = PC;
                    int offset = insts[inst_idx].ints[1];

                    int target_idx = ars.available();
                    ReservationStation* target = nullptr;
                    if (target_idx >= 0) {
                        rob_iter it = rob.push(insts[inst_idx].opr, write_reg);
                        target = ars.push_at(target_idx, insts[inst_idx].opr, inst_idx, v0, v1, q0, q1, it, offset);
                        it->rs = target;
                        it->inst_idx = inst_idx;
                        //issue succ
                        Record::get_instance().update_issue(inst_idx, cycle);

                        //add it to the waiting queue of rs
                        if (q0 != nullptr) {
                            q0->waiting_rs.push_back(target);
                        }
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        //predict
                        int new_addr = btb.predict(inst_idx);
                        if (new_addr < 0) {
                            //not jump
                            regs[PC].value++;
                            it->predict_result = inst_idx + 1;
                        } else {
                            //jump
                            it->predict_result = new_addr;
                            regs[PC].value = new_addr;
                        }

                    }
                    break;
                }
                default: {
                    printf("unknown inst type: %d", insts[inst_idx].opr);
                    return;
                }
            }
        }

        //check functional units
        ReservationStation* target = nullptr;
        int result = 0;
        int left_cycle = 0;
        int target_inst_idx = 0;
        rob_iter it;
        while ((target = ars.select_task(target_inst_idx, result, left_cycle, it)) != nullptr) {
            if (add_fus.full()) break;
            bool ret = add_fus.push_bp(target, target_inst_idx, left_cycle, result, it);
            assert(ret);
        }

        while ((target = mrs.select_task(target_inst_idx, result, left_cycle, it)) != nullptr) {
            if (mult_fus.full()) break;
            bool ret = mult_fus.push_bp(target, target_inst_idx, left_cycle, result, it);
            assert(ret);
        }
        while ((target = load_buffer.select_task(target_inst_idx, result, left_cycle, it)) != nullptr) {
            if (load_fus.full()) break;
            bool ret = load_fus.push_bp(target, target_inst_idx, left_cycle, result, it);
            assert(ret);
        }


        add_fus.update();
        mult_fus.update();
        load_fus.update();

        //print 
#ifndef VERBOSE
        if (print_cycle == cycle) {
#endif
            printf("---------------log start---cycle: %d--------------\n", cycle);
            ars.show();
            mrs.show();
            load_buffer.show();
            regs_show();
            add_fus.show();
            mult_fus.show();
            load_fus.show();
            rob.show();
            btb.show();
            printf("---------------log done---------------------------\n");
#ifndef VERBOSE
        }
#endif

        //check fus
        bool done = add_fus.empty() && mult_fus.empty() && load_fus.empty();
        if (done) break;
    }

    Record::get_instance().end_clock();

    regs_show();

    //instruction status
    Record::get_instance().show_inst_status();
    Record::get_instance().show_time();
    Record::get_instance().show_predict();
    printf("Branch Prediction Total cycle: %d\n", CycleCounter::get_instance().counter);
}


void Tomasulo::run(std::vector<nel::inst_t>& insts, int print_cycle) {
    reset(insts.size());

    Record::get_instance().start_clock();
    while (true) {
        int cycle = ++CycleCounter::get_instance().counter;


        //write back from functional units
        add_fus.write_back(this->regs, stall);
        mult_fus.write_back(this->regs, stall);
        load_fus.write_back(this->regs, stall);

        int inst_idx = regs[PC].value;

        if (inst_idx < insts.size() && !stall) {
            //try issue
            //check reservation station status
            switch (insts[inst_idx].opr) {
                case nel::Add:
                case nel::Sub: {
                    ReservationStation* q0 = regs[insts[inst_idx].regs[1]].status;
                    ReservationStation* q1 = regs[insts[inst_idx].regs[2]].status;
                    int v0 = regs[insts[inst_idx].regs[1]].value;
                    int v1 = regs[insts[inst_idx].regs[2]].value;
                    int write_reg = insts[inst_idx].regs[0];

                    ReservationStation* target = ars.push(insts[inst_idx].opr, inst_idx, v0, v1, q0, q1);
                    if (target != nullptr) {
                        //issue succ
                        Record::get_instance().update_issue(inst_idx, cycle);

                        //add it to the waiting queue of rs
                        if (q0 != nullptr) {
                            q0->waiting_rs.push_back(target);
                        }
                        if (q1 != nullptr) {
                            q1->waiting_rs.push_back(target);
                        }
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;
                    }
                    break;
                }
                case nel::Mul:
                case nel::Div: {
                    ReservationStation* q0 = regs[insts[inst_idx].regs[1]].status;
                    ReservationStation* q1 = regs[insts[inst_idx].regs[2]].status;
                    int v0 = regs[insts[inst_idx].regs[1]].value;
                    int v1 = regs[insts[inst_idx].regs[2]].value;
                    int write_reg = insts[inst_idx].regs[0];

                    ReservationStation* target = mrs.push(insts[inst_idx].opr, inst_idx, v0, v1, q0, q1);
                    if (target != nullptr) {
                        //issue succ
                        Record::get_instance().update_issue(inst_idx, cycle);

                        //add it to the waiting queue of rs
                        if (q0 != nullptr) {
                            q0->waiting_rs.push_back(target);
                        }
                        if (q1 != nullptr) {
                            q1->waiting_rs.push_back(target);
                        }
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;
                    }
                    break;
                }
                case nel::Load: {
                    int addr = insts[inst_idx].ints[0];
                    int write_reg = insts[inst_idx].regs[0];
                    ReservationStation* target = load_buffer.push(inst_idx, addr, write_reg);
                    if (target != nullptr) {
                        //issue secc
                        Record::get_instance().update_issue(inst_idx, cycle);
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;
                    }
                    break;
                }
                case nel::Jump: {
                    ReservationStation* q0 = regs[insts[inst_idx].regs[0]].status;
                    ReservationStation* q1 = nullptr;
                    int v0 = regs[insts[inst_idx].regs[0]].value;
                    int v1 = insts[inst_idx].ints[0];
                    int write_reg = PC;
                    int offset = insts[inst_idx].ints[1];

                    ReservationStation* target = ars.push(insts[inst_idx].opr, inst_idx, v0, v1, q0, q1, offset);
                    stall = true;
                    if (target != nullptr) {
                        //issue succ
                        Record::get_instance().update_issue(inst_idx, cycle);

                        //add it to the waiting queue of rs
                        if (q0 != nullptr) {
                            q0->waiting_rs.push_back(target);
                        }
                        target->waiting_reg = write_reg;
                        regs[write_reg].status = target;

                        regs[PC].value++;
                    }
                    break;
                }
                default: {
                    printf("unknown inst type: %d", insts[inst_idx].opr);
                    return;
                }
            }
        }

        //check functional units
        ReservationStation* target = nullptr;
        int result = 0;
        int left_cycle = 0;
        int target_inst_idx = 0;
        rob_iter it;
        while ((target = ars.select_task(target_inst_idx, result, left_cycle, it)) != nullptr) {
            if (add_fus.full()) break;
            bool ret = add_fus.push(target, target_inst_idx, left_cycle, result);
            assert(ret);
        }
        while ((target = mrs.select_task(target_inst_idx, result, left_cycle, it)) != nullptr) {
            if (mult_fus.full()) break;
            bool ret = mult_fus.push(target, target_inst_idx, left_cycle, result);
            assert(ret);
        }
        while ((target = load_buffer.select_task(target_inst_idx, result, left_cycle, it)) != nullptr) {
            if (load_fus.full()) break;
            bool ret = load_fus.push(target, target_inst_idx, left_cycle, result);
            assert(ret);
        }

        add_fus.update();
        mult_fus.update();
        load_fus.update();

        //print 
#ifndef VERBOSE
        if (print_cycle == cycle) {
#endif
            printf("---------------log start---cycle: %d--------------\n", cycle);
            ars.show();
            mrs.show();
            load_buffer.show();
            regs_show();
            add_fus.show();
            mult_fus.show();
            load_fus.show();
            printf("---------------log done---------------------------\n");
#ifndef VERBOSE
        }
#endif
        //check fus
        bool done = add_fus.empty() && mult_fus.empty() && load_fus.empty();
        if (done) break;
    }

    Record::get_instance().end_clock();

    regs_show();

    //instruction status
    Record::get_instance().show_inst_status();
    Record::get_instance().show_time();
    printf("Non Branch Prediction Total cycle: %d\n", CycleCounter::get_instance().counter);
}