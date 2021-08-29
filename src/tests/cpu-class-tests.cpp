#include <catch2/catch.hpp>

#define private public      // this is for testing only 
#include "../cpu/cpu.hpp"

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

TEST_CASE("cpu fetch", "[cpu-class][func]")
{
    chip8::cpu::CPU cpu;

    *(chip8::cpu::word*)&cpu.ram[0x0200] = 0x1234;
    cpu.Fetch();
    REQUIRE(cpu.registers.pc == 0x0202);
    REQUIRE(cpu.current_opcode == 0x1234);
}