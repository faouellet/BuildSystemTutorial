#include <boost/filesystem.hpp>

#include <iostream>

namespace fs = boost::filesystem;

int main()
{
    std::cout << "Hello world from " << fs::current_path() << "\n";

    return 0;
}