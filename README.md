# analyse-pgn++
https://github.com/mrdcvlsc/analyse-pgn

a simple chess game PGN file analyzer

**analyse-pgn** adds analysis comments for each moves in your pgn file

- shows the best move you should have played
- shows your blunders, mistakes, inaccuracy and etc.
- it uses a chess engine executable for analysis (default:stockfish11)
- the resulting Analyzed pgn can still be loaded on chess gui's like [Chess Arena](http://www.playwitharena.de/)

## installation

GCC C++ Standard >= C++17

**install**

```
make
sudo make install
make clean
```

for windows ```sudo make install``` won't work so you need to add this path manually to your environment variables, look at these examples for more info [example1](https://docs.microsoft.com/en-us/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14)#:~:text=To%20add%20a%20path%20to%20the%20PATH%20environment%20variable&text=In%20the%20System%20dialog%20box,to%20Path%20and%20select%20it.), [example2](https://helpdeskgeek.com/windows-10/add-windows-path-environment-variable/), [example3](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)

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
