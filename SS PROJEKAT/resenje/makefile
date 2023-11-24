	
all:
	bison -d misc/bison.y
	flex misc/flex.l	
	g++ -Iinc bison.tab.c lex.yy.c src/asembler.cpp src/mainAsembler.cpp -o asembler
	g++ -Iinc src/linker.cpp src/mainLinker.cpp -o linker
	g++ -Iinc src/emulator.cpp src/mainEmulator.cpp -o emulator

asembler:
	bison -d misc/bison.y\
	flex misc/flex.l \
	g++ -Iinc bison.tab.c lex.yy.c src/asembler.cpp src/mainAsembler.cpp -o asembler\

linker:
	g++ -Iinc src/linker.cpp src/mainLinker.cpp -o linker

emulator:
	g++ -Iinc src/emulator.cpp src/mainEmulator.cpp -o emulator

clean:
	rm -f asembler
	rm -f linker
	rm -f emulator
