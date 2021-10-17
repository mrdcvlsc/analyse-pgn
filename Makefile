all:
	$(MAKE) -C dependencies/pgn-extract
	mv dependencies/pgn-extract/pgn-extract  bin/pgn-extract

	$(MAKE) -C dependencies/uci-analyser
	mv dependencies/uci-analyser/analyse bin/analyse

	g++ -std=c++17 apgn.cpp -o apgn.out
	
test:
	./apgn.out "./test/first.pgn" "./test/myout.pgn" W

clean:
	$(MAKE) -C dependencies/pgn-extract clean
	$(MAKE) -C dependencies/uci-analyser clean
	rm execute.out ./bin/analyse ./bin/pgn-extract