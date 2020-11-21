# Makefile Generique
 
EXT = c
CXX = gcc

EXEC = websnarf

CXXFLAGS = -Wall -W -pedantic 
LDFLAGS = 


OBJDIR = obj
SRC = $(wildcard *.$(EXT))
OBJ = $(SRC:.$(EXT)=.o)
OBJ := $(addprefix $(OBJDIR)/, $(OBJ))
 
all: objDir $(EXEC) clean
 
$(EXEC): $(OBJ)
	@$(CXX) -o $@ $^ $(LDFLAGS)
 
$(OBJDIR)/%.o: %.$(EXT)
	@$(CXX) -o $@ -c $< $(CXXFLAGS)
 
clean:
	@rm -rf $(OBJDIR)/*.o

objDir:
	mkdir -p $(OBJDIR)