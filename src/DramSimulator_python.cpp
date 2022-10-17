#include "DramSimulator_python.h"

int test_dram_list(int * address_list, int address_list_size){
    int a  = 0;
    for (int i = 0 ;i < address_list_size; i++){
        a += address_list[i];
    }
    return a;
}