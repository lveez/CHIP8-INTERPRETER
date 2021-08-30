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

        case 0x6000:    // 6xnn -> v[x] = nn
        {   
            registers.variable[GetXIndex()] = (current_opcode & byte_mask);
            return;
        }

        case 0x7000:    // 7xnn -> v[x] += nn
        {
            registers.variable[GetXIndex()] += (current_opcode & byte_mask);     
            return;
        }

        case 0x8000:
        {
            switch (current_opcode & nibble_mask)
            {
                case 0x0000:    // 8xy0 -> v[x] = v[y]
                    registers.variable[GetXIndex()] = registers.variable[GetYIndex()];
                    return;

                case 0x0001:    // 8xy1 -> v[x] |= v[y]
                    registers.variable[GetXIndex()] |= registers.variable[GetYIndex()];
                    return;

                case 0x0002:    // 8xy2 -> v[x] &= v[y]
                    registers.variable[GetXIndex()] &= registers.variable[GetYIndex()];
                    return;

                case 0x0003:    // 8xy3 -> v[x] ^= v[y]
                    registers.variable[GetXIndex()] ^= registers.variable[GetYIndex()];
                    return;

                case 0x0004:    // 8xy4 -> v[x] += v[y]
                {
                    registers.variable[0x0f] = 0;
                    byte sum = registers.variable[GetXIndex()] + registers.variable[GetYIndex()];
                    if (sum < registers.variable[GetXIndex()])
                        registers.variable[0x0f] = 1;

                    registers.variable[GetXIndex()] = sum;
                    return;
                }

                case 0x0005:    // 8xy5 -> v[x] -= v[y]
                {
                    registers.variable[0x0f] = 0;
                    if (registers.variable[GetXIndex()] > registers.variable[GetYIndex()])
                        registers.variable[0x0f] = 1;

                    registers.variable[GetXIndex()] -= registers.variable[GetYIndex()];
                    return;
                }

                case 0x0006:    // 8xy6 -> v[x] >> 1
                {
                    if (super_chip)
                        registers.variable[GetXIndex()] = registers.variable[GetYIndex()];
                    
                    registers.variable[0x0f] = (registers.variable[GetXIndex()] & 1);                     
                    registers.variable[GetXIndex()] = registers.variable[GetXIndex()] >> 1;
                    return;
                }

                case 0x0007:    // 8xy7 -> v[x] = v[y] - v[x]
                {
                    registers.variable[0x0f] = 0;
                    if (registers.variable[GetYIndex()] > registers.variable[GetXIndex()])
                        registers.variable[0x0f] = 1;

                    registers.variable[GetXIndex()] = registers.variable[GetYIndex()] - registers.variable[GetXIndex()];
                    return;
                }

                case 0x000e:    // 8xye -> v[x] << 1
                {
                    if (super_chip)
                        registers.variable[GetXIndex()] = registers.variable[GetYIndex()];
                    
                    registers.variable[0x0f] = (registers.variable[GetXIndex()] & 128) >> 7;                     
                    registers.variable[GetXIndex()] = registers.variable[GetXIndex()] << 1;
                    return;
                }
            }
        }

        case 0x9000:    // 9xy0 -> skip next instruction if v[x] != v[y]
        {
            if (registers.variable[GetXIndex()] != registers.variable[GetYIndex()])
                registers.pc += 2;
            return;
        }

        case 0xa000:    // annn -> I = nnn
        {
            if ((current_opcode & address_mask) >= 0x1000)
            {
                std::cout << "attempted to load address " << (current_opcode & address_mask)
                << " into the index register which is out of bounds (" << current_opcode << ")" << std::endl;;
                return;
            }
            
            registers.index = current_opcode & address_mask;
        }

        case 0xb000:    // bnnn -> pc = nn + v[0]
        {
            if (super_chip)
                registers.pc = (current_opcode & address_mask) + registers.variable[GetXIndex()];
            else
                registers.pc = (current_opcode & address_mask) + registers.variable[0];
            return;
        }

        case 0xc000:    // cxnn -> rng
        {
            registers.variable[GetXIndex()] = rand() & (current_opcode & byte_mask);
        }

        case 0xd000:    // dxyn -> draw on screen ---- needs tested
        {
            byte height = current_opcode & nibble_mask;
            byte x = registers.variable[GetXIndex()] % 64;
            byte y = registers.variable[GetYIndex()] % 32;
            byte current_byte;
            byte current_pixel;
            registers.variable[0x0f] = 0;
            
            for (int iy = 0; iy < height; iy++)
            {
                if ((y + iy) > 31)
                    break;

                current_byte = ram[registers.index];

                for (int ix = 0; ix < 8; ix++)
                {
                    if ((x + ix) > 63)
                        break;

                    current_pixel = current_byte >> (7 - ix) & 1;
                    if (vram[((y + iy) * 32) + x + ix] && current_pixel)
                        registers.variable[0x0f] = 1;
                    
                    vram[((y + iy) * 32) + x + ix] ^= current_pixel;
                }
            }
            return;
        }

        case 0xe000:  
        {
            switch (current_opcode & byte_mask)
            {
                case 0x9e:  // ex9e -> skip if key v[x] is pressed
                {
                    if (keyboard[registers.variable[GetXIndex()]])
                        registers.pc += 2;
                    
                    return;
                }

                case 0xa1:  // exa1 -> skip if key v[x] isn't pressed
                {
                    if (!keyboard[registers.variable[GetXIndex()]])
                        registers.pc += 2;
                    
                    return;
                }
            }
        }

        case 0xf000:
        {
            switch (current_opcode & byte_mask)
            {
                case 0x07:  // fx07 -> v[x] = delay
                    registers.variable[GetXIndex()] = registers.delay_timer;
                    return;

                case 0x15:  // fx15 -> delay = v[x]
                    registers.delay_timer = registers.variable[GetXIndex()];
                    return;

                case 0x18:  // fx18 -> sound = v[x]
                    registers.sound_timer = registers.variable[GetXIndex()];
                    return;

                case 0x1e:  // fx1e -> I += v[x]
                {
                    registers.variable[0x0f] = 0;
                    word new_address = registers.index + registers.variable[GetXIndex()];
                    if (new_address >= 0x1000)
                    {
                        std::cout << "attempted to load address " << (current_opcode & address_mask)
                        << " into the index register which is out of bounds (" << current_opcode << ")" << std::endl;
                        registers.variable[0x0f] = 1;   // some games rely on flag being set here
                    }
                    registers.index = new_address;
                    return;
                }

                case 0x0a:  // fx0a -> wait for key to be pressed and store it
                {
                    for (int i = 0; i < keyboard.size(); i++)
                    {
                        if (keyboard[i])
                        {
                            registers.variable[GetXIndex()] = i;
                            return;
                        }
                    }
                    registers.pc -= 2;
                    return;
                }

                case 0x29:  // fx29 -> loads character in v[x]
                    registers.index = registers.variable[GetXIndex()] * 5;
                    return;
                
                case 0x33:  // fx33 -> bcd of v[x]
                {
                    byte number = registers.variable[GetXIndex()];                    
                    std::vector<byte> buffer;
                    
                    int i = 0;
                    while (number > 0)
                    {                   
                        buffer.push_back(number % 10);
                        number /= 10;
                        i++;
                    }

                    for (struct { int i; int j;} v = {buffer.size() - 1, 0}; v.i >= 0; v.i--, v.j++)
                    {
                        ram[registers.index + v.j] = buffer[v.i];
                    }
                    return;
                }
                
                case 0x55:  // fx55 -> loads v[0] to v[x] into memory at I
                {
                    if (new_store_load)
                    {
                        for (int i = 0; i <= GetXIndex(); i++)
                        {
                            ram[registers.index + i] = registers.variable[i];
                        }
                        return;
                    }

                    for (int i = 0; i <= GetXIndex(); i++)
                    {
                        ram[registers.index] = registers.variable[i];
                        registers.index++;
                    }
                    return;                    
                }

                case 0x65:  // fx65 -> loads memory at I into v[0] to v[x]
                {
                    if (new_store_load)
                    {
                        for (int i = 0; i <= GetXIndex(); i++)
                        {
                            registers.variable[i] = ram[registers.index + i];
                        }
                        return;
                    }

                    for (int i = 0; i <= GetXIndex(); i++)
                    {
                        registers.variable[i] = ram[registers.index];
                        registers.index++;
                    }
                    return;                    
                }
            }
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