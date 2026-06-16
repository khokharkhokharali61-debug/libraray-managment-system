CXX      = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET   = library_system
SRCDIR   = src
INCDIR   = include

SRCS = $(SRCDIR)/main.cpp       \
       $(SRCDIR)/Book.cpp        \
       $(SRCDIR)/User.cpp        \
       $(SRCDIR)/Member.cpp      \
       $(SRCDIR)/Admin.cpp       \
       $(SRCDIR)/Transaction.cpp \
       $(SRCDIR)/ReportManager.cpp \
       $(SRCDIR)/UI.cpp          \
       $(SRCDIR)/Library.cpp     \
       $(SRCDIR)/Library2.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

clean:
	rm -f $(SRCDIR)/*.o $(TARGET)

run: all
	./$(TARGET)

.PHONY: all clean run
