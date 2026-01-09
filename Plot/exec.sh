clang -c *.c
clang *.o -o main
rm *.o
./main
python3 plot.py

