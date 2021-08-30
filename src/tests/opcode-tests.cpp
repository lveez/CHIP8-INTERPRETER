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

TEST_CASE("6xnn set", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0x61;
    cpu.ram[0x0201] = 0x12;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[1] == 0x12);
}

TEST_CASE("7xnn add", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.registers.variable[1] = 0x02;
    cpu.ram[0x0200] = 0x71;
    cpu.ram[0x0201] = 0x12;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[1] == 0x14);
}

TEST_CASE("8xy0 set reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.registers.variable[2] = (rand() % 255) + 1;
    cpu.ram[0x0200] = 0x81;
    cpu.ram[0x0201] = 0x20;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[2] != 0);
    REQUIRE(cpu.registers.variable[1] == cpu.registers.variable[2]);
}

TEST_CASE("8xy1 or reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    chip8::cpu::byte x = (rand() % 256);
    cpu.registers.variable[2] = (rand() % 256);
    cpu.registers.variable[1] = x;
    cpu.ram[0x0200] = 0x81;
    cpu.ram[0x0201] = 0x21;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[1] == (x | cpu.registers.variable[2]));
}

TEST_CASE("8xy2 and reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    chip8::cpu::byte x = (rand() % 256);
    cpu.registers.variable[2] = (rand() % 256);
    cpu.registers.variable[1] = x;
    cpu.ram[0x0200] = 0x81;
    cpu.ram[0x0201] = 0x22;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[1] == (x & cpu.registers.variable[2]));
}

TEST_CASE("8xy3 xor reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    chip8::cpu::byte x = (rand() % 256);
    cpu.registers.variable[2] = (rand() % 256);
    cpu.registers.variable[1] = x;
    cpu.ram[0x0200] = 0x81;
    cpu.ram[0x0201] = 0x23;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[1] == (x ^ cpu.registers.variable[2]));
}

TEST_CASE("8xy4 add reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("add")
    {
        cpu.registers.variable[1] = 0x11;
        cpu.registers.variable[2] = 0x05;
        cpu.ram[0x0200] = 0x81;
        cpu.ram[0x0201] = 0x24;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 0x16);
    }

    SECTION("carry")
    {
        cpu.registers.variable[1] = 0xff;
        cpu.registers.variable[2] = 0x05;
        cpu.ram[0x0200] = 0x81;
        cpu.ram[0x0201] = 0x24;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 0x04);
        REQUIRE(cpu.registers.variable[0x0f] == 1);
    }
}

TEST_CASE("8xy5 sub reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("sub")
    {
        cpu.registers.variable[1] = 0x16;
        cpu.registers.variable[2] = 0x05;
        cpu.ram[0x0200] = 0x81;
        cpu.ram[0x0201] = 0x25;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 0x11);
        REQUIRE(cpu.registers.variable[0x0f] == 1);
    }

    SECTION("carry")
    {
        cpu.registers.variable[1] = 0x01;
        cpu.registers.variable[2] = 0x02;
        cpu.ram[0x0200] = 0x81;
        cpu.ram[0x0201] = 0x25;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 0xff);
        REQUIRE(cpu.registers.variable[0x0f] == 0);
    }
}

TEST_CASE("8xy7 sub reg rev", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    SECTION("sub")
    {
        cpu.registers.variable[1] = 0x05;
        cpu.registers.variable[2] = 0x16;
        cpu.ram[0x0200] = 0x81;
        cpu.ram[0x0201] = 0x27;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 0x11);
        REQUIRE(cpu.registers.variable[0x0f] == 1);
    }

    SECTION("carry")
    {
        cpu.registers.variable[1] = 0x02;
        cpu.registers.variable[2] = 0x01;
        cpu.ram[0x0200] = 0x81;
        cpu.ram[0x0201] = 0x27;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 0xff);
        REQUIRE(cpu.registers.variable[0x0f] == 0);
    }
}

TEST_CASE("8xy6 shr", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0x81;
    cpu.ram[0x0201] = 0x26;

    SECTION("basic")
    {
        cpu.registers.variable[1] = 4;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 2);
    }

    SECTION("super_chip")
    {
        cpu.super_chip = true;
        cpu.registers.variable[2] = 4;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 2);
    }

    SECTION("flag")
    {
        cpu.registers.variable[1] = 3;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 1);
        REQUIRE(cpu.registers.variable[0x0f] == 1);
    }
}

TEST_CASE("8xye shr", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0x81;
    cpu.ram[0x0201] = 0x2e;

    SECTION("basic")
    {
        cpu.registers.variable[1] = 4;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 8);
    }

    SECTION("super_chip")
    {
        cpu.super_chip = true;
        cpu.registers.variable[2] = 4;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 8);
    }

    SECTION("flag")
    {
        cpu.registers.variable[1] = 129;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[1] == 2);
        REQUIRE(cpu.registers.variable[0x0f] == 1);
    }
}

#pragma endregion
