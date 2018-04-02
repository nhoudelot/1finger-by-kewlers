#variables de compilation
SHELL = /bin/sh
CC = gcc
CXX = g++
CFLAGS += -O3 -ffast-math -flto -pipe -Irubber1.0 $(shell pkgconf --cflags glu)
CXXFLAGS += -O3 -ffast-math -flto -pipe -Irubber1.0 -std=c++11 $(shell pkgconf --cflags glu)
LDFLAGS += /usr/lib/$(shell dpkg-architecture -qDEB_HOST_MULTIARCH)/libbass.so -lglut $(shell pkgconf --libs glu) -lm

OBJ = test3d.o Kewlmation.o \
      rubber1.0/Rubber.o rubber1.0/Shader.o rubber1.0/Texture.o \
      rubber1.0/Model.o rubber1.0/Camera.o rubber1.0/Datafile.o \
      rubber1.0/Fbo.o rubber1.0/BassRocket.o \
      rocket/sync/data.o rocket/sync/device.o rocket/sync/track.o

TARGET=1finger-by-kewlers

RM_F = rm -f

ifneq (,$(filter parallel=%,$(DEB_BUILD_OPTIONS)))
 NUMJOBS = $(patsubst parallel=%,%,$(filter parallel=%,$(DEB_BUILD_OPTIONS)))
 MAKEFLAGS += -j$(NUMJOBS)
endif

export

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) -flto -o $@ $(OBJ) $(LDFLAGS)

clean:
	-@$(RM_F) $(OBJ) $(TARGET)
