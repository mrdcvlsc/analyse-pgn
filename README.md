# analyse-pgn++
https://github.com/mrdcvlsc/analyse-pgn

a simple chess game PGN file analyzer

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

```
sudo make uninstall
cd ..
rm -rf analyse-pgn (lol)
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

## how to use analyse-pgn in windows (Non-Developers)

1. **Download** the pre-compiled program [analyse-pgn](https://github.com/mrdcvlsc/analyse-pgn/releases/tag/v0.7) | [direct-download-7z](https://github.com/mrdcvlsc/analyse-pgn/releases/download/v0.7/analyse-pgn.7z)
2. **Move** the file where you want to put it permanently, then **Extract** it there

***(Optional)*** Add the path of **analyse-pgn** in your environment variables. below are some tutorial on how to do this:

- [example1 - From Microsoft](https://docs.microsoft.com/en-us/previous-versions/office/developer/sharepoint-2010/ee537574(v=office.14)#:~:text=To%20add%20a%20path%20to%20the%20PATH%20environment%20variable&text=In%20the%20System%20dialog%20box,to%20Path%20and%20select%20it.)
- [example2 - Architect Ryan's Blog](https://www.architectryan.com/2018/03/17/add-to-the-path-on-windows-10/)
- [example3 - From Aseem Kishore](https://helpdeskgeek.com/windows-10/add-windows-path-environment-variable/)

3. **Start** the command line or cmd **terminal**, you can do this by either searching the keyword ```cmd``` in the windows start menu, or you can press the **```Windows Key```+```R```** then type ```cmd``` and click **OK**

Now a window with a black background should appear, that is the command line terminal (cmd), let's just call it **terminal**

4. If you have added the path of **analyse-pgn** in your environment variable you can access the program right away so you can skip this step and go to step 5, if you did not, you should type the command **```cd```** in the **terminal** then press the ```space``` key then you need to copy-paste the path of **analyse-pgn** (you can do this by selecting the path of analyse-pgn, right-click it and click copy, or use the shortcut key **```Ctrl+C```**, then paste it to the **terminal** by pressing **```Ctrl+V```**) then hit **Enter**, the command should look like the example below.

```
cd C:/Users/%USERNAME%/Downloads/analyse-pgn
```

5. Now you can **start analysing** pgn games, type ```apgn``` then press **space**, then open the folder where your pgn files are located, then copy-paste the path of the pgn file to the command line, then press **space** again, then input which color you want to analyse; W for white, B for black, and A for both colors.

the command should look like the example below:

```
apgn C:/Users/%USERNAME%/Downloads/yourChessGame.pgn W
```

let us say for example you downloaded a pgn from one of your online games... let's call that pgn file **yourChessGame.pgn**

_the command above will analyse **yourChessGame.pgn** file in the downloads folder, this will produce another pgn file called **yourChessGameAnalyzed.pgn**, this is the pgn file that contains comments for each move on how well you did, and what is the best move for that turn, It will also produce another file called **yourChessGameAnalyzedStats.txt**, this will contain the numbers of different moves you made and the accuracy of your moves in the game._
