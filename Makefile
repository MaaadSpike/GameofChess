CXX      = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -MMD
LDLIBS   = -lX11
EXEC     = chess

# macOS: X11 comes from XQuartz
ifeq ($(shell uname -s),Darwin)
  CXXFLAGS += -I/opt/X11/include
  LDLIBS   := -L/opt/X11/lib $(LDLIBS)
endif

CORE    = board.o move.o piece.o king.o queen.o rook.o bishop.o knight.o pawn.o empty.o
AI      = algorithm.o level1.o level2.o level3.o level4.o openingbook.o
OBJECTS = main.o gamecontroller.o player.o human.o computer.o textdisplay.o \
          graphdisplay.o window.o scoreboard.o $(CORE) $(AI)
DEPENDS = $(OBJECTS:.o=.d) tests/perft.d

$(EXEC): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXEC) $(LDLIBS)

# Move-generator correctness test (perft) + opening book check
test: tests/perft
	./tests/perft

tests/perft: tests/perft.o $(CORE) openingbook.o
	$(CXX) $(CXXFLAGS) $^ -o $@

-include $(DEPENDS)

.PHONY: clean test
clean:
	rm -f $(OBJECTS) $(DEPENDS) $(EXEC) tests/perft tests/perft.o
