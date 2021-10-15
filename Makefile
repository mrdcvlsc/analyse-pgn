all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract
	$(MAKE) -C dependencies/pgn-extract clean

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse
	$(MAKE) -C dependencies/uci-analyser clean

	g++ -std=c++17 execute.cpp -o execute.out
	./execute.out
