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
	cd assembler/ && make BUILD=$(BUILD)
spu:
	cd processor/ && make BUILD=$(BUILD)

clean:
	cd assembler/ && make clean
	cd processor/ && make clean
