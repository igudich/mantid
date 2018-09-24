#include <MantidParallel/IO/MultiProcessEventLoader.h>
#include <boost/process/child.hpp>
#include <chrono>
#include <iomanip>
#include <numeric>
#include <thread>

#include "MantidParallel/IO/MultiProcessEventLoader.h"
#include "MantidParallel/IO/NXEventDataLoader.h"
#include "MantidTypes/Event/TofEvent.h"

namespace bp = boost::process;

namespace Mantid {
namespace Parallel {
namespace IO {

/// Constructor
MultiProcessEventLoader::MultiProcessEventLoader(unsigned int numPixels,
                                                 unsigned int numProcesses,
                                                 unsigned int numThreads,
                                                 const std::string &binary,
                                                 bool precalc)
    : m_precalculateEvents(precalc), m_numPixels(numPixels),
      m_numProcesses(numProcesses), m_numThreads(numThreads),
      m_binaryToLaunch(binary),
      m_segmentNames(GenerateSegmentsName(numProcesses)),
      m_storageName(GenerateStoragename()) {}

/// Generates "unique" shared memory segment name
std::vector<std::string>
MultiProcessEventLoader::GenerateSegmentsName(unsigned procNum) {
  std::vector<std::string> res(procNum,
                               GenerateTimeBasedPrefix() +
                                   "_mantid_multiprocess_NXloader_segment_");
  unsigned short i{0};
  for (auto &name : res)
    name += std::to_string(i++);
  return res;
}

/// Generates "unique" shared memory storage structure name
std::string MultiProcessEventLoader::GenerateStoragename() {
  return GenerateTimeBasedPrefix() + "_mantid_multiprocess_NXloader_storage";
}

/// Generates "unique" prefix for shared memory stuff
std::string MultiProcessEventLoader::GenerateTimeBasedPrefix() {
  auto now = std::chrono::system_clock::now();
  auto in_time_t = std::chrono::system_clock::to_time_t(now);

  std::stringstream ss;
  ss << std::put_time(std::localtime(&in_time_t), "%Y%m%d%X");
  return ss.str();
}

/**Main API function for loading data from given file, group list of banks,
 * launches child processes for hdf5 parallel reading*/
void MultiProcessEventLoader::load(
    const std::string &filename, const std::string &groupname,
    const std::vector<std::string> &bankNames,
    const std::vector<int32_t> &bankOffsets,
    std::vector<std::vector<Types::Event::TofEvent> *> eventLists) const {

  try {
    H5::H5File file(filename.c_str(), H5F_ACC_RDONLY);
    auto instrument = file.openGroup(groupname);

    auto bkSz = EventLoader::readBankSizes(instrument, bankNames);
    auto numEvents = std::accumulate(bkSz.begin(), bkSz.end(), 0);

    std::size_t storageSize = estimateShmemAmount(numEvents);

    std::size_t evPerPr = numEvents / m_numProcesses;

    std::vector<bp::child> vChilds;

    // prepare command for launching of parallel processes
    for (unsigned i = 1; i < m_numProcesses; ++i) {
      std::size_t upperBound =
          i < m_numProcesses - 1 ? evPerPr * (i + 1) : numEvents;

      std::string command;
      command += m_binaryToLaunch + " ";
      command += m_segmentNames[i] + " ";           // segment name
      command += m_storageName + " ";               // storage name
      command += std::to_string(i) + " ";           // proc id
      command += std::to_string(evPerPr * i) + " "; // first event to load
      command += std::to_string(upperBound) + " ";  // upper bound to load
      command += std::to_string(m_numPixels) + " "; // pixel count
      command += std::to_string(storageSize) + " "; // memory size
      command += filename + " ";                    // nexus file name
      command += groupname + " ";                   // instrument group name
      command += m_precalculateEvents
                 ? "1 "
                 : "0 "; // variant of algorithm used for loading
      for (unsigned j = 0; j < bankNames.size(); ++j) {
        command += bankNames[j] + " ";                   // bank name
        command += std::to_string(bankOffsets[j]) + " "; // bank size
      }

      try {
        // launch child processes
        vChilds.emplace_back(command.c_str());
      } catch (std::exception const &ex) {
        std::rethrow_if_nested(ex);
      }
    }

    // Do self piece of multiprocess work
    EventsListsShmemStorage storage(m_segmentNames[0], m_storageName,
                                    storageSize, 1, m_numPixels, false);
    fillFromFile(storage, filename, groupname, bankNames, bankOffsets, 0,
                 numEvents / m_numProcesses, m_precalculateEvents);

    // waiting for child processes
    for (auto &c : vChilds)
      c.wait();

    // check if oll childs are finished correctly
    for (auto &c : vChilds)
      if (c.exit_code())
        throw std::runtime_error("Error while multiprocess loading\n");

    // Assemble multiprocess data from shared memory
    assembleFromShared(eventLists);

    // clean up shared memory
    for (const auto &name : m_segmentNames)
      ip::shared_memory_object::remove(name.c_str());
  } catch (std::exception const &ex) {
    for (const auto &name : m_segmentNames)
      ip::shared_memory_object::remove(name.c_str());
    std::rethrow_if_nested(ex);
  }
}

/**Collects data from the chunks in shared memory to the final structure*/
void MultiProcessEventLoader::assembleFromShared(
    std::vector<std::vector<Mantid::Types::Event::TofEvent> *> &result) const {
  std::vector<std::thread> workers;
  std::atomic<int> cnt{0};

  for (unsigned i = 0; i < m_numThreads; ++i) {
    workers.emplace_back([&cnt, this, &result]() {
      std::vector<ip::managed_shared_memory> segments;
      std::vector<Mantid::Parallel::IO::Chunks *> chunksPtrs;
      for (unsigned pid = 0; pid < m_numProcesses; ++pid) {
        segments.emplace_back(ip::open_read_only, m_segmentNames[pid].c_str());
        chunksPtrs.emplace_back(
            segments[pid]
                .find<Mantid::Parallel::IO::Chunks>(m_storageName.c_str())
                .first);
      }

      const unsigned portion{
          std::max<unsigned>(m_numPixels / m_numThreads / 3, 1)};
      for (unsigned startPixel = cnt.fetch_add(portion);
           startPixel < m_numPixels; startPixel = cnt.fetch_add(portion)) {

        for (auto pixel = startPixel;
             pixel < std::min(startPixel + portion, m_numPixels); ++pixel) {
          auto &res = result[pixel];
          for (unsigned i = 0; i < m_numProcesses; ++i) {
            for (auto &chunk : *chunksPtrs[i]) {
              res->insert(res->end(), chunk[pixel].begin(), chunk[pixel].end());
            }
          }
        }
      }
    });
  }

  for (auto &worker : workers)
    worker.join();
}

/**Wrapper for loading the PART of ("from" event "to" event) data
 * from nexus file with different strategies*/
void MultiProcessEventLoader::fillFromFile(
    EventsListsShmemStorage &storage, const std::string &filename,
    const std::string &groupname, const std::vector<std::string> &bankNames,
    const std::vector<int32_t> &bankOffsets, unsigned from, unsigned to,
    bool precalc) {
  H5::H5File file(filename.c_str(), H5F_ACC_RDONLY);
  auto instrument = file.openGroup(groupname);

  auto type =
      EventLoader::readDataType(instrument, bankNames, "event_time_offset");

  if (precalc) {
    if (type == H5::PredType::NATIVE_INT32)
      return GroupLoader<LoadType::preCalcEvents>::loadFromGroup<int32_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_INT64)
      return GroupLoader<LoadType::preCalcEvents>::loadFromGroup<int64_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_UINT32)
      return GroupLoader<LoadType::preCalcEvents>::loadFromGroup<uint32_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_UINT64)
      return GroupLoader<LoadType::preCalcEvents>::loadFromGroup<uint64_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_FLOAT)
      return GroupLoader<LoadType::preCalcEvents>::loadFromGroup<float>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_DOUBLE)
      return GroupLoader<LoadType::preCalcEvents>::loadFromGroup<double>(
          storage, instrument, bankNames, bankOffsets, from, to);
    throw std::runtime_error(
        "Unsupported H5::DataType for event_time_offset in NXevent_data");
  } else {
    if (type == H5::PredType::NATIVE_INT32)
      return GroupLoader<LoadType::producerConsumer>::loadFromGroup<int32_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_INT64)
      return GroupLoader<LoadType::producerConsumer>::loadFromGroup<int64_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_UINT32)
      return GroupLoader<LoadType::producerConsumer>::loadFromGroup<uint32_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_UINT64)
      return GroupLoader<LoadType::producerConsumer>::loadFromGroup<uint64_t>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_FLOAT)
      return GroupLoader<LoadType::producerConsumer>::loadFromGroup<float>(
          storage, instrument, bankNames, bankOffsets, from, to);
    if (type == H5::PredType::NATIVE_DOUBLE)
      return GroupLoader<LoadType::producerConsumer>::loadFromGroup<double>(
          storage, instrument, bankNames, bankOffsets, from, to);
    throw std::runtime_error(
        "Unsupported H5::DataType for event_time_offset in NXevent_data");
  }
}

// Estimates the memory amount for shared memory segments
// vector representing each pixel allocated only once, so we have allocationFee
// bytes extra overhead
size_t MultiProcessEventLoader::estimateShmemAmount(size_t eventCount) const {
  auto allocationFee = 8 + 8 + GenerateStoragename().length();
  std::size_t len{(eventCount / m_numProcesses + eventCount % m_numProcesses) *
      sizeof(TofEvent) +
      m_numPixels * (sizeof(EventLists) + allocationFee) +
      sizeof(Chunks) + allocationFee};
  return len;
}

} // namespace IO
} // namespace Parallel
} // namespace Mantid