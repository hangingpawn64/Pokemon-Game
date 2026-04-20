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

# SFML stage2 build (loads sprites from URLs via libcurl)
SFML_SRCS = sfml_stage2.cpp\
	SpriteLoader.cpp\
	Pokemon.cpp\
	Move.cpp\
	pokemons/Pikachu.cpp\
	pokemons/Bulbasaur.cpp\
	pokemons/Charmander.cpp\
	pokemons/Squirtle.cpp

SFML_TARGET = sfml_stage2
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lcurl

all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET)

$(SFML_TARGET): $(SFML_SRCS)
	$(CXX) $(CXXFLAGS) $(SFML_SRCS) -o $(SFML_TARGET) $(SFML_LIBS)

clean:
	rm -f $(TARGET) $(SFML_TARGET)