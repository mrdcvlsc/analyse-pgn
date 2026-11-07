# analyse-pgn

![ci](https://github.com/mrdcvlsc/analyse-pgn/actions/workflows/ci.yml/badge.svg)

A simple chess game PGN file analyser for the command-line terminal.

This program will generate an `analyzed-<original-pgn-filename>.pgn` file, and an `analyzed-stats-<original-pgn-filename>.json` from a pgn file, next to the original file (e.g. `game.pgn` gives `analyzed-game.pgn` and `analyzed-stats-game.json`). A pgn file that has no complete game in it (moves and a result such as `1-0`) is skipped with a warning.

The `analyzed-<original-pgn-filename>.pgn` will contain the following:

- a per-move variation showing the engine's best move, its score and how the game would continue, whenever the move played was not the best move.

- per-move comment on how good or bad the move was compared to the engine's best move (list sorted by best to worst). The difference is measured in centipawns (100 centipawns = 1 pawn). The second label of a pair is used instead of the first when the position is losing, i.e. its score is below -115 (`DISADVANTAGE` or `LOSING`).
  1. **`brilliant !!!` or `accurate !!!`**
        - `brilliant !!!` - indicates that the move is the best move (or forces mate at least as fast as the engine's best move).
        - `accurate !!!` - the move is the best move, but the player is still in a losing position (also used for the best defence when the player is getting mated).
  2. **`excellent !!` or `accurate !!`**
        - `excellent !!` -  indicates that the move is one of the top moves (less than 10 centipawns worse than the best move).
        - `accurate !!` - the move was one of the top moves, but the player is still in a losing position.
  3. **`good !` or `questionable ?`**
        - `good !` - indicates that the move is decent, not a bad move (less than 35 centipawns worse than the best move).
        - `questionable ?` - the move was decent, but the player is still in a losing position.
  4. **`inaccurate ?`** - the move is 35 to 70 centipawns worse than the best move.
  5. **`mistake ??`** - the move is 70 to 140 centipawns worse than the best move.
  6. **`mistake ???`** - the move is 140 to 222 centipawns worse than the best move (also used when the player gets mated sooner than with the best defence).
  7. **`missed !?? | blunder !??`** - the move is 222 or more centipawns worse than the best move.
        1. `missed` is when a player failed to find the best move that gives a very large advantage (missed win), but the player's score is still 0 or better. Also used for a forced mate that is slower than the engine's, or for missing a forced mate while still winning.
        2. `blunder` is a losing move that is highly likely to cause the player to lose at the end. Also used for a move that allows a forced mate.
  
- per-move comment on what is the current status of the player (list sorted by best to worst).
  1.  **`WINNING`** (score above +150, or a forced mate for the player) - means that the ***advantage*** is so great the chance of ***winning*** is very high, as long as you don't blunder badly or play many inaccurate moves.
  2.  **`ADVANTAGE`** (+116 to +150) - slightly better than the opponent.
  3.  **`UNCERTAIN`** (-115 to +115) - means that the position is still equal; anything could happen.
  4.  **`DISADVANTAGE`** (-150 to -116) - slightly worse than the opponent.
  5.  **`LOSING`** (score below -150, or the player is getting mated) - means that the ***disadvantage*** is so great the chances of ***losing*** are very high, as long as the other side does not blunder badly or play many inaccurate moves.

### **Standard NAGs instead of words**

With `--outputnag std`, the assessment is written as standard NAGs right after the move instead of apgn's words, so other programs can read it: `1. e4 $1 $18 { CP:300 }`. `--outputnag both` writes the NAG and the words. The move's quality maps as follows, in the way lichess and chessbase use, where only a notable or a bad move gets a glyph:

| apgn label | NAG |
|---|---|
| `brilliant !!!`, `accurate !!!` (the best move) | `$1` (!) |
| `excellent !!`, `accurate !!`, `good !`, `questionable ?` | none |
| `inaccurate ?` | `$6` (?!) |
| `mistake ??`, `mistake ???`, `missed !??` | `$2` (?) |
| `blunder !??` | `$4` (??) |

The position status becomes a position NAG. apgn's status is from the moving player's point of view, while position NAGs are always from White's, so it is converted:

| status | white to move | black to move |
|---|---|---|
| `WINNING` | `$18` (+-) | `$19` (-+) |
| `ADVANTAGE` | `$16` (+/-) | `$17` (-/+) |
| `UNCERTAIN` | `$10` (=) | `$10` (=) |
| `DISADVANTAGE` | `$17` (-/+) | `$16` (+/-) |
| `LOSING` | `$19` (-+) | `$18` (+-) |

The best-move variation keeps its comment and is given no NAGs, and the statistics keep using apgn's labels.

- per-move comment showing the `CP:<value>` (centipawn value, from the point of view of the player who made the move: positive is good for them) or the `MATE_IN:<n>` (forced checkmate in n moves; negative when the player is the one getting mated).
  
Example Comment:

- `brilliant !!! WINNING CP:276` - the `brilliant !!!` part indicates you found the best move, `WINNING` indicates the position after the move is winning with a 2.76 pawn (CP:276) advantage.
- `mistake ?? WINNING CP:162` - the `mistake ??` part indicates your move is not one of the good moves; you lose some advantage, but your position is still `WINNING` with a 1.62 pawn (CP:162) advantage.
- `missed !?? ADVANTAGE CP:147` - the `missed !??` part indicates that there was a winning move you didn't find and missed a larger advantage; the `ADVANTAGE` part indicates that you still have a slight advantage despite the missed opportunity.

The `analyzed-stats-<original-pgn-filename>.json` file will contain, for every game, its pgn tags and, for every analysed player (a color left out with `--player` is not listed):
- `move_stats` - how many of the player's moves got each label (`brilliant !!!`, `excellent !!`, ..., `blunder !??`).
- `accuracy` - a number from 0 to 1 representing the accuracy of the moves the color played throughout the whole game (1 means every move was `brilliant !!!`; each label has a weight, e.g. `excellent !!` 0.98, `accurate !!!` 0.80, `blunder !??` 0.01).
- `centipawns` - the score of each of the player's analysed moves (0 for moves that lead to a forced mate).

## The resulting Analyzed pgn is recommended to be loaded on chess GUIs for better visualization like the following mentioned below:

- [Chess Arena](http://www.playwitharena.de/)
- [ingram-braun.net - online-pgn-viewer](https://ingram-braun.net/erga/online-pgn-viewer/)
- [chess.com](https://www.chess.com/analysis)
- [chesstempo.com](https://chesstempo.com/pgn-viewer/)

---------------------------

## Build Support
  - c++ standard: `>= C++23`.
  - Windows: `MinGW Makefile` + `gcc/g++` & `cmake v3.28+`.
  - Linux `gcc/g++ or clang/clang++` + `Makefile` (available by default in ubuntu flavors) and `cmake v3.28+`.
  - The Boost (1.89) and glaze libraries are downloaded automatically by CMake the first time you configure the project, so an internet connection is needed for that.

## Possible Weird Errors That Could Happen

- when a third-party executable is not allowed/trusted in your system, for example the `pgn-extract.exe` or `stockfish.exe` (`pgn-extract` or `stockfish` for ubuntu/linux), the program might fail to run because of permission issues.

    use the `chmod` command in ubuntu to allow programs to read, write and execute process, e.g. `chmod +x bin/engines/stockfish bin/pgn-extract/pgn-extract`.

- when something goes wrong (for example a pgn file that does not exist, an invalid flag value, or a chess engine that stops responding) apgn prints `error: ...` and exits with status 1.

## For Developers

For developers who want to work on this project, install `clangd` in your system and vscode too, **config** with `-DCMAKE_BUILD_TYPE=Debug` and **build** with `--config Debug` using cmake to generate the `compile_commands.json` file in the `build` folder to enable clangd (lsp) auto-complete in vscode (for developers on windows, don't forget to use the `-G "MinGW Makefiles"`).

### **Windows Release Build**

```
git clone https://github.com/mrdcvlsc/analyse-pgn.git

cd analyse-pgn

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=gcc -DCMAKE_CXX_COMPILER=g++ -G "MinGW Makefiles"

cmake --build build --config Release
```

### **Ubuntu/Linux Release Build**

```
git clone https://github.com/mrdcvlsc/analyse-pgn.git

cd analyse-pgn

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++

cmake --build build --config Release
```

The build puts the `apgn` executable in the repository root and `pgn-extract` in `bin/pgn-extract/`. apgn looks for the `bin` folder (`bin/pgn-extract` and `bin/engines`) next to its own executable, so keep them together when you move it.

On Linux the `Makefile` wraps the same commands: `make` (configure and build in `build/`, `CMAKE_CONFIG=Release` by default), `make test` (run the tests) and `make package` (create the release `.zip`).

### **Running the tests**

```
ctest --test-dir build -C Debug --output-on-failure                # every test
ctest --test-dir build -C Debug -L regression --output-on-failure  # only the bug regression tests
```

The regression tests are in `tests/regression/`: one `test_*.cmake` script per fixed bug, registered with CTest automatically (helpers are in `tests/regression/common.cmake`). Tests that need exact engine scores use a small scripted UCI engine, `tests/regression/fake_engine.cpp`, which CMake builds next to apgn (target `fake_engine`). Scripts named `test_posix_*.cmake` are skipped on Windows (they need `make` or POSIX locales). CI runs all of them in the "Regression tests" step.

-------------------------------------------

## The ```--help``` menu

**Flags:**

```bash
--engine [PATH] - path to the UCI chess engine executable
                  used for the analysis.
        default: <apgn folder>/bin/engines/stockfish[.exe] (stockfish 11)

--player [Both|White|Black] - select one of the options.
        default: Both

--outputnag [std|apgn|both] - how to write the move assessment: standard NAGs,
                              apgn's own words, or both.
        default: apgn

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

**Allowed values** - apgn exits with an error when a value is outside these ranges:

| flag | allowed values |
|---|---|
| `--depth` | 1 to 100 |
| `--threads` | 1 to 1024 |
| `--hash-size` | 1 to 33554432 (MB) |
| `--outputnag` | `std`, `apgn` or `both` |
| `--analyse-start-on-move` | 0 to 10000 (0 and 1 both start from the first move) |
| `--analyse-until-move` | 1 to 10000 |

Only files with a `.pgn` extension (in any letter case, e.g. `GAME.PGN`) are treated as pgn files; other arguments are ignored with a message.


**Defaults Flag values** - if a flag is not specified, the default value will be used.

## Analyse a pgn game

### **Example 1** - Using Default Values:

    apgn myGame1.pgn myGame2.pgn


### **Example 2** - Using Custom Values:

    apgn --player Black --threads 4 myGame1.pgn myGame2.pgn

    # NOTE: these examples will use the custom values for the specified flags
    #      and use the default values for the unspecified flags.

------------------------------

## how to use analyse-pgn in windows (Non-Developers)

**Installation Part**

1. **Download** the pre-compiled program from the [latest release](https://github.com/mrdcvlsc/analyse-pgn/releases/latest): `release-apgn-windows-g++.zip` (on Linux: `release-apgn-linux-g++.zip` or `release-apgn-linux-clang++.zip`; the `debug-` archives are debug builds)
2. **Move** and **Extract** the file where you want to put it permanently (keep the `bin` folder next to `apgn`, it contains the chess engine and pgn-extract)

3. **Add path** of _analyse-pgn_ to your **Environment Variables**. below are some tutorial on how to do this:

- [example1 - From Microsoft](https://docs.microsoft.com/en-us/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14)#:~:text=To%20add%20a%20path%20to%20the%20PATH%20environment%20variable&text=In%20the%20System%20dialog%20box,to%20Path%20and%20select%20it.)
- [example2 - Architect Ryan's Blog](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)
- [example3 - From Aseem Kishore](https://helpdeskgeek.com/windows-10/add-windows-path-environment-variable/)

**Running The Program** (you can skip the steps 1, 2 & 3 (Installation Part) after you have done it once)

4. **Copy** the filepath of the _.pgn_ file you want to analyse

5. **Open** the command line terminal **CMD**

6. **Follow the format** and **Paste** the pgn's path that you copied to **analyse** the game

**Command Format :**
  - **```apgn --player White /PATH/GAME.pgn ```** or

  - **```apgn --player <White/Black/Both> --analyse-start-on-move <Positive-Number> --depth <Positive-Number>  --threads <Positive-Number>  /PATH/GAME1.pgn /PATH/GAME2.pgn```**
  
you can pass as many pgn file you want to the program


### example:

let us say for you have downloaded a pgn from one of your online games where you have played as the white piece
... let's call that pgn file **yourChessGame.pgn** and you want to analyse it.

to do that you need to input the command below in your **CMD**

```
apgn C:/Users/%USERNAME%/Downloads/yourChessGame.pgn --player White
```

_the command above will analyse **yourChessGame.pgn** file in the downloads folder for the player color white, this will produce another pgn file called **analyzed-yourChessGame.pgn**, this is the pgn file that contains comments for each move on how well you did, and what is the best move for that turn, It will also produce another file called **analyzed-stats-yourChessGame.json**, this will contain statistical information of your moves._

load **analyzed-yourChessGame.pgn** on chess GUI's like [Chess Arena](http://www.playwitharena.de/) and [PGN Viewer](https://chesstempo.com/pgn-viewer/) to better visualize your moves.
