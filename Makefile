CC = g++
CXXFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Wfatal-errors -w
app: Catacombs.cpp  ; $(CC) $(CXXFLAGS)  -o app Catacombs.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp\
		World.cpp ProcGen.cpp


clean:  ; -rm $(objects) app error.txt log

debug:  ; $(CC) $(CXXFLAGS) -o app -g Catacombs.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp\
		World.cpp ProcGen.cpp


log:  ; $(CC) $(CXXFLAGS) 2> error.txt -o app Catacombs.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp\
		World.cpp ProcGen.cpp
