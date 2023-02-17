
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

#include <wz/Wz.hpp>
#include <wz/Node.hpp>
#include <wz/File.hpp>
#include <wz/Reader.hpp>
#define U8 static_cast<uint8_t>
#define IV4(A,B,C,D) {U8(A),U8(B),U8(C),U8(D)}

void u642iv(uint64_t u64, uint8_t* iv) {
    for (int i = 0; i < 4; ++i) {
        iv[i] = u64 >> (3-i)*8 & 0xFF;
    }
}
uint64_t iv2u64(uint8_t* iv) {
    uint64_t u64 = 0;
    for (int i = 0; i < 4; ++i) {
        u64 += uint64_t(iv[i]) << (3-i)*8;
    }
    return u64;
}
bool nextiv(uint8_t* iv) {
    uint64_t u64 = iv2u64(iv) + 1;
    if (u64 > uint64_t(0xFFFFFFFF)) {
        return false;
    }
    u642iv(u64, iv);
    return true;
}

int main(int argc, char** argv) {
    if (argc <= 1) {
        std::cout << "crackimg <path of img>" << std::endl;
        return -1;
    }
    uint8_t iv[] = IV4(0x4D, 0x23, 0xC7, 1);
    uint64_t ivend = 0;
    auto filepath = std::string(argv[1]);
    bool quiet = false;
    bool iswz = false;
    // parse argv
    if (argc > 2) {
        auto cmd = std::string(argv[2]);
        if (cmd == "-q" || cmd == "-qiv" || cmd == "-qivr") {
            quiet = true;
        } 
        if (argc > 3 && (cmd == "-iv" || cmd == "-qiv" || cmd == "-ivr" || cmd == "-qivr")) {
            uint64_t ivhex;
            std::istringstream iss(argv[3]);
            iss >> std::hex >> ivhex;
            u642iv(ivhex, iv);
        }
        if (argc > 4 && (cmd == "-ivr" || cmd == "-qivr")) {
            std::istringstream iss(argv[4]);
            iss >> std::hex >> ivend;
        }
        if (filepath[filepath.size()-2] == 'w' && filepath[filepath.size()-1] == 'z') {
            iswz = true;
        }
    }
    //wz::Reader reader(key, filepath.c_str());
    wz::File file({iv[0],iv[1],iv[2],iv[3]}, filepath.c_str());
    wz::Reader &reader = file.get_reader();
    bool iscracked = false;
    do {
        auto ivhex = iv2u64(iv);
        if (ivhex >= ivend) {
            break;
        }
        file.set_iv({iv[0],iv[1],iv[2],iv[3]});
        if (!iswz) {
            // image file
            iscracked = reader.is_has_property();
        } else {
            // wz file
            iscracked = file.parse();
        }
        if (!quiet) {
            std::cout << "cracking: " << argv[1] << " res: " << iscracked << " the IV: " << std::hex;
            for (auto i : iv) {
                std::cout << "0x" << int(i) << " ";
            }
            std::cout << "left: " << uint64_t(0xFFFFFFFF)-ivhex << std::endl;
        } 
        reader.set_position(0);
    } while(!(iscracked || !nextiv(iv)));
    if (iscracked) {
        std::cout << "cracked:  " << argv[1] << " the IV: " << std::hex;
        for (auto i : iv) {
            std::cout << "0x" << int(i) << " ";
        }
        std::cout << std::endl;
    } else {
        std::cout << "crack end: 0x" << std::hex << ivend << std::endl;
    }
    
    return 0;
}