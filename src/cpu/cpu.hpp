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
using byte = uint8_t;
using word = uint16_t;

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
    word current_opcode = 0;
    word    size_of_rom = 0;

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
    byte GetXIndex() const;
    byte GetYIndex() const;
    void Push();
    void Pop();
    
private:    //fontset
    const std::array<byte, 80> fontset =
    {
        0xF0, 0x90, 0x90, 0x90, 0xF0, //0
        0x20, 0x60, 0x20, 0x20, 0x70, //1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
        0x90, 0x90, 0xF0, 0x10, 0x10, //4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
        0xF0, 0x10, 0x20, 0x40, 0x40, //7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
        0xF0, 0x90, 0xF0, 0x90, 0x90, //A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
        0xF0, 0x80, 0x80, 0x80, 0xF0, //C
        0xE0, 0x90, 0x90, 0x90, 0xE0, //D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
        0xF0, 0x80, 0xF0, 0x80, 0x80  //F
    };
};

};
};

#endif