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
    // to preserve endian-ness
    byte high_byte = ram[registers.pc];
    byte low_byte = ram[registers.pc + 1];

    current_opcode = (high_byte << 8) | low_byte;
    
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
                    return;

                case 0x00ee:    // 00ee -> return from subroutine
                    Pop();
                    return;

                default:
                    std::cout << "unknown opcode -- " << current_opcode << std::endl;
                    return;
            }
        }
        
        case 0x1000:    // 1nnn -> jump to address nnn 
        {
            if ((current_opcode & address_mask) >= 0x1000)
            {
                std::cout << "attempted to jump to address " << (current_opcode & address_mask) 
                << " which is out of bounds" << std::endl;
                return;
            } 
            registers.pc = current_opcode & address_mask;
            return;
        }

        case 0x2000:    // 2nnn -> call subroutine at address nnn
        {
            if ((current_opcode & address_mask) >= 0x1000)
            {
                std::cout << "attempted to call subroutine at address " << (current_opcode & address_mask)
                << " which is out of bounds";
                return;
            }
            Push();
            registers.pc = current_opcode & address_mask;
            return;
        }

        case 0x3000:    // 3xnn -> skip next instruction if v[x] == nn
        {
            if (registers.variable[GetXIndex()] == (current_opcode & byte_mask))
                registers.pc += 2;
            return;
        }

        case 0x4000:    // 4xnn -> skip next instruction if v[x] != nn
        {
            if (registers.variable[GetXIndex()] != (current_opcode & byte_mask))
                registers.pc += 2;
            return;
        }

        case 0x5000:    // 5xy0 -> skip next instruction if v[x] == v[y]
        {
            if (registers.variable[GetXIndex()] == registers.variable[GetYIndex()])
                registers.pc += 2;
            return;
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

        case 0x9000:    // 9xy0 -> skip next instruction if v[x] != v[y]
        {
            if (registers.variable[GetXIndex()] != registers.variable[GetYIndex()])
                registers.pc += 2;
            return;
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

// ?x?? isolates x to get the reg_variable index
byte CPU::GetXIndex() const
{
    return (current_opcode & x_reg_mask) >> 8;
}

// ??y? isolates y to get the reg_variable index
byte CPU::GetYIndex() const
{
    return (current_opcode & y_reg_mask) >> 4;
}

// pushes current value of pc to stack
void CPU::Push()
{
    stack[registers.stack_pointer] = registers.pc;
    registers.stack_pointer++;
}

// pops address from stack loads into pc
void CPU::Pop()
{
    registers.stack_pointer--;
    registers.pc = stack[registers.stack_pointer];
}

};
};