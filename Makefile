CC = gcc
CXX = g++
CFLAGS = -O2 -ffast-math -fomit-frame-pointer -Irubber1.0
CXXFLAGS = -O2 -ffast-math -fomit-frame-pointer -Irubber1.0 -std=c++0x
LIBS = /usr/lib/$(shell dpkg-architecture -qDEB_HOST_MULTIARCH)/libbass.so -lglut -lGLU -lGL -lm

OBJ = test3d.o Kewlmation.o \
      rubber1.0/Rubber.o rubber1.0/Shader.o rubber1.0/Texture.o \
      rubber1.0/Model.o rubber1.0/Camera.o rubber1.0/Datafile.o \
      rubber1.0/Fbo.o rubber1.0/BassRocket.o \
      rocket/sync/data.o rocket/sync/device.o rocket/sync/track.o

onefinger: $(OBJ)
	$(CXX) -o $@ $(OBJ) $(LIBS)
	strip $@

clean:
	-rm $(OBJ) onefinger
