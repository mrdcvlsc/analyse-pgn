# analyse-pgn++
https://github.com/mrdcvlsc/analyse-pgn

a simple chess game PGN file analyzer on terminal

**analyse-pgn** adds analysis comments for each moves in your pgn file

- shows the best move you should have played
- shows your blunders, mistakes, inaccuracy and etc.
- it uses a chess engine executable for analysis (default:stockfish11)
- the resulting Analyzed pgn can still be loaded on chess gui's like [Chess Arena](http://www.playwitharena.de/)

---------------------------

GCC C++ Standard >= C++17

<details>
<summary><b>Linx Build From Source</b></summary>
<br>
<ul>
  
```
git clone https://github.com/mrdcvlsc/analyse-pgn.git
cd analyse-pgn
make
sudo make install
make clean
```

**Uninstall**

in the analyse-pgn directory
  
```
sudo make uninstall
cd ..
rm -rf analyse-pgn
```

</ul>
</details>


<details>
<summary><b>Windows Build From Source</b></summary>
<br>
<ul>
  
You can also build analyse-pgn from source with GCC compilers, and the provided Makefile using your **cmd** in windows
  
download, or git clone **analyse-pgn** first(if you have git in windows)
  
in your command line (cmd) change directory to analyse-pgn, ```cd C:/Users/%USERNAME%/Downloads/analyse-pgn``` 
  
then use the commands below:
  
```
make
make clean
```

  _then after that you need to add the path of **analyse-pgn** into your environment variables... (or don't you can also use it right away after building it, just ```cd``` to where it is located and copy the full path of you pgn game that you want to analyse)_

</ul>
</details>

-------------------------------------------

## analyse a pgn game

**Command Format :**
  - **```apgn /PATH/FILE.pgn COLOR```**
  - **```apgn /PATH/FILE.pgn COLOR OPENING_SKIP DEPTH THREADS```**

***COLORs :***
- W = white
- B = black
- A = both

example:

if you want to analyse a pgn file called 'myGame.pgn' for the player color black, ```cd``` to where it is located (or copy it's whole path), then add the color you want to analyse, like the example below

```shell
apgn myGame.pgn B
```

this will produce another pgn file named 'myGameAnalyzed.pgn' on that same directory with analysis comments on each move for the player color black

------------------------------

## the env.txt

the env.txt contains the default settings for analysis, like how deep you want to analyse your moves, the chess engine you want to use, and etc.

------------------------------

## how to use analyse-pgn in windows (Non-Developers)

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
  - **```apgn /PATH/FILE.pgn COLOR```**

or

  - **```apgn /PATH/FILE.pgn COLOR OPENING_SKIP DEPTH THREADS```**
</ul>


example:

let us say for you have downloaded a pgn from one of your online games where you have played as the white piece
... let's call that pgn file **yourChessGame.pgn** and you want to analyse it.

to do that you need to input the command below in your **CMD**

```
apgn C:/Users/%USERNAME%/Downloads/yourChessGame.pgn W
```

_the command above will analyse **yourChessGame.pgn** file in the downloads folder for the player color white, this will produce another pgn file called **yourChessGameAnalyzed.pgn**, this is the pgn file that contains comments for each move on how well you did, and what is the best move for that turn, It will also produce another file called **yourChessGameAnalyzedStats.txt**, this will contain statistical information of your moves._

load **yourChessGameAnalyzed.pgn** on chess GUI's like [Chess Arena](http://www.playwitharena.de/) and [PGN Viewer](https://chesstempo.com/pgn-viewer/) to better visualize your moves.
