#include "tomasulo.h"
Tomasulo::Tomasulo() {
    for (int i = 0; i < REG_NUM; ++i) {
        regs[i].status = nullptr;
    }
}


void Tomasulo::run(std::vector<nel::inst_t>& insts) {
    int inst_idx = 0;
    CycleCounter::get_instance().reset();
    while (true) {
        //write back from functional units
        add_fus.write_back(this->regs);
        mult_fus.write_back(this->regs);
        load_fus.write_back(this->regs);

        if (inst_idx < insts.size()) {
            //try issue
            
            //TODO: record issue time
            
            //check reservation station status
            switch (insts[inst_idx].opr) {
                case nel::Add: {
                }
                case nel::Sub: {

                }
                case nel::Div: {

                }
                case nel::Mul: {

                }
                case nel::Load: {

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
    }
}