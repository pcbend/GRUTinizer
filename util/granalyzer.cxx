#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include "GRUTinizerInterface.h"
#include "ThreadsafeQueue.h"
#include "RCNPEvent.h"

using namespace std;
/* main */
int main()
{
    const char* filename = "./datatest/run1001.bld";
    ThreadsafeQueue<RCNPEvent> gr_queue;
    stringstream stream; stream.str(""); stream << "cat " << filename;
    std::thread grloop(StartGRAnalyzer,stream.str().c_str(),[&](RCNPEvent* event){
        gr_queue.Push(*event);
    });
    RCNPEvent data;
    while (gr_queue.Pop(data,0)) {

    }
    grloop.join();

    return 0;
}

// std::promise<void> flag;
// std::thread grloop([&](){ StartGRAnalyzer("./datatest/run6106.bld",[&](RCNPEvent* event){
//                 gr_queue.Push(*event);
//             }); flag.set_value(); });
// std::this_thread::sleep_for(std::chrono::milliseconds(1000));

// auto future = flag.get_future();
// while(future.wait_for(std::chrono::milliseconds(0)) != std::future_status::ready) {
//     std::cout << gr_queue.Size() << std::endl;
// }
// grloop.join();
