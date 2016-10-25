gcc -std=gnu11 -c *.c ; nm -nS *.o > output.txt ; gcc *.o -o code -lm ; printf '\nFinal Table:\n' >> output.txt ; nm -nS code >> output.txt
