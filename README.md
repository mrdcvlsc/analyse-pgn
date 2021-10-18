# analyse-pgn++

(linux only for now)

a simple chess game PGN file analyzer

This program will add comments in your pgn file

- shows the best move you should have played instead
- shows your blunders, mistakes, inaccuracy and etc.
- it uses a chess engine executable for analysis (default:stockfish10)
- the resulting Analyzed pgn can still be loaded on chess gui's like Arena

C++ Standard >= C++17

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
