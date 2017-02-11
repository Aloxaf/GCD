%.o : %.c
	${CC} -o $@ -c $^
GCD.dll :  GCDver.o GCDimage.o GCDmouse.o GCDGetConsoleInput.o
	${CC} -shared -o GCD.dll $^ -lgdi32 -lmsimg32 -luser32 -lkernel32 -lgdiplus