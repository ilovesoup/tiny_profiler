#pragma once

#include <fstream>
#include <sstream>
#include <map>
#include <string>
#include <thread>
#include <cstdio>


#define __THREAD_LOCAL thread_local

namespace Profiler {
using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;

struct ProfilerData {
  // should have an identical name
  ProfilerData() {
    timeElps = TimePoint::duration::zero();
    hitCount = 0;
  }

  void add(const TimePoint::duration & duration) {
    hitCount ++;
    timeElps += duration;
  }

  TimePoint::duration timeElps;
  int hitCount;
};

using DataMap = std::map<const char *, ProfilerData *>;

extern std::map<std::__thread_id, DataMap *> globalDataMap;
extern std::mutex gMapMutex;

class ProfilerManagerBase {
public:
  static ProfilerData * initProfilerData(const char * modName);
protected:
  static void clearAll();
  static __THREAD_LOCAL DataMap * mapModData;
};

template <typename T>
class ProfilerManager : public ProfilerManagerBase {
public:
  ProfilerManager(const char * fileName) {
    this->fileName = fileName;
    clearAll();
  }

  static void output(const char * fileName)
  {
    std::lock_guard<std::mutex> lock(gMapMutex);
    for (auto git = globalDataMap.begin(); git != globalDataMap.end(); git++) {
      std::stringstream ss;
      ss << fileName << ".pid-" << git->first;
      std::ofstream out(ss.str());

      for (auto it = git->second->begin(); it != git->second->end(); it++) {
        out << "Module Name : " << it->first << std::endl;
        out << "Hit Count : " << it->second->hitCount << std::endl;
        out << "Total Time Elapsed : "
            << std::chrono::duration_cast<T>(it->second->timeElps).count()
            << std::endl;
        out << std::endl;
      }
      out.close();
    }
  }

  ~ProfilerManager() { output(this->fileName); }

private:
  const char * fileName;
};

class Profiler {
public:
  Profiler(const char * modName) {
    this->pData = ProfilerManagerBase::initProfilerData(modName);
    start = std::chrono::high_resolution_clock::now();
  }

  ~Profiler() {
    pData->add(std::chrono::high_resolution_clock::now() - start);
  }

private:
  ProfilerData *pData;
  TimePoint start;
};

}