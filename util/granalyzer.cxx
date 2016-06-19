#ifdef RCNP
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include "GRUTinizerInterface.h"
#include "ThreadsafeQueue.h"
#include "RCNPEvent.h"
#include <atomic>
using namespace std;
/* main */
int main()
{
    const char* filename = "./datatest/run6106.bld";
    ThreadsafeQueue<RCNPEvent*> gr_queue(500000);
    atomic<int> sig(0);
    stringstream stream; stream.str(""); stream << "cat " << filename;
    std::thread grloop(StartGRAnalyzer,stream.str().c_str(),&sig,[&](RCNPEvent* event){
        gr_queue.Push(event);
     },false);
    RCNPEvent* data;
    static int count = 0;
    while (gr_queue.Pop(data,0)) {
        count++;
        if (count > 1e5) { sig = 1;  break; }
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

#else
int main() { return 0; }
#endif
