# analyse-pgn

![ci](https://github.com/mrdcvlsc/analyse-pgn/actions/workflows/ci.yml/badge.svg)

A simple chess game PGN file analyser for the command-line terminal.

This program will generate an `analyzed-<original-pgn-filename>.pgn` file, and an `analyzed-<original-pgn-filename>.stat` from a pgn file.

The `analyzed-<original-pgn-filename>.pgn` will contain the following:

- per-move comment showing the best move.

- per-move comment on how good or bad the move was compared to the best move (list sorted by best to worst).
  1. **`brilliant !!!` or `accurate !!!`**
        - `brilliant !!!` - indicates that the move is the best move.
        - `accurate !!!` - the move was one of the top move but the player is still in a losing position.
  2. **`excellent !!` or `accurate !!`**
        - `excellent !!` -  indicates that the move is one of the top moves.
        - `accurate !!` - the move was one of the good moves but the player is still in a losing position.
  3. **`good !` or `questionable ?`**
        - `good !` - indicates that the move is decent or average, not a bad move.
        - `questionable ?` - the move is not accurate at all in an already losing position.
  7. **`inaccurate ?`** - indicates that the move resulted in a slight disadvantage.
  8. **`mistake ??`** - the move is slightly losing.
  8. **`mistake ???`** - the move is very much losing.
  9. **`missed !?? | blunder !??`**
        1. `missed` is when a player failed to find the best move that gives a very large advantage (missed win).
        2. `blunder` is a losing move that is highly likely to cause the player to lose at the end.
  
- per-move comment on what is the current status of the player (list sorted by best to worst).
  1.  **`WINNING`** - means that the ***advantage*** is so great the chance of ***winning*** is very high, as long as you don't blunder badly or play many inaccurate moves.
  2.  **`ADVANTAGE`** - slightly better than the opponent.
  3.  **`UNCERTAIN`** - means that the position is still equal; anything could happen.
  4.  **`DISADVANTAGE`** - slightly worse than the opponent.
  5.  **`LOSING`** - means that the ***disadvantage*** is so great the chances of ***losing*** are very high, as long as the other side does not blunder badly or play many inaccurate moves.

- per-move comment showing the `CP:<value>` (centipawn value) or the `MATE:<in-moves>` (checkmate in a certain number of moves).
  
Example Comment:

- `brilliant !!! WINNING CP:276` - the `brilliant !!!` part indicates you found the best move, `WINNING` indicates the position after the move is winning with a 2.76 pawn (CP:276) advantage.
- `mistake ?? WINNING CP:162` - the `mistake ??` part indicates your move is not one of the good moves; you lose some advantage, but your position is still `WINNING` with a 1.62 pawn (CP:162) advantage.
- `missed ??? ADVANTAGE CP:147` - the `missed ???` part indicates that there was a winning move you didn't find and missed a larger advantage; the `ADVANTAGE` part indicates that you still have a slight advantage despite the missed opportunity.

The `analyzed-<original-pgn-filename>.stat` file will contain:
- the total counts of brilliant, excellent, good, average, mistake, blunder, and missed moves in the pgn game.
- it will also provide a percentage representing the accuracy of moves a color played throughout the whole game.

## The resulting Analyzed pgn is recommended to be loaded on chess GUIs for better visualization like the following mentioned below:

- [Chess Arena](http://www.playwitharena.de/)
- [ingram-braun.net - online-pgn-viewer](https://ingram-braun.net/erga/online-pgn-viewer/)
- [chess.com](https://www.chess.com/analysis)
- [chesstempo.com](https://chesstempo.com/pgn-viewer/)

---------------------------

## Build Support
  - c++ standard: `>= C++17`.
  - Windows: `MinGW Makefile` + `gcc/g++` & `cmake v3.28+`.
  - Linux `gcc/g++ or clang/clang++` + `Makefile` (available by default in ubuntu flavors)

**Build apgn**

- For windows use `make TARGET=windows` instead of `make`.

- For windows debug build use `make TARGET=windows BUILD=Debug` instead of `make`.

```
git clone https://github.com/mrdcvlsc/analyse-pgn.git
cd analyse-pgn
make
make clean
```

**Install & Uninstall for Linux**
```
sudo make install
sudo make uninstall
```

-------------------------------------------

## analyse a pgn game

### The ```--help``` menu

**Flags:**

```bash
--engine [PATH] - the directory location with the
                   filename of your pgn file.
        default: ./bin/engine/stockfish[.exe] (stockfish 11)

--player [Both|White|Black] - select one of the options.
        default: Both

--analyse-start-on-move [N] - the move number where the engine will start the
                              analysis, the engine will skip moves before it.
        default: 0

--analyse-until-move [N] - the move number where the engine stops the analysis.
        default: 1,000 (will analyse all pgn game moves below 1,000 moves)

--depth [N] - this is how deep the chess engine will analyse
              the given pgn file, the larger the number the
              the better the analysis, but will also take
              more time to finish.
        default: 11

--threads [N] - this is the number of the worker threads you want
                your engine to use, the more threads the faster
                the analysis, given that you did not exceed your CPUs
                number of threads, but if you did a bigger thread could
                also slow down the analysis
        default: ~75% of cpu threads

--hash-size [N] - size of memory used by the chess engine's hashtables, in Megabytes (MB)
        default: 850
```

<br>

**Defaults Flag values** - if a flag is not specified, the default value will be used, below are the default values of each flags :

        engine  - ../analyse-pgn/bin/engines/stockfish11_x64
        color   - A
        oskip   - 4
        depth   - 11
        threads - 1

<br>

**Example 1** - Using Default Values:

    apgn myGame1.pgn myGame2.pgn

<br>

**Example 2** - Using Costum Values:

    apgn -color B -threads 4 myGame1.pgn myGame2.pgn

    NOTE: this examples will use the costum values for the specified flags
          and use the default values for the unspecified flags.

------------------------------

## how to use analyse-pgn in windows (Non-Developers)

**Installation Part**

1. **Download** the pre-compiled program [analyse-pgn](https://github.com/mrdcvlsc/analyse-pgn/releases/tag/v1.1) | [zip](https://github.com/mrdcvlsc/analyse-pgn/releases/download/v1.1/apgn.zip) | [7z](https://github.com/mrdcvlsc/analyse-pgn/releases/download/v1.1/apgn.7z) | [tar.gz](https://github.com/mrdcvlsc/analyse-pgn/releases/download/v1.1/apgn.tar.gz)
2. **Move** and **Extract** the file where you want to put it permanently

3. **Add path** of _analyse-pgn_ to your **Environment Variables**. below are some tutorial on how to do this:

- [example1 - From Microsoft](https://docs.microsoft.com/en-us/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14)#:~:text=To%20add%20a%20path%20to%20the%20PATH%20environment%20variable&text=In%20the%20System%20dialog%20box,to%20Path%20and%20select%20it.)
- [example2 - Architect Ryan's Blog](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)
- [example3 - From Aseem Kishore](https://helpdeskgeek.com/windows-10/add-windows-path-environment-variable/)

**Running The Program** (you can skip the steps 1,2,&,3 (Installation Part) after you have done it once)

4. **Copy** the filepath of the _.pgn_ file you want to analyse

4. **Open** the command line terminal **CMD**

5. **Follow the format** and **Paste** the pgn's path that you copied to **analyse** the game

<ul>

**Command Format :**
  - **```apgn -color COLOR /PATH/GAME.pgn ```**

or

  - **```apgn -color COLOR -oskip OPENING_SKIP -depth DEPTH -threads THREADS /PATH/GAME1.pgn /PATH/GAME2.pgn```**
  
  you can pass as many pgn file you want to the program
  
</ul>


example:

let us say for you have downloaded a pgn from one of your online games where you have played as the white piece
... let's call that pgn file **yourChessGame.pgn** and you want to analyse it.

to do that you need to input the command below in your **CMD**

```
apgn C:/Users/%USERNAME%/Downloads/yourChessGame.pgn -color W
```

_the command above will analyse **yourChessGame.pgn** file in the downloads folder for the player color white, this will produce another pgn file called **yourChessGame.analyzed.pgn**, this is the pgn file that contains comments for each move on how well you did, and what is the best move for that turn, It will also produce another file called **yourChessGame.stats.txt**, this will contain statistical information of your moves._

load **yourChessGame.analyzed.pgn** on chess GUI's like [Chess Arena](http://www.playwitharena.de/) and [PGN Viewer](https://chesstempo.com/pgn-viewer/) to better visualize your moves.
