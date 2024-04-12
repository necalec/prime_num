#include <iostream>
#include <thread>
#include <mutex>
#include <vector>
#include <chrono>

using namespace std;
using namespace chrono;

mutex mtx;

const int vecSize = 10000;
const int numThreads = 12;

static bool isPrime(int n) {
    if (n <= 1) return false;
    if (n <= 3) return true;

    if (n % 2 == 0 || n % 3 == 0) return false;

    for (int i = 5; i * i <= n; i += 6) {
        if (n % i == 0 || n % (i + 2) == 0) return false;
    }

    return true;
}

static void findPrime(int start, int end, vector<int>& localPrimes) {
    for (int i = start; i <= end; ++i) {
        if (isPrime(i)) {
            lock_guard<mutex> lock(mtx);
            localPrimes.push_back(i);
        }
    }
}

int main() {
    auto start = high_resolution_clock::now();

    int range = vecSize / numThreads;
    vector<thread> threads;
    vector<vector<int>> results(numThreads);

    for (int i = 0; i < numThreads; ++i) {
        int start = i * range + 1;
        int end = (i == numThreads - 1) ? vecSize : (i + 1) * range;
        threads.push_back(thread(findPrime, start, end, ref(results[i])));
    }

    for (auto& t : threads) {
        t.join();
    }

    vector<int> primes;
    for (const auto& result : results) {
        primes.insert(primes.end(), result.begin(), result.end());
    }

    for (int prime : primes) {
        cout << prime << " ";
    }
    cout << endl;

    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<milliseconds>(stop - start);
    cout << "time: " << duration.count() << endl;

    return 0;
}
//получаем буст только с 10000
