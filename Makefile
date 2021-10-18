mkfile_path = $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse

	chmod +x bin/engines/stockfish10_linux

# g++ -std=c++17 apgn.cpp -o apgn -DDEBUG -fsanitize=address -g
	g++ -std=c++17 apgn.cpp -o apgn -O3

test:
	./apgn ./pgn_samples/first.pgn W

test_clean:
	rm ./pgn_samples/firstAnalyzed.pgn

install:
	ln -s $(dir $(abspath $(lastword $(MAKEFILE_LIST))))apgn /usr/bin

uninstall:
	rm /usr/bin/apgn
	@rm ./bin/analyse ./bin/pgn-extract ./apgn

clean:
	@echo "removing pgn-extract object files"
	@$(MAKE) -C dependencies/pgn-extract clean
	@echo "removing uci-analyse object files"
	@$(MAKE) -C dependencies/uci-analyser clean
	@echo "removing analyse-pgn object files"