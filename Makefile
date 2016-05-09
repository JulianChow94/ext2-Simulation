# The first rule is what is executed when you run make with no arguments
# In this case it will build two programs.
all: ext2_rm skeleton

# The executable program depends on the .o files.
# The compile line links all the .o files together to create the executable.
ext2_rm: ext2_rm.o
	gcc -Wall -g -o ext2_rm ext2_rm.o

skeleton: skeleton.o
	-gcc -Wall -g -o skeleton skeleton.o

# Each individual source file depends on itself plus the header file
# Each source file can be separately compiled to produce a .o file
# that will be linked together to create the executable.

ext2_rm.o: ext2_rm.c functions.c header.h
	gcc -Wall -g -c ext2_rm.c

skeleton.o: skeleton.c functions.c header.h
	gcc -Wall -g -c skeleton.c

clean:
	rm *.o