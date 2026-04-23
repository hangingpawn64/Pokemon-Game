CXX = g++
CXXFLAGS = -I./src -std=c++17 -Wall

# Default build target
TARGET = game
OVERWORLD_TARGET = overworld
SFML_TARGET = sfml_stage2

# Main game build
SRCS = src/main.cpp \
	src/Move.cpp \
	src/Pokemon.cpp \
	src/Battle.cpp \
	src/pokemons/Squirtle.cpp \
	src/pokemons/Charmander.cpp \
	src/pokemons/Bulbasaur.cpp \
	src/pokemons/Pikachu.cpp

# Sprite loader variant
SFML_SRCS = src/sfml_stage2.cpp \
	src/Pokemon.cpp \
	src/Move.cpp \
	src/pokemons/Pikachu.cpp \
	src/pokemons/Bulbasaur.cpp \
	src/pokemons/Charmander.cpp \
	src/pokemons/Squirtle.cpp

# Overworld map
OVERWORLD_SRCS = src/overworld.cpp \
	src/sfml_stage2.cpp \
	src/Pokemon.cpp \
	src/Move.cpp \
	src/pokemons/Pikachu.cpp \
	src/pokemons/Bulbasaur.cpp \
	src/pokemons/Charmander.cpp \
	src/pokemons/Squirtle.cpp

# SFML and optional curl
SFML_LIBS = -lsfml-graphics -lsfml-window -lsfml-system -lcurl

# Default rule
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $(TARGET) $(SFML_LIBS)

$(SFML_TARGET): $(SFML_SRCS)
	$(CXX) $(CXXFLAGS) $(SFML_SRCS) -o $(SFML_TARGET) $(SFML_LIBS)

$(OVERWORLD_TARGET): $(OVERWORLD_SRCS)
	$(CXX) $(CXXFLAGS) $(OVERWORLD_SRCS) -o $(OVERWORLD_TARGET) $(SFML_LIBS)

clean:
	rm -f $(TARGET) $(SFML_TARGET) $(OVERWORLD_TARGET)