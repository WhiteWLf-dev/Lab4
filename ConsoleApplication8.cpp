#include <iostream>
#include <vector>
#include <random>
#include <chrono>
size_t maxBound=1<<20;
class ModifiedLCRNG {
private:
    size_t seed;
    const size_t a = 6364136223846793005; // (a*seed+c) mod m
    const size_t c = 1442695040888963407;
    const size_t m =1<<31;
    const size_t shift = 10; // shift bits
public:
    ModifiedLCRNG(size_t seed) {
        this->seed = seed;
    }
   size_t generate() {
        seed = (a * seed + c) % m;
        
        return (seed >> shift)%maxBound; 
    }
};
class ModifiedXorShift {
private:
    size_t x, y, z, w;
    const size_t shift = 10; // shift bits
public:
    ModifiedXorShift(size_t seed) {
        x = seed;
        y = 362436069;
        z = 521288629;
        w = 88675123;
    }
    size_t generate() {
        size_t t = x ^ (x << 11);
        x = y; 
        y = z; 
        z = w;
        w = (w ^ (w >> 19)) ^ (t ^ (t >> 8));
        return (w >> shift) % maxBound; 
    }
};
double mean(std::vector<size_t>& a) {
    size_t sum = 0;

    for (auto& r_i : a) {
        sum += r_i;
    }

    double n = a.size();

    return sum / n;
}
double deviation(std::vector<size_t>& a,double mean) {
    double sum = 0;

    for (auto& ch : a)
        sum += (ch - mean) * (ch - mean);

    return sqrt(sum / a.size());
}
double variationCoeff(double deviation, double mean) {
    return deviation / mean;
}
double chiSquareTest(std::vector<size_t>& vec) {
    //нормировка
    std::vector<double> v;
    for (int i = 0; i < vec.size(); ++i)
        v.push_back(vec[i] / static_cast<double>(maxBound - 1));

    const size_t n = v.size();
    const size_t k = 1 + 3.322 * log(n);
    const double p = 1 / static_cast<double>(k);

    std::vector<size_t> n_i(k, 0);

    for (auto& u : v)
        for (int j = 0; j < k; ++j)
            if (p * j <= u && p * (j + 1) > u)
                n_i[j] += 1;

    double chi = 0;

    for (auto& u : n_i)
        chi += ((static_cast<double>(u) - p * n) * (static_cast<double>(u) - p * n)) / (p * n);

    return chi;
}
int main() {
    
    int arr[10] = {rand(),rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand(), rand() };
    int arr1[10] = { 1000, 10000, 100000,200000,300000,400000,500000,600000,800000,1000000 };
    int selection[10] = { 1,2,3,4,5,6,7,8,9,10 };
    
    for (int i = 0; i <= 9; ++i) {
        std::vector<size_t> a1,a2;
        ModifiedLCRNG rng1(arr[i]);
        ModifiedXorShift rng2(arr[i]);
        for (int j = 0; j < 102; j++) {
            a1.push_back(rng1.generate());
            a2.push_back(rng2.generate());
        }
        a1.resize(101);
        a2.resize(101);
        std::cout << "For selections number " << selection[i] << "\n";
        std::cout << " ModifiedLCRNG :\n";
        
        std::cout << "Mean: " << mean(a1)<< "\n";
        std::cout << "Deviation: " << deviation(a1, mean(a1)) << "\n";
        std::cout << "Variation coefficient: " << variationCoeff(deviation(a1, mean(a1)), mean(a1))<<"\n";
        std::cout << "Chi-Squre Test: " << chiSquareTest(a1) << "\n";
        std::cout << " ModifiedXorShift :\n";
        
        std::cout << "Mean: " << mean(a2) << "\n";
        std::cout << "Deviation: " << deviation(a2, mean(a2)) << "\n";
        std::cout << "Variation coefficient: " << variationCoeff(deviation(a2, mean(a2)), mean(a2)) << "\n";
        std::cout << "Chi-Squre Test: " << chiSquareTest(a2) << "\n";
    }
    for (int i = 0; i < 10; ++i) {
        ModifiedLCRNG rng1(arr[i]);
        ModifiedXorShift rng2(arr[i]);
        std::vector <size_t> v;
        auto begin = std::chrono::steady_clock::now();       
        for (int j = 0; j < arr1[i]; ++j) {
            v.push_back(rng1.generate());
        }
        auto end = std::chrono::steady_clock::now();
        auto elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        std::cout << "The time of generating " << arr1 [i] << " elements through ModifiedLCRNG : " << elapsed_ms.count() << " mc\n";
        std::vector <size_t> v1;
        begin = std::chrono::steady_clock::now();
        for (int j = 0; j < arr1[i]; ++j) {
            v1.push_back(rng2.generate());
        }
        end = std::chrono::steady_clock::now();
        elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - begin);
        std::cout << "The time of generating " << arr1[i] << " elements through ModifiedXorShift : " << elapsed_ms.count() << " mc\n";
    } 
    return 0;
}