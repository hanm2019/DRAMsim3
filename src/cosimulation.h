#ifndef __COSIMULATION_H
#define __COSIMULATION_H

#include <list>
#include <queue>
#include <string>
#include "memory_system.h"
class CoDRAMTrans {
public:
    uint64_t address;
    bool is_write;
    int id;
    int uid;
    int length;
    bool last;
    bool enable;
    bool writeback;

    CoDRAMTrans() : CoDRAMTrans(0, false, 0,0,0,false,false,false) { }
    CoDRAMTrans(uint64_t address, bool is_write,  uint64_t id, int uid, int length,bool last, bool enable, bool writeback)
        : address(address), is_write(is_write),
        id(id),uid(uid),length(length),
        last(last), enable(enable),
        writeback(writeback){ };

    CoDRAMTrans(const CoDRAMTrans* trans){
        this->address = trans->address;
        this->is_write = trans->is_write;
        this->id = trans->id;
        this->uid = trans->uid;
        this->length = trans->length;
        this->last = trans->last;
        this->enable = trans->enable;
        this->writeback = trans->writeback;
    }
};

struct TransQueue{
    std::queue<CoDRAMTrans*> resp_read_queue;
    std::queue<CoDRAMTrans*> resp_write_queue;
};

class CoDRAMsim3 {
public:
    virtual ~CoDRAMsim3() { };
    // Tick the co-sim model.
    virtual void tick() = 0;
    // Returns true on success and false on failure.
    virtual bool will_accept(uint64_t address, bool is_write) = 0;
    // Send request to co-sim model.
    virtual bool add_request(const CoDRAMTrans *request) = 0;

    // Check whether there is some read response available. Returns NULL on failure.
    virtual bool check_read_response(int id) = 0;
    // Check whether there is some write response available. Returns NULL on failure.
    virtual bool check_write_response(int id) = 0;

    // Check whether there is some read response available. Returns NULL on failure.
    virtual CoDRAMTrans *get_read_response(int id) = 0;
    // Check whether there is some write response available. Returns NULL on failure.
    virtual CoDRAMTrans *get_write_response(int id) = 0;
    // Get DRAM ticks.
    inline uint64_t get_clock_ticks() { return dram_clock; }

protected:
    uint64_t dram_clock;
};


class ComplexCoDRAMsim3 : public CoDRAMsim3 {
public:
    // Initialize a DRAMsim3 model.
    ComplexCoDRAMsim3(const std::string &config_file, const std::string &output_dir, int channelNum);
    ~ComplexCoDRAMsim3();
    // Tick the DRAM model.
    void tick();
    // Returns true on success and false on failure.
    bool will_accept(uint64_t address, bool is_write);
    // Send request to CoDRAM model.
    bool add_request(const CoDRAMTrans *request);
    // Check whether there is some read response available. Returns NULL on failure.
    CoDRAMTrans *get_read_response(int id);
    // Check whether there is some write response available. Returns NULL on failure.
    CoDRAMTrans *get_write_response(int id);

    bool check_read_response(int id);
    bool check_write_response(int id);
    bool debug = false;
    void debug_on(){
        this->debug = true;
    }
    void debug_off(){
        this->debug = false;
    }
    int clock_period();
private:
    std::vector<struct TransQueue> transqueue;
    std::list<CoDRAMTrans*> req_list;
    void callback(uint64_t addr, bool is_write);
    // Check whether there is some response in the queue. Returns NULL on failure.
    bool check_response(std::queue<CoDRAMTrans*> &resp_list);
    int tck_period=0;
    dramsim3::MemorySystem *memory = NULL;
};

#endif
