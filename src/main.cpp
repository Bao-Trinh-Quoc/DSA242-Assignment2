#include <iostream>
#include <iomanip>
#include <sstream>
#include <string>
#include "test/tc_xmap.h"
#include "test/tc_heap.h"
#include "test/tc_compressor.h"

// Remember to comment out when submitting
#include "test/tc_xarraylist.h"
#include "test/tc_dlinkedlist.h"
#include "test/tc_inventory.h"

using namespace std;

void (*func_ptr[17])() = {
    hashDemo1,
    hashDemo2,
    hashDemo3,
    hashDemo4,
    hashDemo5,
    hashDemo6,
    hashDemo7,
    heapDemo1,
    heapDemo2,
    heapDemo3,
    // tc_huffman1001,
    // tc_huffman1002,
    // tc_huffman1003,
    // tc_huffman1004,
    // tc_huffman1005,
    // tc_compressor1001,
    // tc_compressor1002
};

// void (*func_ptr[15])() = {
//     dlistDemo1,
//     dlistDemo2,
//     dlistDemo3,
//     dlistDemo4,
//     dlistDemo5,
//     dlistDemo6,
//     xlistDemo1,
//     xlistDemo2,
//     xlistDemo3,
//     xlistDemo4,
//     tc_inventory1001,
//     tc_inventory1002,
//     tc_inventory1003,
//     tc_inventory1004,
//     tc_inventory1005,
// };

void run(int func_idx)
{
    func_ptr[func_idx]();
}

int main(int argc, char **argv)
{
    // cout << "Assignment-2" << endl;
    if (argc == 1) {
        // hashDemo1();
        for (int i = 0; i < 10; i++) {
            cout << "==========Running test case=======: " << i + 1 << endl;
            run(i);
        }
    }
    
    return 0;
}