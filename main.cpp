#include <map>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <thread>
#include <mutex>
#include <algorithm>

using namespace std;
mutex mutex1;
//-----Thread-------

void createThread(const vector<string>& vector1, int start, int end,map<string, int>& myMap){
    if(start > vector1.size()){
        return;
    }
    if(start < vector1.size() && end > vector1.size()){
        end = vector1.size();
    }
    map<string, int> count_words;
    for (int i = start; i < end; i++){
        ++count_words[vector1[i]];
    }
   lock_guard<mutex> ls(mutex1);
    for(auto const &word: count_words){
        myMap[word.first] += word.second;
    }
}
 


//realization of clock
inline chrono::high_resolution_clock::time_point get_current_time_fenced()
{
    atomic_thread_fence(memory_order_seq_cst);
    auto res_time = chrono::high_resolution_clock::now();
    atomic_thread_fence(memory_order_seq_cst);
    return res_time;
}
template<class D>
inline long long to_us(const D& d)
{
    return chrono::duration_cast<chrono::microseconds>(d).count();
}


//----For sorting-----
template <typename T1, typename T2>
struct less_second {
    typedef pair<T1, T2> type;
    bool operator ()(type const& a, type const& b) const {
        return a.second < b.second;
    }
};

int main () {
 //--------------------Open file------------------

  string file;
  string word;
  cout << "Please enter a path to file with words: ";
  cin >> file;
  auto open_start_time = get_current_time_fenced();
  ifstream myfile(file);

  vector<string> vector1 = {};

  if (myfile.is_open())
  {
    while (myfile >> word){
      vector1.push_back(word);
    }
    myfile.close();
  }
  else {
      cout << "Unable to open file" << file << endl;
      return 0;
  }
  auto open_end_time = get_current_time_fenced();

  //========================================================

  //--------Threads---------------------

  int numOfThreads;
  cout << "Please enter a number of threads: ";
  cin >> numOfThreads;

  thread threads[numOfThreads];

  int part = int(vector1.size()/numOfThreads);
     if (vector1.size()%numOfThreads != 0){
        part += 1;
     }
  map<string, int> myMap;
  int start = 0;
  int end = part;

  auto counting_start_time = get_current_time_fenced();

  for(int it = 0; it < numOfThreads; it++){
      threads[it] = thread(createThread, cref(vector1),start,end,ref(myMap));
      start += part;
      end += part;
  }
  auto counting_end_time = get_current_time_fenced();
  cout << "\nOpening time: "<< to_us(open_end_time-open_start_time) / (double)(1000) << " ms\n" << endl;
  cout << "Counting time: "<< to_us(counting_end_time-counting_start_time) / (double)(1000) << " ms\n" << endl;
  for(int i = 0; i < numOfThreads; i++){
      threads[i].join();
  }
  //===============================

  //------------Write to file by word------------------
  string wrFile1;
  cout << "Please enter a path to file, where words will be sorted alphabetically: ";
  cin >> wrFile1;
  ofstream outmyfile;
  outmyfile.open(wrFile1);
  if (outmyfile.is_open()){
  for (auto it = myMap.begin(); it != myMap.end(); ++it)
  {
    outmyfile << (*it).first << " : " << (*it).second << "\n";
  }
    outmyfile.close();}
  else{
      cout << "Enable to open a file" << wrFile1 << endl;
      return 0;
  }

  //===================================================

  //-------------Write to file by numbers----------------

    vector<pair<string, int> > mapcopy(myMap.begin(), myMap.end());
    sort(mapcopy.begin(), mapcopy.end(), less_second<string, int>());
    string wrFile2;
    cout << "Please enter a path to file, where words will be sorted by number of each word: ";
    cin >> wrFile2;
    ofstream outmyfile2;
    outmyfile2.open (wrFile2);
    if (outmyfile2.is_open()){
    for (auto it = mapcopy.begin(); it != mapcopy.end(); ++it)
    {
      outmyfile2 << (*it).first << " : " << (*it).second << "\n";
    }
    outmyfile2.close();
    auto total_end_time = get_current_time_fenced();
    cout << "Total time: "<< to_us(total_end_time-open_start_time) / (double)(1000) << " ms\n" << endl;
    }
    else{
        cout << "Enable to open a file" << wrFile2 << endl;
        return 0;
    }

  //==========================================================

  return 0;
}
