SRCFILES := ${wildcard src/*.cpp src/*/*.cpp}									# get all files

TESTFILES = ${wildcard src/tests/*.cpp src/tests/*/*.cpp}						# get the test files

MAINFILES:= $(filter-out $(TESTFILES), $(SRCFILES))								# filter out test files to build main program
TESTFILES := ${filter-out src/main.cpp, $(SRCFILES)}							# filter out main.cpp to build test program

QUICKCOMPILETESTFILES := $(filter-out src/tests/tests-main.cpp, $(TESTFILES))	# about 25% quicker compile

all:
	g++ -mwindows -mconsole $(MAINFILES) -o./bin/chip8

test:
	g++ -I./include tests-main.o $(QUICKCOMPILETESTFILES) -o./bin/chip8-tests