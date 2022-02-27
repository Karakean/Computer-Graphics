CC = g++ -Wall -pedantic -ansi

all:
	$(CC) -Wno-long-long source/Linux_OpenGL.cpp source/render.cpp -o Linux_OpenGL -lX11 -lGL -lGLU -lXxf86vm

clean:
	@echo Cleaning up...
	@rm Linux_OpenGL
	@echo Done.
