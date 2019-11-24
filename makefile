exec:
	gcc -c *.h -O2
	gcc -c *.c -O2
	gcc -o automaton *.o -O2
