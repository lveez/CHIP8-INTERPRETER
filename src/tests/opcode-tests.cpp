#include <catch2/catch.hpp>

#include <iostream>

#define private public      // this is for testing only 
#include "../cpu/cpu.hpp"

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

TEST_CASE("1nnn jump", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x200] = 0x15;
    cpu.ram[0x201] = 0x11;
    cpu.Cycle();

    REQUIRE(cpu.current_opcode == 0x1511);
    REQUIRE(cpu.registers.pc == 0x0511);
}

TEST_CASE("2nnn call", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x200] = 0x25;
    cpu.ram[0x201] = 0x11;
    cpu.Cycle();

    REQUIRE(cpu.current_opcode == 0x2511);
    REQUIRE(cpu.registers.pc == 0x0511);
    REQUIRE(cpu.registers.stack_pointer == 1);
    REQUIRE(cpu.stack[0] == 0x0202);
}

TEST_CASE("3xnn skip if equal const", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("equal")
    {
        cpu.ram[0x0200] = 0x32;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x10;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0204);
    }

    SECTION("not equal")
    {
        cpu.ram[0x0200] = 0x32;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x11;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0202);
    }
}

TEST_CASE("4xnn skip if not equal const", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("equal")
    {
        cpu.ram[0x0200] = 0x42;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x10;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0202);
    }

    SECTION("not equal")
    {
        cpu.ram[0x0200] = 0x42;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x11;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0204);
    }
}

TEST_CASE("5xy0 skip if equal reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("equal")
    {
        cpu.ram[0x0200] = 0x52;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x10;
        cpu.registers.variable[1] = 0x10;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0204);
    }

    SECTION("not equal")
    {
        cpu.ram[0x0200] = 0x52;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x10;
        cpu.registers.variable[1] = 0x11;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0202);
    }
}

TEST_CASE("9xy0 skip if not equal reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("equal")
    {
        cpu.ram[0x0200] = 0x92;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x10;
        cpu.registers.variable[1] = 0x10;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0202);
    }

    SECTION("not equal")
    {
        cpu.ram[0x0200] = 0x92;
        cpu.ram[0x0201] = 0x10;
        cpu.registers.variable[2] = 0x10;
        cpu.registers.variable[1] = 0x11;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x0204);
    }
}

#pragma endregion
