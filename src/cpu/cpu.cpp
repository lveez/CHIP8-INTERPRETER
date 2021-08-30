#include "cpu.hpp"

#include <time.h>
#include <vector>
#include <fstream>
#include <iostream>

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
    std::ifstream rom_file(rom_filename, std::ios::binary | std::ios::ate);

    if (!rom_file.is_open())   
    {
        std::cout << "ROM file wasn't opened" << std::endl;
        return;
    }

    buffer.resize(rom_file.tellg());

    if (buffer.empty())
    {
        std::cout << "ROM file is empty" << std::endl;
        return;
    }

    rom_file.seekg(0);
    rom_file.read(reinterpret_cast<char*>(&ram[0x0200]), buffer.size());

    size_of_rom = buffer.size();
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
    current_opcode = *reinterpret_cast<word*>(ram.data() + registers.pc);
    registers.pc += 2;
}

// decides what to do based on the current opcode
void CPU::Decode()
{
    switch (current_opcode & opcode_mask)
    {
        case 0x0000:
        {
            switch (current_opcode & byte_mask)
            {
                case 0x00e0:    //  00e0 -> clear screen
                    std::fill(vram.begin(), vram.end(), 0);

                case 0x00ee:    // 00ee -> return from subroutine
                    Pop();
            }
        }
        
        case 0x1000:
        {

        }

        case 0x2000:
        {

        }

        case 0x3000:
        {

        }

        case 0x4000:
        {
            
        }

        case 0x5000:
        {
            
        }

        case 0x6000:
        {
            
        }

        case 0x7000:
        {
            
        }

        case 0x8000:
        {
            
        }

        case 0x9000:
        {
            
        }

        case 0xa000:
        {
            
        }

        case 0xb000:
        {
            
        }

        case 0xc000:
        {
            
        }

        case 0xd000:
        {
            
        }

        case 0xe000:
        {
            
        }

        case 0xf000:
        {
            
        }
    }
}

// loads font set into ram
void CPU::LoadFont()
{
    std::copy(fontset.begin(), fontset.end(), ram.data());
}

// pops address from stack loads into pc
void CPU::Pop()
{
    registers.stack_pointer--;
    registers.pc = stack[registers.stack_pointer];
}

};
};