gcc -Wall -std=c17 -o program program.c -lm
./program < test0.txt > output0.txt
./program < test1.txt > output1.txt
diff output0.txt test0-output.txt
diff output1.txt test1-output.txt