# analyse-pgn++

a simple chess game PGN file analyzer

This program will add comments in your pgn file

- shows the best move you should have played instead
- shows your blunders, mistakes, inaccuracy and etc.
- it uses a chess engine executable for analysis (default:stockfish10)
- the resulting Analyzed pgn can still be loaded on chess gui's like [Chess Arena](http://www.playwitharena.de/)

## installation (linux-lubuntu)

GCC C++ Standard >= C++17

**install**

```
make
sudo make install
make clean
```

**uninstall**
```
sudo make uninstall
```

## analyse a pgn game

**Format : ```apgn PGN_FILE COLOR```**

***Colors :***
- W = white
- B = black
- A = both

example:

if you want to analyse a pgn file called 'myGame.pgn' for the player color black, ```cd``` to where it is located (or copy it's whole path), then add the color you want to analyse, like the example below

```shell
apgn myGame.pgn B
```

this will produce another pgn file named 'myGameAnalyzed.pgn' on that same directory with analysis comments on each move for the player color black

## the env.txt

the env.txt contains the default settings for analysis, like how deep you want to analyse your moves, the chess engine you want to use, and etc.
