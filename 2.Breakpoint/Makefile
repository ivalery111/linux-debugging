dynamic: for_trace_dyn main_dyn
static: for_trace_static main_static

for_trace_dyn: for_trace.c
	gcc -Wall -pedantic-errors --std=c99 for_trace.c -o trace_me_dyn.out

main_dyn: main.c
	gcc -Wall -pedantic-errors --std=c99 main.c -o trace_dyn.out

for_trace_static: for_trace.c
	gcc -Wall -pedantic-errors -static --std=c99 for_trace.c -o trace_me_static.out

main_static: main.c
	gcc -Wall -pedantic-errors -static --std=c99 main.c -o trace_static.out


run_dyn:
	./trace_dyn.out trace_me_dyn.out 

run_static:
	./trace_static.out trace_me_static.out 

clean:
	rm *.out

