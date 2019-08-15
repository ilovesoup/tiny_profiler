#include "profiler.h"
#include <iostream>

using namespace std;

void task(int times)
{
  Profiler::Profiler profiler("thread task");
  int counter = 0;
  for (int i = 0; i < times; i++)
  {
    counter ++;
  }
  cout << counter << endl;
}

int main()
{
  Profiler::ProfilerManager<chrono::microseconds> trigger("/data/profile.log");
  Profiler::Profiler profiler("Total");

  int times = 0;
  cin >> times;

  thread t1(task, times);
  thread t2(task, times * 2);
  thread t3(task, times * 3);

  {
    Profiler::Profiler profiler("section 1");
    int counter = 0;
    for (int i = 0; i < times; i++)
    {
      counter ++;
    }
    cout << counter << endl;
  }

  {
    Profiler::Profiler profiler("section 2");
    double counter = 1.1;
    for (int i = 0; i < times * 2; i++)
    {
      counter *= 3.1;
    }
    cout << counter << endl;
  }

  double counter = 1.1;
  for (int i = 0; i < times * 2; i++)
  {
    Profiler::Profiler profiler("section 3");
    counter *= 3.1;
  }
  cout << counter << endl;

  t1.join();
  t2.join();
  t3.join();

  return 0;
}