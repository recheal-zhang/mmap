SOURCEDIR := .
INCLUDEDIR := .
BUILD_DIR := .
CURRENTDIR := .
EXE := $(BUILD_DIR)/mmap
CC := g++
LIBS := pthread
DEBUGNAMES := DEBUG NONBLOCK ACK
SOURCES := $(wildcard $(SOURCEDIR)/*.cpp $(SOURCEDIR)/*.c)
OBJS := $(patsubst %.c, %.o, $(patsubst %.cpp, %.o, $(SOURCES))) 
RM := rm -rf
CXXFLAGS := -Wall -g -O -std=c++11 -DDEBUG 

start: prepare $(EXE)

prepare: 
	mkdir -p $(BUILD_DIR)

$(EXE): $(OBJS)
	$(CC) -o $@ $(OBJS) $(addprefix -l, $(LIBS)) $(addprefix -D, $(DEBUGNAMES))

#$(CURRENTDIR)/%.o: $(CURRENTDIR)/%.cpp
#	$(CC) $(addprefix -I, $(INCLUDEDIR)) $(addprefix -I, $(SOURCEDIR)) $(addprefix -I, $(CURRENTDIR)) -c $(CXXFLAGS) %< -o %@

$(SOURCEDIR)/%.o: $(SOURCEDIR)/%.cpp #$(INCLUDEDIR)/%.h
	$(CC) -c $(CXXFLAGS) $< -o $@ -I$(INCLUDEDIR) $(addprefix -D, $(DEBUGNAMES)) #-fpermissive

.PHONY: clean rebuild
clean:
	$(RM) $(EXE)
	$(RM) $(SOURCEDIR)/*.o
rebuild: clean start


