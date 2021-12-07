mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
OS := $(shell uname)

CC=g++
CXX_FLAGS=-std=c++17 -static-libgcc -static-libstdc++ -O3
ifeq ($(OS), Linux)
EXECUTABLE=apgn
else
EXECUTABLE=apgn.exe
endif
INSTALLPATH=/usr/local/bin

all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse

ifeq ($(OS), Linux)
	chmod +x bin/engines/stockfish11_x64
else
	chmod +x bin/engines/stockfish11_x64.exe
endif
	${CC} ${CXX_FLAGS} main.cpp -o ${EXECUTABLE}

test:
	./${EXECUTABLE} ./pgn_samples/first.pgn W

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
else
	@echo "uninstall make is not supported for windows"
endif

clean:
	@echo "removing pgn-extract object files"
	@$(MAKE) -C dependencies/pgn-extract clean
	@echo "removing uci-analyse object files"
	@$(MAKE) -C dependencies/uci-analyser clean
	@echo "removing analyse-pgn binaries files"
	@rm ./bin/analyse ./bin/pgn-extract ./${EXECUTABLE}