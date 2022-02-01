CC = g++
CXXFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17
ll: Catacombs.cpp  ; $(CC) $(CXXFLAGS) -o app Catacombs.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp

clean:  ; -rm $(objects) app error.txt

debug:  ; $(CC) $(CXXFLAGS) -o app -g Catacombs.cpp olcPixelGameEngine.cpp


log:  ; $(CC) $(CXXFLAGS) 2> error.txt -o app Catacombs.cpp olcPixelGameEngine.cpp
