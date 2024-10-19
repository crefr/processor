BUILD = RELEASE

ifeq ($(BUILD),WIN)
	CFLAGS = $(CFLAGS_WINDOWS)
else ifeq ($(BUILD),LINUX)
	CFLAGS = $(CFLAGS_LINUX)
else ifeq ($(BUILD),RELEASE)
	CFLAGS = $(CFLAGS_RELEASE)
endif

all: asm spu

asm:
	cd assembler/ && make BUILD=$(BUILD) && cd ..
spu:
	cd processor/ && make BUILD=$(BUILD) && cd ..

clean:
	cd assembler/ && make clean && cd ..
	cd processor/ && make clean && cd ..
