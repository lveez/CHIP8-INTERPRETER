SRCFILES := ${wildcard src/*.cpp src/*/*.cpp}			# get all files

TESTFILES = ${wildcard src/tests/*.cpp}					# get the test files

MAINFILES:= $(filter-out $(TESTFILES), $(SRCFILES))		# filter out test files to build main program
TESTFILES := ${filter-out src/main.cpp, $(SRCFILES)}	# filter out main.cpp to build test program

all:
	g++ -mwindows $(MAINFILES) -o./bin/chip8

test:
	g++ -I./include $(TESTFILES) -o./bin/chip8-tests