./code/MScompiler programs/$1.ms $1.o #>> /dev/null
#./program ../programs/program2.ms prog2.o
clang++ $1.o -o $1 >> /dev/null
echo
echo "Executing..."
echo
./$1