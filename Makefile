game:
	clang++ -v -std=c++17 src/*.cpp -o Shotgun\ Billy  -I include -L lib -l SDL2-2.0.0 -l SDL2_image-2.0.0 -l SDL2_mixer
	./Shotgun\ Billy

