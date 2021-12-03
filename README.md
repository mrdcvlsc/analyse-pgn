# analyse-pgn++
https://github.com/mrdcvlsc/analyse-pgn

a simple chess game PGN file analyzer on terminal

## analyse-pgn

this program will generate an **.analyzed.pgn** file, and a **.stats.txt**.

The **.analyzed.pgn** will contain the following:

- per-move comments showing the best move
- per-move comments on how good or bad your move was compared to the best move.
  1. **```brilliant!!!```** - indicates that the move is the best move
  2. **```excellent!!```** - indicates that the move is a top move
  3. **```good!```** - indicates that the move is still accurate or average
  4. **```inaccurate?```** - indicates that the move provide a slight disadvantage
  5. **```mistake??```** - the move is slightly losing
  6. **```blunder/missed```** - blunder is a completely losing move, missed is when you give up a large advantage

The **.stats.txt** file will contain
- the total counts of brilliant, excellent, good, average, mistake, blunder, and missed moves in the pgn game.
- it will also provide a percentage representing the accuracy of moves a color played through out the whole game.

### the resulting Analyzed pgn is recomended ton be loaded on chess gui's like [Chess Arena](http://www.playwitharena.de/) for better visualization.

---------------------------

## Build Support
  - standard >= C++17
  - Windows MinGW Makefile
  - Linux GCC Makefile

**Build apgn**

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

  **Modifier Flags:**

    -engine [PATH]  - the directory location with the
                      filename of your pgn file

    -color [A,W,B]  - select one letter from A, W or B, where
                      W = white, B = black, and A = both

    -oskip [+I>=0]  - this is the number of moves in the opening
                      that the engine will not analyse
                      this value should be >= 0 and < the total moves

    -depth [+I>0]   - this is how deep the chess engine will analyse
                      the given pgn file, the larger the number the
                      the better the analysis, but will also take
                      more time to finish, this value should be >= 1

    -threads [+I>0] - this is the number of the worker threads you want
                      your engine to use, the more threads the faster
                      the analysis, given that you did not exceed your CPUs
                      maximum thread, but if you did a bigger thread will
                      also slow down the analysis

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

## how to use analyse-pgn in windows (Non-Developers) (old)

**Installation Part**

1. **Download** the pre-compiled program [analyse-pgn](https://github.com/mrdcvlsc/analyse-pgn/releases/tag/v0.7) | [direct-download-7z](https://github.com/mrdcvlsc/analyse-pgn/releases/download/v0.7/analyse-pgn.7z)
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
  - **```apgn /PATH/FILE.pgn -color COLOR```**

or

  - **```apgn /PATH/FILE.pgn -color COLOR -oskip OPENING_SKIP -depth DEPTH -threads THREADS```**
</ul>


example:

let us say for you have downloaded a pgn from one of your online games where you have played as the white piece
... let's call that pgn file **yourChessGame.pgn** and you want to analyse it.

to do that you need to input the command below in your **CMD**

```
apgn C:/Users/%USERNAME%/Downloads/yourChessGame.pgn -color W
```

_the command above will analyse **yourChessGame.pgn** file in the downloads folder for the player color white, this will produce another pgn file called **yourChessGameAnalyzed.pgn**, this is the pgn file that contains comments for each move on how well you did, and what is the best move for that turn, It will also produce another file called **yourChessGameAnalyzedStats.txt**, this will contain statistical information of your moves._

load **yourChessGameAnalyzed.pgn** on chess GUI's like [Chess Arena](http://www.playwitharena.de/) and [PGN Viewer](https://chesstempo.com/pgn-viewer/) to better visualize your moves.
