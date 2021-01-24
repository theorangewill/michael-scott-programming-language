compile = clang++
CXXFLAGS = $(shell llvm-config --cxxflags)
LDFLAGS = $(shell llvm-config --ldflags --system-libs --libs core)

program: llvm.s michaelscottgrammar.s parser.s main.s 
	$(compile) -o program $(LDFLAGS) main.s parser.s michaelscottgrammar.s llvm.s 

main.s: main.cpp main.h parser.h
	$(compile) -O3 -S $< $(CXXFLAGS) -o main.s
	
parser.s: parser.cpp lexic.h michaelscottgrammar.h
	$(compile) -O3 -S $< $(CXXFLAGS) -o parser.s

michaelscottgrammar.s: michaelscottgrammar.cpp michaelscottgrammar.h ast.h llvm.h
	$(compile) -O3 -S $< $(CXXFLAGS) -o michaelscottgrammar.s 

llvm.s: llvm.cpp llvm.h
	$(compile) -O3 -S $< $(CXXFLAGS) -o llvm.s

#program: main.cpp main.h michaelscottgrammar.h parser.h lexic.h ast.h llvm.h 
#	$(compile) -v -g llvm.cpp michaelscottgrammar.cpp parser.cpp  main.cpp  $(llvmflags) -o program

clean:
	rm -rf *.o
	rm -rf *.s
	rm -rf program