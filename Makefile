main: main.cpp sampler.cpp
	g++ main.cpp sampler.cpp -o main -std=c++11 -O2 -lpthread

clean:
	rm main
