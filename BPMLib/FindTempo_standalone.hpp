#pragma once

#include <algorithm>
#include <functional>
#include <atomic>
#include <iostream>
#include <math.h>
#include <cstring>
#include <fstream>
#include <experimental/filesystem>
#include <vector>
#include <queue>
#include <thread>
#include <cassert>

#include <aubio/aubio.h>
#include "polyfit.h"
#include "Polyfit/PolyfitBoost.hpp"

#ifdef _WIN32
    #define AlignedMalloc(type, count) ((type*)_aligned_malloc((count) * sizeof(type), 16))
    #define AlignedFree(ptr)           if (ptr){_aligned_free(ptr); ptr = nullptr;}
#else
    #define AlignedMalloc(type, count) ((type*)aligned_alloc(16, (count) * sizeof(type)))
    #define AlignedFree(ptr)           if (ptr){free(ptr); ptr = nullptr;}
#endif

#define DEFAULT_THREAD_COUNT 2
// #define MarkProgress(number, text) { if (*data->terminate) {return;} data->progress = number; }

namespace fs = std::experimental::filesystem;

typedef double real_t;

inline void PrintOut(std::string s) {
	std::cout << s << std::endl;
}

inline void MarkProgress(int step, std::string s) {
	std::cout << "[" << step << "] " << s << std::endl;
}

inline void PrintErr(std::string s) {
	std::cerr << "[ERROR] " << s << std::endl;
}

inline void PrintStream(std::ofstream& m, std::string s) {
    m << s << std::endl;
}

struct vec2i {
    int x;
    int y;
};

struct Onset { 
    Onset(int);
    Onset(int, float);
    Onset() : pos(0), strength(0.0f) {}
    
    int pos;
    float strength;

    operator std::string() const
    {
        return "Onset(" + std::to_string(pos) + ", " + std::to_string(strength) + ")";
    }
};

inline Onset::Onset(int i) : pos{i} {}
inline Onset::Onset(int i, float s) : pos{i}, strength{s} {}


template <typename T>
class Vector : public std::vector<T> {
    public:
        // T &operator[](int i) {}
        void append(T obj) { this->std::vector<T>::push_back(obj); }
        void append(float a, float b, float c) { this->std::vector<T>::push_back({a, b, c}); }
        void append(double a, double b, double c) { this->std::vector<T>::push_back({static_cast<float>(a), static_cast<float>(b), static_cast<float>(c)}); }
        void erase(int i) { this->std::vector<T>::erase(this->std::vector<T>::begin() + i); }
        int size() { return (int)this->std::vector<T>::size(); }
        typename std::vector<T>::iterator begin() { return this->std::vector<T>::begin(); }
};

class ParallelThreads {
    private:
	    std::queue<int>* instanceQ;  // contains the instances to be executed
	    bool* statusList;  // contains an index per thread with a bool indicating its status (false = waiting, true = in progress)
        
    public:
        virtual void execute(int i, int t) {};  // t = thread
        
	std::thread* _execute(int i, int t) {
            return new std::thread([this] (int i, int t) { execute(i, t); statusList[t] = false; }, i, t);
        }
        
        void run(int numInstances, int maxThreads) {
            if (numInstances <= 0 || maxThreads <= 0) return;
            
            // initialize bool per thread and a queue for every instance needed to run
            statusList = AlignedMalloc(bool, maxThreads);
            instanceQ = new std::queue<int>();
            
            for (int i = 0; i < maxThreads; i++)
                statusList[i] = false;
            for (int i = 0; i < numInstances; i++)
                instanceQ->push(i);
            
            // this code currently causes memory leaks, instead of having a status list (or WITH a status list) maybe have a pointer list instead so we can join everything

	    std::thread* tp; // thread pointer
            uint8_t t = 0;  // current thread
            while (!instanceQ->empty()) {
                if (!statusList[t]) {
                    statusList[t] = true;
                    // printf("%d @ thread %d started\n", instanceQ->front(), t);
                    tp = _execute(instanceQ->front(), t);
                    instanceQ->pop();
                }
                t = (t + 1) % maxThreads;
            }
            
            // wait for threads to finish
            int finished = 0;  // number of finished threads
            while (finished < maxThreads) {
                finished = (statusList[t] ? 0 : finished + 1);
                t = (t + 1) % maxThreads;
            }
            
            AlignedFree(statusList);
            delete instanceQ;
        }

        static int concurrency() {
            int t = (int)std::thread::hardware_concurrency();
            return t > 0 ? t : DEFAULT_THREAD_COUNT;
        }
};

class Samples {
    public:
        Samples(aubio_source_t* aud, int hop_size);
        const short* samplesL() {
            return left;
        }
        const short* samplesR() {
            return right;
        }
        /* const float* samplesFloat() {
            return mono;
        } */
        int getNumFrames() const { 
            return frame_len;
        }
        int getFrequency() const {
            return freq;
        }
    
    private:
        short* left;
        short* right;
        // float* mono;
        int frame_len;
        int freq;
};

// destructor needs del_aubio_onset (o);
// del_fvec (buf);

struct TempoResult
{
	TempoResult() : fitness(0.0f), bpm(0.0f), offset(0.0f) {}

	float fitness;
    float bpm;
    float offset;

	TempoResult(float a, float b, float c) {
		bpm = a; offset = b; fitness = c;
	}
};

typedef Vector<TempoResult> TempoResults;


struct TempoSort {
	bool operator()(const TempoResult& a, const TempoResult& b) {
		return a.fitness > b.fitness;
	}
};

class SerializedTempo
{
public:
	SerializedTempo() : onsets(nullptr) {}

	float* samples;
	int samplerate;
	int numFrames;
	int numThreads;
	Vector<Onset>* onsets;
	unsigned char* terminate;
	TempoResults result;
};

class GapData
{
	public:
		GapData(int numThreads, int maxInterval, int downsample, int numOnsets, const Onset* onsets);
		~GapData();

		const Onset* onsets;
		int* wrappedPos;
		real_t* wrappedOnsets;
		real_t* window;
		int bufferSize, numOnsets, windowSize, downsample;
};

class IntervalTester
{
public:
	IntervalTester(int samplerate, int numOnsets, const Onset* onsets);
	~IntervalTester();

	int minInterval;
	int maxInterval;
	int numIntervals;
	int samplerate;
	int gapWindowSize;
	int numOnsets;
	const Onset* onsets;
	real_t* fitness;
	real_t coefs[4];
};





class TempoDetector  //: public TempoDetector, public BackgroundThread
{
    public:
        TempoDetector(Samples s, double time, double len);
		TempoDetector(Vector<Onset>* onsets, int sr);
        ~TempoDetector();

        void exec();

        // bool hasSamples() { return (myData.samples != nullptr); }
        // const char* getProgress() const { return sProgressText[myData.progress]; }
        // bool hasResult() const { return isDone(); }
        const Vector<TempoResult>& getResult() const { return myData.result; }

    private:
        SerializedTempo myData;
};


