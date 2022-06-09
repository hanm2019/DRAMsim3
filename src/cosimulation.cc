#include "cosimulation.h"



ComplexCoDRAMsim3::ComplexCoDRAMsim3(const std::string &config_file, const std::string &output_dir, int channelNum):
    transqueue(channelNum)
{
    if (memory || channelNum <= 0) {
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
    return !transqueue.at(id).resp_read_queue.empty();
}

bool ComplexCoDRAMsim3::check_write_response(int id) {
    return !transqueue.at(id).resp_write_queue.empty();
}

CoDRAMTrans * ComplexCoDRAMsim3::get_write_response(int id){
    auto response = transqueue.at(id).resp_write_queue.front();
    transqueue.at(id).resp_write_queue.pop();
    return response;
}

CoDRAMTrans * ComplexCoDRAMsim3::get_read_response(int id){
    auto response = transqueue.at(id).resp_read_queue.front();
    transqueue.at(id).resp_read_queue.pop();
    return response;
}


bool ComplexCoDRAMsim3::check_response(std::queue<CoDRAMTrans*> &resp_queue) {
    return !resp_queue.empty();
}

void ComplexCoDRAMsim3::callback(uint64_t addr, bool is_write) {
    // search for the first matched request
    auto iter = req_list.begin();
    while (iter != req_list.end()) {
        auto resp = *iter;
        if (resp->address == addr && resp->is_write == is_write) {
            if(debug){
                std::cout << "[CallBack] " << std::left << std::setw(18) << std::dec << get_clock_ticks() <<" Channel:"<< resp->id <<" Addr:"<<std::hex << addr << std::dec << " " << (is_write ? "WRITE " : "READ ") <<  std::endl;
            }
            req_list.erase(iter);
            if(resp->is_write){
                transqueue.at(resp->id).resp_write_queue.push(resp);
            } else{
                transqueue.at(resp->id).resp_read_queue.push(resp);
            }
            return;
        }
        iter++;
    }
    std ::cout << "INTERNAL ERROR: Do not find matched request for this response "
               << "(0x" << std::hex << addr << ", " << is_write << ")." << std::endl;
    abort();
}

