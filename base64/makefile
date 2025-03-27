CXX = g++
CXXFLAGS = -Wall -std=c++20
app : base64.cpp
	$(CXX) $(CXXFLAGS) base64.cpp -o app
hello : hello.cpp
	$(CXX) $(CXXFLAGS) hello.cpp -o hello
run : app hello
	./app
clean : 
	rm app