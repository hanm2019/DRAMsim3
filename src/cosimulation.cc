#include "cosimulation.h"
#include "memory_system.h"

dramsim3::MemorySystem *memory = NULL;

ComplexCoDRAMsim3::ComplexCoDRAMsim3(const std::string &config_file, const std::string &output_dir) {
    if (memory) {
        std::cout << "should only init one memory currently" << std::endl;
        abort();
    }
    dram_clock = 0;
    memory = new dramsim3::MemorySystem(config_file, output_dir,
        std::bind(&ComplexCoDRAMsim3::callback, this, std::placeholders::_1, false),
        std::bind(&ComplexCoDRAMsim3::callback, this, std::placeholders::_1, true));
    std::cout << "DRAMsim3 memory system initialized." << std::endl;
}

ComplexCoDRAMsim3::~ComplexCoDRAMsim3() {
    memory->PrintStats();
    delete memory;
}

void ComplexCoDRAMsim3::tick() {
    memory->ClockTick();
    dram_clock++;
}

bool ComplexCoDRAMsim3::will_accept(uint64_t address, bool is_write) {
    return memory->WillAcceptTransaction(address, is_write);
}

bool ComplexCoDRAMsim3::add_request(const CoDRAMTrans *request) {
        memory->AddTransaction(request->address, request->is_write);
        req_list.push_back(new CoDRAMTrans(request));
        return true;
}

bool ComplexCoDRAMsim3::check_read_response() {
    return check_response(resp_read_queue);
}

bool ComplexCoDRAMsim3::check_write_response() {
    return check_response(resp_write_queue);
}

CoDRAMTrans * ComplexCoDRAMsim3::get_write_response(){
    CoDRAMTrans* response = resp_write_queue.front();
    resp_write_queue.pop();
    return response;
}

CoDRAMTrans * ComplexCoDRAMsim3::get_read_response(){
    CoDRAMTrans* response = resp_read_queue.front();
    resp_read_queue.pop();
    return response;
}


bool ComplexCoDRAMsim3::check_response(std::queue<CoDRAMTrans*> &resp_queue) {
    if (resp_queue.empty())
        return false;
    else
        return true;
}

void ComplexCoDRAMsim3::callback(uint64_t addr, bool is_write) {
     std::cout << "cycle " << std::dec << get_clock_ticks() << " callback "
               << "is_write " << std::dec << is_write << " addr " << std::hex << addr << std::endl;
    // search for the first matched request
    auto iter = req_list.begin();
    while (iter != req_list.end()) {
        auto resp = *iter;
        if (resp->address == addr && resp->is_write == is_write) {
            req_list.erase(iter);
            auto &queue = (resp->is_write) ? resp_write_queue : resp_read_queue;
            queue.push(resp);
            return;
        }
        iter++;
    }
    std ::cout << "INTERNAL ERROR: Do not find matched request for this response "
               << "(0x" << std::hex << addr << ", " << is_write << ")." << std::endl;
    abort();
}

