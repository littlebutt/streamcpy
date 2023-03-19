#include <iostream>
#include <algorithm>
#include <chrono>
#include <vector>
#include <random>

int main(void) {
    std::default_random_engine e;  

    std::vector<int> v;
    for (size_t i = 0; i < 2 << 10; ++i)
    {
        v.push_back(e());
    }

    auto start = std::chrono::system_clock::now();
    std::vector<int> temp;
    for (auto& _data : v)
    {
        temp.push_back(_data + 1);
    }
    for (auto& _data: temp)
    {
        std::cout<<_data<<std::endl;
    }
    auto end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end - start;
    std::cout << "Time using: " << elapsed_seconds.count() << "s" << std::endl;
    return 0;
}