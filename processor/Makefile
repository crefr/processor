FILENAME = run.exe
OBJDIR = Obj/
SRCDIR = sources/
HEADDIR = headers/

CC = g++
BUILD  = RELEASE
# windows
CFLAGS_WINDOWS =-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline						\
		-Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default					\
		-Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy 				\
		-Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers 	\
		-Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing 		\
		-Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -Werror=vla -D_DEBUG -D_EJUDGE_CLIENT_SIDE					\
		-I./$(HEADDIR)

# linux
CFLAGS_LINUX = -I./$(HEADDIR) -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations 		\
		-Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported 			\
		-Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security 					\
		-Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual 			\
		-Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo 						\
		-Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods 					\
		-Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code 		\
		-Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing 		\
		-Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector 				\
		-fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE	\
		-Werror=vla -fsanitize=address,alignment,bool,bounds,enum,float-cast-overflow,float-divide-by-zero,integer-divide-by-zero,leak,nonnull-attribute,null,object-size,return,returns-nonnull-attribute,shift,signed-integer-overflow,undefined,unreachable,vla-bound,vptr

# release
CFLAGS_RELEASE = -I./$(HEADDIR) -O3

ifeq ($(BUILD),WIN)
	CFLAGS = $(CFLAGS_WINDOWS)
else ifeq ($(BUILD),LINUX)
	CFLAGS = $(CFLAGS_LINUX)
else ifeq ($(BUILD),RELEASE)
	CFLAGS = $(CFLAGS_RELEASE)
endif

$(FILENAME): $(OBJDIR)main.o $(OBJDIR)mystack.o $(OBJDIR)logger.o $(OBJDIR)hash.o $(OBJDIR)processor.o
	$(CC) $(CFLAGS) $^ -o $@

$(OBJDIR)main.o: $(SRCDIR)main.cpp $(HEADDIR)mystack.h $(HEADDIR)logger.h $(HEADDIR)processor.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)mystack.o: $(SRCDIR)mystack.cpp $(HEADDIR)mystack.h $(HEADDIR)logger.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)logger.o: $(SRCDIR)logger.cpp $(HEADDIR)logger.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)hash.o: $(SRCDIR)hash.cpp $(HEADDIR)hash.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@
$(OBJDIR)processor.o: $(SRCDIR)processor.cpp $(HEADDIR)processor.h $(HEADDIR)mystack.h
	mkdir -p $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@


clean:
	rm $(OBJDIR)*

run:
	./$(FILENAME)
