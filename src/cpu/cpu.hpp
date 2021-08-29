#ifndef CPU_H
#define CPU_H

#include <cstdint>
#include <array>
#include <string>

namespace chip8
{
namespace cpu
{
// alias types
using byte = std::uint8_t;
using word = std::uint16_t;

// bitmasks to manipulate opcode
const word  opcode_mask = 0xf000;
const word address_mask = 0x0fff;
const word   x_reg_mask = 0x0f00;
const word   y_reg_mask = 0x00f0;
const word    byte_mask = 0x00ff;
const word  nibble_mask = 0x000f;



// contains all the registers
struct Registers
{
    word pc;
    word index;
    byte delay_timer;
    byte sound_timer;
    std::array<byte, 16> variable;
    byte stack_pointer;
};



class CPU
{
private:    // internal components
    Registers             registers = { 0x0200, 0x0000, 0x00,
                                        0x00,   { 0 },  0x00};
    std::array<byte, 0x1000>    ram = { 0 };
    std::array<byte, 64 * 32>  vram = { 0 };
    std::array<word, 16>      stack = { 0 };
    std::array<byte, 16>   keyboard = { 0 };

private:    // private data
    word current_opcode;
    word size_of_rom;

public:     // constructors and destructors
    CPU();
    CPU(const CPU& cpu) = delete;
    CPU(CPU&& cpu) = delete;
    virtual ~CPU() = default;

public:     // public functions
    void Init();
    void LoadROM(const std::string& rom_filename);
    void Cycle();

private:    // private functions
    void Fetch();
    void Decode();
    void LoadFont();

private:    // opcode implementations
};

};
};

#endif