#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>
#include <sstream>

const int NumberOfPhilosophers = 5;
const int EatingDuration = 1000;
const int ThinkingDuration = 1000;
const int IterationsLimit = 10;

class Philosopher {
public:
    int id;
    int iterations;

    Philosopher(int id) : id(id), iterations(IterationsLimit) {}

    void operator()() {
        while (iterations > 0) {
            think();
            eat();
            --iterations;
        }
    }

    static std::ostringstream outputBuffer_;

private:
    static std::vector<std::mutex> forks_;
    static std::mutex coutMutex_;

    void think() {
        log("is thinking.");
        std::this_thread::sleep_for(std::chrono::milliseconds(ThinkingDuration));
    }

    void eat() {
        int leftFork = id;
        int rightFork = (id + 1) % NumberOfPhilosophers;

        while (true) {
            if (forks_[leftFork].try_lock()) {
                if (forks_[rightFork].try_lock()) {
                    break;
                }
                else {
                    forks_[leftFork].unlock();
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }

        log("started eating.");
        std::this_thread::sleep_for(std::chrono::milliseconds(EatingDuration));
        forks_[leftFork].unlock();
        forks_[rightFork].unlock();
        log("finished eating.");
    }

    void log(const std::string& action) {
        std::ostringstream localBuffer;
        localBuffer << "Philosopher " << id << " " << action << std::endl;
        std::lock_guard<std::mutex> lock(coutMutex_);
        outputBuffer_ << localBuffer.str();
    }
};

std::ostringstream Philosopher::outputBuffer_;
std::vector<std::mutex> Philosopher::forks_(NumberOfPhilosophers);
std::mutex Philosopher::coutMutex_;

int main() {
    std::vector<std::thread> philosopherThreads;
    philosopherThreads.reserve(NumberOfPhilosophers);

    for (int i = 0; i < NumberOfPhilosophers; ++i) {
        philosopherThreads.emplace_back(Philosopher(i));
    }

    for (auto& thread : philosopherThreads) {
        thread.join();
    }

    std::cout << Philosopher::outputBuffer_.str();
    return 0;
}
