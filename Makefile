mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))
OS := $(shell uname)
TARGET:=
BUILD:=Release

CXX=g++
CXX_FLAGS:=-std=c++17 -static-libgcc -static-libstdc++ -Werror=shadow
BUILD_TYPE:=-O2
ifeq ($(OS), Linux)
EXTENSION=
EXECUTABLE=apgn
else
EXECUTABLE=apgn.exe
EXTENSION=.exe
endif
INSTALLPATH=/usr/local/bin

ifeq ($(BUILD),Release)
	BUILD_TYPE=-O3
else ifeq ($(BUILD),Debug)
	BUILD_TYPE=-g3 -O0 -fno-omit-frame-pointer
endif

ifeq ($(TARGET),windows)
  MKDIR = @if not exist "$(subst /,\,$(1))" mkdir "$(subst /,\,$(1))
else
  MKDIR = @mkdir -p $(1)
endif

all:
	@echo OS : $(OS)
	$(call MKDIR,bin)
	$(call MKDIR,bin/engines)
	$(call MKDIR,bin/pgn-extract)
	$(call MKDIR,bin/analyse)

	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract$(EXTENSION)  bin/pgn-extract/

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse$(EXTENSION) bin/analyse/

ifeq ($(OS), Linux)
	chmod +x bin/engines/stockfish
else
	chmod +x bin/engines/stockfish.exe
endif
	${CXX} ${CXX_FLAGS} ${BUILD_TYPE} main.cpp -o ${EXECUTABLE}

test:
	./${EXECUTABLE} ./pgn_samples/first.pgn -color W

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