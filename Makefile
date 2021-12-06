mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
OS := $(shell uname)

EXECUTABLE=apgn
INSTALLPATH=/usr/local/bin

all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse

ifeq ($(OS), Linux)
	chmod +x bin/engines/stockfish11_x64
	g++ -static-libgcc -static-libstdc++ -std=c++17 main.cpp -o ${EXECUTABLE} -O3
# g++ -std=c++17 main.cpp -o apgn -DDEBUG -fsanitize=address -g # For Debugging
else
	chmod +x bin/engines/stockfish11_x64.exe
	g++ -static-libgcc -static-libstdc++ -std=c++17 main.cpp -o ${EXECUTABLE}.exe -O3
endif

test:
ifeq ($(OS), Linux)
	./${EXECUTABLE} ./pgn_samples/first.pgn W
else
	${EXECUTABLE}.exe pgn_samples/first.pgn W
endif

test_clean:
ifeq ($(OS), Linux)
	rm ./pgn_samples/firstAnalyzed.pgn
else
	rm pgn_samples/firstAnalyzed.pgn
endif

install:
ifeq ($(OS), Linux)
	@ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))${EXECUTABLE} ${INSTALLPATH}
else
	@echo "Set It Manually for now"
# SETX /M PATH "%PATH%;$(dir $(abspath $(lastword $(MAKEFILE_LIST))))"
endif

uninstall:
ifeq ($(OS), Linux)
	@rm ${INSTALLPATH}/${EXECUTABLE}
	@rm ./bin/analyse ./bin/pgn-extract ./${EXECUTABLE}
else
	@rm ./bin/analyse.exe ./bin/pgn-extract.exe ./${EXECUTABLE}.exe
endif

clean:
	@echo "removing pgn-extract object files"
	@$(MAKE) -C dependencies/pgn-extract clean
	@echo "removing uci-analyse object files"
	@$(MAKE) -C dependencies/uci-analyser clean
	@echo "removing analyse-pgn object files"