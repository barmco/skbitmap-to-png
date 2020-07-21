#include <fstream>
#include <iostream>
#include <vector>

#include <skbitmap_to_png.h>

int main() {
    std::ifstream file("test/sample", std::ios::binary | std::ios::ate);
    size_t size = file.tellg();
    
    file.seekg(0, std::ios::beg);

    std::vector<char> buffer(size);
    if(file.read(buffer.data(), buffer.size())) {
        auto res = transform_to_bgra8888(800, 400, buffer.size(), buffer.data());

        std::ofstream file2("test/sample.bgra8888", std::ios::binary);
        file2.write(reinterpret_cast<char *>(res.encoded), res.size);
        
        memfree(res.handle);
    }
}