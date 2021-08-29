#include "cpu.hpp"

#include <time.h>
#include <vector>
#include <fstream>

namespace chip8
{
namespace cpu
{

CPU::CPU()
{
    Init();
}

// initializes internals to correct state and seeds random
void CPU::Init()
{
    LoadFont();
    srand(time(nullptr));
}

void CPU::LoadROM(const std::string& rom_filename)
{
    std::vector<byte> buffer;
    buffer.reserve(0x1000);     // reserve the capacity since storage isn't an issue
    std::ifstream rom_file(rom_filename, std::ios::binary);
    byte current_byte;

    while (rom_file.good())
    {
        rom_file >> current_byte;
        buffer.emplace_back(current_byte);
    }

    size_of_rom = buffer.size();
    std::copy(buffer.begin(), buffer.end(), &ram[0x0200]);
}

// runs one full cycle
void CPU::Cycle()
{
    Fetch();
    Decode();
}

// gets the opcode at the current value of the PC then updates PC
void CPU::Fetch() 
{
    current_opcode = *reinterpret_cast<word*>(&ram[registers.pc]);
    registers.pc += 2;
}

// decides what to do based on the current opcode
void CPU::Decode()
{
    switch (current_opcode & opcode_mask)
    {

    }
}

// loads font set into ram
void CPU::LoadFont()
{
    std::copy(fontset.begin(), fontset.end(), ram.data());
}

};
};