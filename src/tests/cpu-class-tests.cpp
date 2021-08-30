#include <catch2/catch.hpp>
#include <fstream>
#include <iostream>

#define private public      // this is for testing only 
#include "../cpu/cpu.hpp"

#pragma region init

TEST_CASE("cpu ram init", "[cpu-class][init]")
{
    chip8::cpu::CPU cpu;

    SECTION("ram is set to 0")
    {
        for (int i = 80; i < cpu.ram.size(); i++)
            REQUIRE(cpu.ram[i] == 0);
    }

    SECTION("fontset is loaded")
    {
        for (int i = 0; i < cpu.fontset.size(); i++)
            REQUIRE(cpu.ram[i] == cpu.fontset[i]);
    }
}

TEST_CASE("cpu vram init", "[cpu-class][init]")
{
    chip8::cpu::CPU cpu;
    
    for (auto i : cpu.vram)
        REQUIRE(i == 0);
}

TEST_CASE("cpu stack init", "[cpu-class][init]")
{
    chip8::cpu::CPU cpu;

    for (auto i : cpu.stack)
        REQUIRE(i == 0);
}

TEST_CASE("cpu keyboard init", "[cpu-class][init]")
{
    chip8::cpu::CPU cpu;

    for (auto i : cpu.keyboard)
        REQUIRE(i == 0);
}

TEST_CASE("cpu registers init", "[cpu-class][init]")
{
    chip8::cpu::CPU cpu;

    REQUIRE(cpu.registers.pc == 0x0200);
    REQUIRE(cpu.registers.delay_timer == 0);
    REQUIRE(cpu.registers.index == 0);
    REQUIRE(cpu.registers.sound_timer == 0);
    REQUIRE(cpu.registers.stack_pointer == 0);
    for (auto i : cpu.registers.variable)
        REQUIRE(i == 0);
}

TEST_CASE("cpu class data init", "[cpu-class][init]")
{
    chip8::cpu::CPU cpu;

    REQUIRE(cpu.current_opcode == 0);
    REQUIRE(cpu.size_of_rom == 0);
}

#pragma endregion

#pragma region func

TEST_CASE("cpu fetch", "[cpu-class][func]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0x12;
    cpu.ram[0x0201] = 0x34;
    cpu.Fetch();

    REQUIRE(cpu.registers.pc == 0x0202);
    REQUIRE(cpu.current_opcode == 0x1234);
}

TEST_CASE("cpu loadrom", "[cpu-class][func]")
{
    chip8::cpu::CPU cpu;

    const std::array<chip8::cpu::byte, 0x84> rom =
    {
        0x00, 0xe0, 0xa2, 0x2a, 0x60, 0x0c, 0x61, 0x08, 0xd0, 0x1f, 0x70, 0x09, 0xa2, 0x39, 0xd0, 0x1f,
        0xa2, 0x48, 0x70, 0x08, 0xd0, 0x1f, 0x70, 0x04, 0xa2, 0x57, 0xd0, 0x1f, 0x70, 0x08, 0xa2, 0x66,
        0xD0, 0x1F, 0x70, 0x08, 0xA2, 0x75, 0xD0, 0x1F, 0x12, 0x28, 0xFF, 0x00, 0xFF, 0x00, 0x3C, 0x00, 
        0x3C, 0x00, 0x3C, 0x00, 0x3C, 0x00, 0xFF, 0x00, 0xFF, 0xFF, 0x00, 0xFF, 0x00, 0x38, 0x00, 0x3F,
        0x00, 0x3F, 0x00, 0x38, 0x00, 0xFF, 0x00, 0xFF, 0x80, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x80, 0x00,
        0x80, 0x00, 0xE0, 0x00, 0xE0, 0x00, 0x80, 0xF8, 0x00, 0xFC, 0x00, 0x3E, 0x00, 0x3F, 0x00, 0x3B,
        0x00, 0x39, 0x00, 0xF8, 0x00, 0xF8, 0x03, 0x00, 0x07, 0x00, 0x0F, 0x00, 0xBF, 0x00, 0xFB, 0x00,
        0xF3, 0x00, 0xE3, 0x00, 0x43, 0xE0, 0x00, 0xE0, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80, 0x00, 0x80,
        0x00, 0xE0, 0x00, 0xE0
    };

    cpu.LoadROM("roms/IBM Logo.ch8");

    for (int i = 0; i < 0x84; i++)
    {
        INFO(i);
        REQUIRE(int(cpu.ram[0x0200 + i]) == int(rom[i]));
    }
}

#pragma endregion

#pragma region op

TEST_CASE("00e0 clear screen", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;
    std::fill(cpu.vram.begin(), cpu.vram.end(), 1);

    SECTION("set all pixels to on")
    {
        for (auto i : cpu.vram)
            REQUIRE_FALSE(i == 0);
    }

    SECTION("check clear screen sets all pixels to off")
    {
        cpu.ram[0x0200] = 0x00;
        cpu.ram[0x0201] = 0xe0;
        cpu.Cycle();

        REQUIRE(cpu.current_opcode == 0x00e0);

        for (auto i : cpu.vram)
            REQUIRE(i == 0);
    }
}

TEST_CASE("00ee return", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;
    // fake push
    cpu.stack[0] = 0x333;
    cpu.registers.stack_pointer++;

    cpu.ram[0x0200] = 0x00;
    cpu.ram[0x0201] = 0xee;
    cpu.Cycle();
    
    REQUIRE(cpu.current_opcode == 0x00ee);

    REQUIRE(cpu.registers.stack_pointer == 0);
    REQUIRE(cpu.registers.pc == 0x333);
}

#pragma endregion
