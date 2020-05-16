#ifndef __CYCLE_COUNTER_H
#define __CYCLE_COUNTER_H

class CycleCounter {
private:
    static CycleCounter instance;
    CycleCounter() {}
    ~CycleCounter() {}
    CycleCounter(const CycleCounter&) = delete;
    CycleCounter& operator=(const CycleCounter&) = delete;
public:
    static CycleCounter& get_instance() {
        return instance;
    }

    int counter;

    void reset() {
        counter = 0;
    }
};

#endif