#include <fstream>
#include <time.h>
#include "MantidAPI/AlgoTimeRegister.h"
#include "MantidKernel/MultiThreaded.h"

namespace Mantid {
namespace Instrumentation {

AlgoTimeRegister::Dump::Dump(AlgoTimeRegister& atr, const std::string& nm)
    : algoTimeRegister(atr), name(nm) {
  clock_gettime(CLOCK_MONOTONIC, &regStart);
}

timespec AlgoTimeRegister::diff(timespec start, timespec end)
{
  timespec temp;
  if ((end.tv_nsec-start.tv_nsec) < 0) {
    temp.tv_sec = end.tv_sec-start.tv_sec-1;
    temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
  } else {
    temp.tv_sec = end.tv_sec-start.tv_sec;
    temp.tv_nsec = end.tv_nsec-start.tv_nsec;
  }
  return temp;
}

AlgoTimeRegister::Dump::~Dump() {
  timespec regFinish;
  clock_gettime(CLOCK_MONOTONIC, &regFinish);
  {
    std::lock_guard<std::mutex> lock(algoTimeRegister.mutex);
    algoTimeRegister.info.emplace_back(
        name, std::this_thread::get_id(), regStart, regFinish
    );
  }
}

AlgoTimeRegister::AlgoTimeRegister()
    : start(std::chrono::high_resolution_clock::now()) {
  clock_gettime(CLOCK_MONOTONIC, &hstart);
}

AlgoTimeRegister::~AlgoTimeRegister() {
  std::fstream fs;
  fs.open("./algotimeregister.out", std::ios::out);
  fs << "START_POINT: "
     << std::chrono::duration_cast<std::chrono::nanoseconds>
         (start.time_since_epoch()).count() << " MAX_THREAD: "
     << PARALLEL_GET_MAX_THREADS << "\n";
  for(auto& elem: info) {
    auto st = diff(hstart, elem.begin);
    auto fi = diff( hstart, elem.end);
    fs << elem.threadId << ">>"
       << elem.name << ":"
       << std::size_t(st.tv_sec*1000000000) + st.tv_nsec << "<>"
       << std::size_t(fi.tv_sec*1000000000) + fi.tv_nsec << "\n";
  }
}

} // namespace API
} // namespace Mantid
