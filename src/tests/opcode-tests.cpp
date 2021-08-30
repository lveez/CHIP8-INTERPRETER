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

TEST_CASE("annn ld i", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xa4;
    cpu.ram[0x0201] = 0xbc;
    cpu.Cycle();

    REQUIRE(cpu.registers.index == 0x04bc);
}

TEST_CASE("bnnn jump off", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;
    cpu.ram[0x0200] = 0xb4;
    cpu.ram[0x0201] = 0xc3;
    
    SECTION("basic")
    {
        cpu.registers.variable[0] = 0x05;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x04c8);
    }

    SECTION("super_chip")
    {
        cpu.registers.variable[4] = 0x02;
        cpu.super_chip = true;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x04c5);
    }
}

TEST_CASE("ex9e skip key", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xe1;
    cpu.ram[0x0201] = 0x9e;
    cpu.registers.variable[1] = 0;

    SECTION("pressed")
    {
        cpu.keyboard[0] = 1;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x204);
    }

    SECTION("not pressed")
    {
        cpu.keyboard[0] = 0;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x202);
    }
}

TEST_CASE("exa1 skip not key", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xe1;
    cpu.ram[0x0201] = 0xa1;
    cpu.registers.variable[1] = 0;

    SECTION("pressed")
    {
        cpu.keyboard[0] = 1;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x202);
    }

    SECTION("not pressed")
    {
        cpu.keyboard[0] = 0;
        cpu.Cycle();

        REQUIRE(cpu.registers.pc == 0x204);
    }
}

TEST_CASE("fx07 v[x] = del", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf1;
    cpu.ram[0x0201] = 0x07;
    cpu.registers.delay_timer = 0x12;
    cpu.Cycle();

    REQUIRE(cpu.registers.variable[1] == 0x12);
}

TEST_CASE("fx15 del = v[x]", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf1;
    cpu.ram[0x0201] = 0x15;
    cpu.registers.variable[1] = 0x12;
    cpu.Cycle();

    REQUIRE(cpu.registers.delay_timer == 0x12);
}

TEST_CASE("fx18 snd = v[x]", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf1;
    cpu.ram[0x0201] = 0x18;
    cpu.registers.variable[1] = 0x12;
    cpu.Cycle();

    REQUIRE(cpu.registers.sound_timer == 0x12);
}

TEST_CASE("fx1e i += v[x]", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf1;
    cpu.ram[0x0201] = 0x1e;
    cpu.registers.variable[1] = 0x05;    

    SECTION("no carry")
    {   
        cpu.registers.index = 0x0300;
        cpu.Cycle();

        REQUIRE(cpu.registers.index == 0x305);
        REQUIRE(cpu.registers.variable[0x0f] == 0);
    }

    SECTION("carry")
    {
        cpu.registers.index = 0x0fff;
        cpu.Cycle();

        REQUIRE(cpu.registers.index == 0x1004);
        REQUIRE(cpu.registers.variable[0x0f] == 1);
    }
}

TEST_CASE("fx29 load char", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf1;
    cpu.ram[0x0201] = 0x29;
    cpu.registers.variable[1] = 5;
    cpu.Cycle();

    REQUIRE(cpu.registers.index == 25);
}

TEST_CASE("fx33 bcd", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf1;
    cpu.ram[0x0201] = 0x33;
    cpu.registers.index = 0x300;
    
    SECTION("1 dig")
    {
        cpu.registers.variable[1] = 4;
        cpu.Cycle();

        REQUIRE(cpu.ram[cpu.registers.index] == 4);
        REQUIRE(cpu.ram[cpu.registers.index + 1] == 0);
        REQUIRE(cpu.ram[cpu.registers.index + 2] == 0);
    }

    SECTION("2 dig")
    {
        cpu.registers.variable[1] = 78;
        cpu.Cycle();

        REQUIRE(cpu.ram[cpu.registers.index] == 7);
        REQUIRE(cpu.ram[cpu.registers.index + 1] == 8);
        REQUIRE(cpu.ram[cpu.registers.index + 2] == 0);
    }

    SECTION("3 dig")
    {
        cpu.registers.variable[1] = 126;
        cpu.Cycle();

        REQUIRE(cpu.ram[cpu.registers.index] == 1);
        REQUIRE(cpu.ram[cpu.registers.index + 1] == 2);
        REQUIRE(cpu.ram[cpu.registers.index + 2] == 6);
    }
}

TEST_CASE("fx55 ld to ram", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf3;
    cpu.ram[0x0201] = 0x55;
    cpu.registers.index = 0x0300;
    cpu.registers.variable[0] = 0x15;
    cpu.registers.variable[1] = 0x17;
    cpu.registers.variable[2] = 0x93;
    cpu.registers.variable[3] = 0x3a;

    SECTION("new")
    {
        cpu.new_store_load = true;
        cpu.Cycle();

        REQUIRE(cpu.ram[0x0300] == 0x15);
        REQUIRE(cpu.ram[0x0301] == 0x17);
        REQUIRE(cpu.ram[0x0302] == 0x93);
        REQUIRE(cpu.ram[0x0303] == 0x3a);
        REQUIRE(cpu.registers.index == 0x0300);
    }

    SECTION("old")
    {
        cpu.new_store_load = false;
        cpu.Cycle();

        REQUIRE(cpu.ram[0x0300] == 0x15);
        REQUIRE(cpu.ram[0x0301] == 0x17);
        REQUIRE(cpu.ram[0x0302] == 0x93);
        REQUIRE(cpu.ram[0x0303] == 0x3a);
        REQUIRE(cpu.registers.index == 0x0304);
    }
}

TEST_CASE("fx65 ld to reg", "[cpu-class][op]")
{
    chip8::cpu::CPU cpu;

    cpu.ram[0x0200] = 0xf3;
    cpu.ram[0x0201] = 0x65;
    cpu.registers.index = 0x0300;
    cpu.ram[0x0300] = 0x15;
    cpu.ram[0x0301] = 0x17;
    cpu.ram[0x0302] = 0x93;
    cpu.ram[0x0303] = 0x3a;

    SECTION("new")
    {
        cpu.new_store_load = true;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[0] == 0x15);
        REQUIRE(cpu.registers.variable[1] == 0x17);
        REQUIRE(cpu.registers.variable[2] == 0x93);
        REQUIRE(cpu.registers.variable[3] == 0x3a);
        REQUIRE(cpu.registers.index == 0x0300);
    }

    SECTION("old")
    {
        cpu.new_store_load = false;
        cpu.Cycle();

        REQUIRE(cpu.registers.variable[0] == 0x15);
        REQUIRE(cpu.registers.variable[1] == 0x17);
        REQUIRE(cpu.registers.variable[2] == 0x93);
        REQUIRE(cpu.registers.variable[3] == 0x3a);
        REQUIRE(cpu.registers.index == 0x0304);
    }
}

#pragma endregion
