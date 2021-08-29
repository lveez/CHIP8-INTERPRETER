SRCFILES := ${wildcard src/*.cpp src/*/*.cpp}				# get all files

TESTFILES = ${wildcard src/tests/*.cpp src/tests/*/*.cpp}	# get the test files

MAINFILES:= $(filter-out $(TESTFILES), $(SRCFILES))			# filter out test files to build main program
TESTFILES := ${filter-out src/main.cpp, $(SRCFILES)}		# filter out main.cpp to build test program

all:
	g++ -mwindows -mconsole -ftime-report $(MAINFILES) -o./bin/chip8

test:
	g++ -I./include -ftime-report $(TESTFILES) -o./bin/chip8-tests