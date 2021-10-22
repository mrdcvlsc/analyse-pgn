mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

OS := $(shell uname)

all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse

ifeq ($(OS), Linux)
	chmod +x bin/engines/stockfish11_x64
	g++ -static-libgcc -static-libstdc++ -std=c++17 apgn.cpp -o apgn -O3
# g++ -std=c++17 apgn.cpp -o apgn -DDEBUG -fsanitize=address -g # For Debugging
else
	chmod +x bin/engines/stockfish11_x64.exe
	g++ -static-libgcc -static-libstdc++ -std=c++17 apgn.cpp -o apgn.exe -O3
endif

test:
ifeq ($(OS), Linux)
	./apgn ./pgn_samples/first.pgn W
else
	apgn.exe pgn_samples/first.pgn W
endif

test_clean:
ifeq ($(OS), Linux)
	rm ./pgn_samples/firstAnalyzed.pgn
else
	rm pgn_samples/firstAnalyzed.pgn
endif

install:
ifeq ($(OS), Linux)
	@ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))apgn /usr/bin
# @ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))apgn /usr/local/bin
# @ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))apgn /home/$(USER)/.local/bin
else
	@echo "Set It Manually for now"
# SETX /M PATH "%PATH%;$(dir $(abspath $(lastword $(MAKEFILE_LIST))))"
endif

uninstall:
ifeq ($(OS), Linux)
	# @rm /usr/bin/apgn
# @rm /usr/local/bin/apgn
# @rm /home/$(USER)/.local/bin/apgn
	@rm ./bin/analyse ./bin/pgn-extract ./apgn
else
	@rm ./bin/analyse.exe ./bin/pgn-extract.exe ./apgn.exe
endif

clean:
	@echo "removing pgn-extract object files"
	@$(MAKE) -C dependencies/pgn-extract clean
	@echo "removing uci-analyse object files"
	@$(MAKE) -C dependencies/uci-analyser clean
	@echo "removing analyse-pgn object files"