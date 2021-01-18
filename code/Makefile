compile = clang++
llvmflags = `llvm-config --cxxflags --ldflags --system-libs --libs core`


program: llvm.s michaelscottgrammar.s parser.s main.s 
	as -o program main.s parser.s michaelscottgrammar.s llvm.s

main.s: main.cpp main.h parser.h
	$(compile) -O3 -S $< -o main.s
	
parser.s: parser.cpp lexic.h michaelscottgrammar.h
	$(compile) -O3 -S $< -o parser.s

michaelscottgrammar.s: michaelscottgrammar.cpp michaelscottgrammar.h ast.h llvm.h
	$(compile) -O3 -S $< -o michaelscottgrammar.s

llvm.s: llvm.cpp llvm.h
	$(compile) -O3 -S $< $(llvmflags) -o llvm.s

#program: main.cpp main.h michaelscottgrammar.h parser.h lexic.h ast.h llvm.h 
#	$(compile) -v -g llvm.cpp michaelscottgrammar.cpp parser.cpp  main.cpp  $(llvmflags) -o program

clean:
	rm -rf *.o
	rm -rf *.s
	rm -rf program