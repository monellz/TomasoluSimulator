#include "tomasulo.h"
Tomasulo::Tomasulo(): ars("Ars"), mrs("Mrs"), load_buffer("LB"), add_fus("Add"), mult_fus("Mult"), load_fus("Load") {
    std::cout << "tomasulo init" << std::endl;
    for (int i = 0; i < REG_NUM; ++i) {
        regs[i].status = nullptr;
        regs[i].value = 0;
    }
}

void Tomasulo::regs_show() {
    printf("Regs:\n");
    for (int reg_start = 0; reg_start < 32; reg_start += 8) {
        printf("\t");
        for (int i = reg_start; i < reg_start + 8; ++i) {
            printf("\tR%d", i);
        }
        printf("\n\tstatus");
        for (int i = reg_start; i < reg_start + 8; ++i) {
            if (regs[i].status != nullptr) {
                printf("\t%x/%s", regs[i].value, regs[i].status->get_name().c_str());
            } else {
                printf("\t%x", regs[i].value);
            }
        }
        printf("\n");
    }
}


void Tomasulo::run(std::vector<nel::inst_t>& insts) {
    std::cout << "in run" << std::endl;
    int inst_idx = 0;
    CycleCounter::get_instance().reset();
    Record::get_instance().reset(insts.size());
    while (true) {
        int cycle = ++CycleCounter::get_instance().counter;

        //write back from functional units
        add_fus.write_back(this->regs);
        mult_fus.write_back(this->regs);
        load_fus.write_back(this->regs);




        std::cout << "write back done" << std::endl;

        if (inst_idx < insts.size()) {
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
                        target->waiting_regs.push_back(write_reg);
                        regs[write_reg].status = target;

                        inst_idx++;
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
                        target->waiting_regs.push_back(write_reg);
                        regs[write_reg].status = target;

                        inst_idx++;
                    }
                    break;
                }
                case nel::Load: {
                    int addr = insts[inst_idx].ints[0];
                    int write_reg = insts[inst_idx].regs[0];
                    printf("load for reg %d\n", write_reg);
                    ReservationStation* target = load_buffer.push(inst_idx, addr, write_reg);
                    if (target != nullptr) {
                        //issue secc
                        Record::get_instance().update_issue(inst_idx, cycle);
                        target->waiting_regs.push_back(write_reg);
                        regs[write_reg].status = target;

                        inst_idx++;
                    }
                    break;
                }
                case nel::Jump: {
                    printf("not implement for jump\n");
                    return;
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
        int inst_idx = 0;
        target = ars.select_task(inst_idx, result, left_cycle);
        if (target != nullptr) {
            add_fus.push(target, inst_idx, left_cycle, result);
        }

        target = mrs.select_task(inst_idx, result, left_cycle);
        if (target != nullptr) {
            mult_fus.push(target, inst_idx, left_cycle, result);
        }

        target = load_buffer.select_task(inst_idx, result, left_cycle);
        if (target != nullptr) {
            load_fus.push(target, inst_idx, left_cycle, result);
        }


        add_fus.update();
        mult_fus.update();
        load_fus.update();
        
        //print 
        printf("---------------log start---cycle: %d--------------\n", cycle);
        ars.show();
        mrs.show();
        load_buffer.show();
        regs_show();
        add_fus.show();
        mult_fus.show();
        load_fus.show();
        printf("---------------log done---------------------------\n");


        //check fus
        bool done = add_fus.empty() && mult_fus.empty() && load_fus.empty();
        if (done) break;
    }


    //instruction status
    Record::get_instance().show_inst_status();
}