build:
	gcc voxel.c dos.c  `sdl2-config --libs --cflags` -lGLEW -lGL -lm -lpthread -o voxel
run:
	./voxel
