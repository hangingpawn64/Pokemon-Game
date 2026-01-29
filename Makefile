CXX = g++ # this tell program to use g++ compiler
CXXFLAGS = -I. -std=c++17 -Wall # -I. means look for .h files || -std=c++17 tells to use c++17 feature || -Wall shows all warnings during compilation

SRCS = main.cpp\
	Move.cpp\
	Pokemon.cpp\
	Battle.cpp\
	pokemons/Squirtle.cpp\
	pokemons/Charmander.cpp\
	pokemons/Bulbasaur.cpp\
	pokemons/Pikachu.cpp

TARGET = game

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

clean:
	rm -f $(TARGET)