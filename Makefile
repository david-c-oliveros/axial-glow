CC = g++
CXXFLAGS = -lX11 -lGL -lpthread -lpng -lstdc++fs -std=c++17 -Wfatal-errors -w
app: Axial.cpp  ; $(CC) $(CXXFLAGS)  -o app Axial.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp\
		World.cpp Loot.cpp Counter.cpp Enemy.cpp ProcGen.cpp


clean:  ; -rm $(objects) app error.txt log

debug:  ; $(CC) $(CXXFLAGS) -o app -g Axial.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp\
		World.cpp Loot.cpp Counter.cpp Enemy.cpp ProcGen.cpp


log:  ; $(CC) $(CXXFLAGS) 2> error.txt -o app Axial.cpp Entity.cpp Player.cpp olcPixelGameEngine.cpp\
		World.cpp Loot.cpp Counter.cpp Enemy.cpp ProcGen.cpp
