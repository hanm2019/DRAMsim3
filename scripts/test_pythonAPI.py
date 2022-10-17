import ctypes
from ctypes import *

lib = CDLL("../libDramSimulator.so")

addressList = [1,2,3,4,5,6]

address_1d = (ctypes.c_int * len(addressList))(*(i for i in addressList))

lib.test_dram_list.restype = ctypes.c_int

res_list_1d = lib.test_dram_list(byref(address_1d), len(addressList))

print(res_list_1d)