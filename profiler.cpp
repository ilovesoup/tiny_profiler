#include "profiler.h"
#include <memory>
#include <mutex>

namespace Profiler {

std::map<std::__thread_id, DataMap *> globalDataMap;
std::mutex gMapMutex;

DataMap * initThreadLocalMap() {
  std::lock_guard<std::mutex> lock(gMapMutex);
  auto ptr = new DataMap;
  globalDataMap[std::this_thread::get_id()] = ptr;
  return ptr;
}

__THREAD_LOCAL DataMap * ProfilerManagerBase::mapModData = initThreadLocalMap();


ProfilerData *ProfilerManagerBase::initProfilerData(const char *modName) {
  auto it = mapModData->find(modName);
  if (it != mapModData->end()) {
    return it->second;
  } else {
    ProfilerData * ptr = new ProfilerData();
    mapModData->emplace(modName, ptr);
    return ptr;
  }
}

void ProfilerManagerBase::clearAll() {
  for (auto it = mapModData->begin(); it != mapModData->end(); it++) {
    delete it->second;
  }
  mapModData->clear();
}

}