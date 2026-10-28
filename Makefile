CXX      = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -O2
TARGET   = explorer
OBJS     = main.o explorer.o viewer.o utils.o

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)
