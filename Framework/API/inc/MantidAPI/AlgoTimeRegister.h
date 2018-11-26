// Mantid Repository : https://github.com/mantidproject/mantid
//
// Copyright &copy; 2018 ISIS Rutherford Appleton Laboratory UKRI,
//     NScD Oak Ridge National Laboratory, European Spallation Source
//     & Institut Laue - Langevin
// SPDX - License - Identifier: GPL - 3.0 +
#ifndef MANTID_API_ALGOTIMEREGISTER_H_
#define MANTID_API_ALGOTIMEREGISTER_H_

#include <thread>
#include <mutex>
#include <vector>

#include "MantidAPI/DllConfig.h"

class timespec;

namespace Mantid {
namespace Instrumentation {

/** AlgoTimeRegister : TODO: DESCRIPTION
*/
class AlgoTimeRegister {
public:
  static AlgoTimeRegister globalAlgoTimeRegister;
  static timespec diff(timespec start, timespec end);
  struct Info {
    std::string name;
    std::thread::id threadId;
    timespec begin;
    timespec end;

    Info(const std::string& nm, const std::thread::id& id, const timespec& be, const timespec& en) :
        name(nm), threadId(id), begin(be), end(en) {}
  };

  class Dump {
    AlgoTimeRegister& algoTimeRegister;
    timespec regStart;
    const std::string name;
  public:
    Dump(AlgoTimeRegister& atr, const std::string& nm);
    ~Dump();
  };

  AlgoTimeRegister();
  ~AlgoTimeRegister();
private:
  std::mutex mutex;
  std::vector<Info> info;
  timespec hstart;
  std::chrono::high_resolution_clock::time_point start;
};

} // namespace API
} // namespace Mantid

#endif /* MANTID_API_ALGOTIMEREGISTER_H_ */