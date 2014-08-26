#$Id: Makefile,v 1.6 2009-12-09 10:27:51-08 - - $
# This sample Makefile allows you to make an OpenGL application
#   whose source is exactly one .c file.
#
#
# To use this Makefile, you type:
#
#        make xxxx
#                  
# where
#       xxxx.c is the name of the file you wish to compile 
#       
# A binary named xxxx will be produced
# Libraries are assumed to be in the default search paths
# as are any required include files
#
# For linux replace the next line with
# CC =  gcc
# and you may have to add -L/usr/X11R6/lib to LDLIBS

GPP = g++ 

LDLIBS = -ggdb -I/usr/local/glut-3.7/include -L/usr/local/glut-3.7/lib/glut -lglut -lGL -lGLU -lXmu -lX11 -lm

.cpp:
	$(GPP)  $@.cpp $(LDLIBS) -o $@

all:
	gmake 3Dmodel

save:
	cid 3Dmodel.cpp 3Dmodel.h Makefile

run:
	3Dmodel enterprise.coor enterprise.poly

runall:
	3Dmodel enterprise.coor enterprise.poly woman.coor woman.poly

open:
	emacs 3Dmodel.cpp 3Dmodel.h &

