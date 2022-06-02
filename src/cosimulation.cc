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
    this->tck_period = (int)(1000 * (memory->GetTCK()));
    std::cout << "DRAMsim3 memory system initialized." << std::endl;
}

int ComplexCoDRAMsim3::clock_period(){
    return this->tck_period;
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

bool ComplexCoDRAMsim3::check_read_response(int id) {
    return check_response(resp_read_list,id);
}

bool ComplexCoDRAMsim3::check_write_response(int id) {
    return check_response(resp_write_list,id);
}

CoDRAMTrans * ComplexCoDRAMsim3::get_write_response(int id){
    auto iter = resp_write_list.begin();
    while (iter != resp_write_list.end()) {
        auto resp = *iter;
        if(resp->id == id){
            resp_write_list.erase(iter);
            return resp;
        }
    }
    return NULL;
}

CoDRAMTrans * ComplexCoDRAMsim3::get_read_response(int id){
    auto iter = resp_read_list.begin();
    while (iter != resp_read_list.end()) {
        auto resp = *iter;
        if(resp->id == id){
            resp_read_list.erase(iter);
            return resp;
        }
    }
    return NULL;
}


bool ComplexCoDRAMsim3::check_response(std::list<CoDRAMTrans*> &resp_list,int id) {
    auto iter = resp_list.begin();
    while (iter != resp_list.end()) {
        auto resp = *iter;
        if(resp->id == id){
            return true;
        }
    }
    return false;
}

void ComplexCoDRAMsim3::callback(uint64_t addr, bool is_write) {
    if(debug){
    std::cout << "[CallBack] " << std::left << std::setw(18) << std::dec << get_clock_ticks() << std::hex << addr << std::dec << " " << (is_write ? "WRITE " : "READ ") <<  std::endl;
    }
    // search for the first matched request
    auto iter = req_list.begin();
    while (iter != req_list.end()) {
        auto resp = *iter;
        if (resp->address == addr && resp->is_write == is_write) {
            req_list.erase(iter);
            auto &list = (resp->is_write) ? resp_write_list : resp_read_list;
            list.push_back(resp);
            return;
        }
        iter++;
    }
    std ::cout << "INTERNAL ERROR: Do not find matched request for this response "
               << "(0x" << std::hex << addr << ", " << is_write << ")." << std::endl;
    abort();
}

