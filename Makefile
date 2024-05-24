mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
OS := $(shell uname)

CXX=g++
CXX_FLAGS=-static -static-libgcc -static-libstdc++ -std=c++17 -O3
ifeq ($(OS), Linux)
EXTENSION=
EXECUTABLE=apgn
else
EXECUTABLE=apgn.exe
EXTENSION=.exe
endif
INSTALLPATH=/usr/local/bin

all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse

ifeq ($(OS), Linux)
	chmod a+rx bin/engines/stockfish
else
	chmod a+rx bin/engines/stockfish.exe
endif
	${CXX} ${CXX_FLAGS} main.cpp -o ${EXECUTABLE}

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
	@echo "make install is not supported for windows"
# SETX /M PATH "%PATH%;$(dir $(abspath $(lastword $(MAKEFILE_LIST))))"
endif

uninstall:
ifeq ($(OS), Linux)
	@rm ${INSTALLPATH}/${EXECUTABLE}
else
	@echo "make uninstall is not supported for windows"
endif

clean:
	@echo "removing pgn-extract object files"
	@$(MAKE) -C dependencies/pgn-extract clean
	@echo "removing uci-analyse object files"
	@$(MAKE) -C dependencies/uci-analyser clean
	@echo "removing analyse-pgn binaries files"
	@rm ./bin/analyse${EXTENSION} ./bin/pgn-extract${EXTENSION} ./${EXECUTABLE}