#-fprofile-generate -fprofile-use  
CC=g++
ENGINE=../gl_engine
CFLAGS=-c  -I"./includes" -I"./glm" -I"../gl_engine" -O3  -std=gnu++14 -D _GLFW_WIN32
ifeq ($(OS),Windows_NT)
	DELOPERATOR= del /q
	LDFLAGS=  -lglew32 -lSOIL -l:../libglfw3.a libirrKlang.a -lopengl32 -static-libgcc -static-libstdc++  -l:libgdi32.a -L"./gl"
else
	DELOPERATOR= rm
	LDFLAGS=  -lGLEW -lSOIL -lGL `pkg-config --cflags glfw3` `pkg-config --static --libs glfw3`
endif
ENGINE_SOURCES= $(wildcard $(ENGINE)/*.cpp)
APP_SOURCES= $(wildcard *.cpp)
SOURCES= $(ENGINE_SOURCES) $(APP_SOURCES)
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=game


all: build

build:	$(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) game.res
	$(CC)  $(OBJECTS) -o $@ $(LDFLAGS) game.res

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
game.res:
	windres game.rc -O coff -o game.res

rebuild: clean build

clean: clean_engine clean_app

clean_app: 
	$(DELOPERATOR) *.o

ifeq ($(OS),Windows_NT)
clean_engine: 
	$(DELOPERATOR) ..\gl_engine\*.o
else
clean_engine: 
	$(DELOPERATOR) ../gl_engine/*.o
endif