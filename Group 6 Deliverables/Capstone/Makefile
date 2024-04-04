## Compiler
CC = gcc

# Compiler flags (include directories)
CFLAGS = -g -Wformat=0 -I/home/student/Desktop/Capstone/include -I/usr/include/mysql

# Libraries
LIBS = -lm -lmysqlclient 

# Directories
OBJDIR = /home/student/Desktop/Capstone/obj
SRCDIR = /home/student/Desktop/Capstone/src
BINDIR = /home/student/Desktop/Capstone/bin
SCRIPTDIR = /home/student/Desktop/Capstone/scripts.sh

# Source files
SRCFILES := $(wildcard $(SRCDIR)/*.c)

# Object files
OBJFILES := $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCFILES))

# Bash scripts
SCRIPTS := $(wildcard $(SCRIPTDIR)/*.sh)

# Target executable name
TARGET = /home/student/Desktop/Capstone/program-a

# Default target
all: $(TARGET) scripts

# Create the directories if they don't exist
$(shell mkdir -p $(OBJDIR) $(BINDIR))

# Rule to make object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# Linking the target executable
$(TARGET): $(OBJFILES)
	$(CC) $^ $(LIBS) -o $@

# Copy scripts and set execute permissions
scripts: $(SCRIPTS)
	cp $^ $(BINDIR)
	chmod +x $(BINDIR)/*.sh

# Clean target
clean:
	rm -rf $(OBJDIR)/*.o $(TARGET) $(BINDIR)/*


