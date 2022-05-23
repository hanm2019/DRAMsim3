#ifndef __COSIMULATION_H
#define __COSIMULATION_H

#include <list>
#include <queue>
#include <string>

class CoDRAMTrans {
public:
    uint64_t address;
    bool is_write;
    uint64_t id;
    int burstLength;
    int burstType;
    int dramBurstLength;
    int dramBurstOffset;

    CoDRAMTrans() : CoDRAMTrans(0, false, 0,0,0,0,0) { }
    CoDRAMTrans(uint64_t address, bool is_write,  uint64_t id, int burstLength, int burstType,
                  int dramBurstLength, int dramBurstOffset)
        : address(address), is_write(is_write),
        id(id),burstLength(burstLength),
        burstType(burstType), dramBurstLength(dramBurstLength),
        dramBurstOffset(dramBurstOffset){ };

    CoDRAMTrans(const CoDRAMTrans* trans){
        this->address = trans->address;
        this->is_write = trans->is_write;
        this->id = trans->id;
        this->burstType = trans->burstType;
        this->burstLength = trans->burstLength;
        this->dramBurstOffset = trans->dramBurstOffset;
        this->dramBurstLength = trans->dramBurstLength;
    }
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
    virtual bool check_read_response() = 0;
    // Check whether there is some write response available. Returns NULL on failure.
    virtual bool check_write_response() = 0;

    // Check whether there is some read response available. Returns NULL on failure.
    virtual CoDRAMTrans *get_read_response() = 0;
    // Check whether there is some write response available. Returns NULL on failure.
    virtual CoDRAMTrans *get_write_response() = 0;
    // Get DRAM ticks.
    inline uint64_t get_clock_ticks() { return dram_clock; }

protected:
    uint64_t dram_clock;
};


class ComplexCoDRAMsim3 : public CoDRAMsim3 {
public:
    // Initialize a DRAMsim3 model.
    ComplexCoDRAMsim3(const std::string &config_file, const std::string &output_dir);
    ~ComplexCoDRAMsim3();
    // Tick the DRAM model.
    void tick();
    // Returns true on success and false on failure.
    bool will_accept(uint64_t address, bool is_write);
    // Send request to CoDRAM model.
    bool add_request(const CoDRAMTrans *request);
    // Check whether there is some read response available. Returns NULL on failure.
    CoDRAMTrans *get_read_response();
    // Check whether there is some write response available. Returns NULL on failure.
    CoDRAMTrans *get_write_response();

    bool check_read_response();
    bool check_write_response();
    bool debug = false;
    void debug_on(){
        this->debug = true;
    }
    void debug_off(){
        this->debug = false;
    }
private:

    std::list<CoDRAMTrans*> req_list;
    std::queue<CoDRAMTrans*> resp_read_queue;
    std::queue<CoDRAMTrans*> resp_write_queue;

    void callback(uint64_t addr, bool is_write);
    // Check whether there is some response in the queue. Returns NULL on failure.
    bool check_response(std::queue<CoDRAMTrans*> &resp_queue);
};

#endif
