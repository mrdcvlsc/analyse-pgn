# UCI Notes

---

# Starting a game

```bash
uci
isready
ucinewgame
position startpos
```

---

# Making moves

You need to paste all the moves, concatenating new moves if there are one.

```bash
position startpos moves e2e4
position startpos moves e2e4 b7b6
```

---

# Analyse Moves What's The Best Move

To start analysis and get the best move, you can use the `go` command after 
setting the current positions/moves on the board.

```bash
position startpos moves e2e4
go depth 9
```

This will output something like shown below:

```bash
info depth 1 seldepth 1 multipv 1 score cp -6 nodes 632 nps 632000 tbhits 0 time 1 pv f5e6
info depth 2 seldepth 2 multipv 1 score cp -6 nodes 1432 nps 1432000 tbhits 0 time 1 pv f5e6 f1d3
info depth 3 seldepth 3 multipv 1 score cp -6 nodes 2019 nps 2019000 tbhits 0 time 1 pv f5e6 f1d3 d5g5
info depth 4 seldepth 4 multipv 1 score cp -6 nodes 2619 nps 2619000 tbhits 0 time 1 pv f5e6 f1d3 d5g5 g2g3
info depth 5 seldepth 5 multipv 1 score cp -6 nodes 3274 nps 3274000 tbhits 0 time 1 pv f5e6 f1d3 d5g5 g2g3 g5a5
info depth 6 seldepth 7 multipv 1 score cp -29 nodes 4988 nps 4988000 tbhits 0 time 1 pv f5d7 h4f3 d5a5 d1b3 c6c5 f1c4 e8g8
info depth 7 seldepth 10 multipv 1 score cp -29 nodes 6045 nps 6045000 tbhits 0 time 1 pv f5e6 f1d3 d5g5 g2g3 g5a5 e1g1 e8g8 a2a3 b4c3 b2c3
info depth 8 seldepth 17 multipv 1 score cp -41 nodes 19885 nps 9942500 tbhits 0 time 2 pv f5e6 f1d3 d5g5 g2g3 g5a5 e1g1 b8d7 h4g2 b4c3 b2c3
info depth 9 seldepth 15 multipv 1 score cp -48 nodes 25636 nps 8545333 tbhits 0 time 3 pv f5e6 f1d3 d5g5 g2g3 g5a5 e1g1 e8g8 h4g2 b8d7 g2f4 f8e8 a1c1 e6a2 c3a2 a5a2
bestmove f5e6 ponder f1d3
```

The last line `bestmove f5e6 ponder f1d3` shows the best move and the best reply to the best move.

---

### Analyse a Move

Almost the same output as the previous command with the only difference is that it limits the search only to the specified move(s).

You can use this to evaluate your move.

```bash
go depth 9 searchmoves a2a4
```

---

# The `go` analyze outputs explained

**Given the line**

```
info depth 9 seldepth 15 multipv 1 score cp -48 nodes 25636 nps 8545333 tbhits 0 time 3 pv f5e6 f1d3 d5g5 g2g3 g5a5 e1g1 e8g8 h4g2 b8d7 g2f4 f8e8 a1c1 e6a2 c3a2 a5a2
```

### Field-by-field explanation

* **`info`**
  Prefix used by UCI engines to send progress/status lines to the GUI. These are intermediate (not final) search reports.

* **`depth 9`**
  The current *search depth* measured in plies (half-moves). `9` here means the engine’s main search reached 9 plies from the root.

* **`seldepth 15`**
  *Selective depth* — the maximum depth reached in some variation after selective extensions (quiescence, checks, extensions). It can be larger than `depth` because some lines get searched deeper.

* **`multipv 1`**
  Which principal variation this info refers to. `1` = the engine’s top/best line. If `MultiPV` is >1 the engine reports multiple candidate lines and this tag disambiguates them.

* **`score cp -48`**
  The engine’s numerical evaluation in **centipawns** (1.00 = 1 pawn = 100 centipawns).

  * `cp -48` means the position is evaluated about **−0.48 pawns** (≈ half a pawn) **in favor of Black**. By convention a **positive** `cp` means advantage for White, **negative** for Black. (If there were a forced mate the engine would print `score mate N` instead.) ([python-chess.readthedocs.io][2])

* **`nodes 25636`**
  How many search tree nodes the engine has visited so far during this search (total node count).

* **`nps 8545333`**
  Nodes Per Second — the engine’s current speed. It’s computed from `nodes` / `time` (converted to seconds). Here the engine did 25,636 nodes in `time 3` ms → ~8,545,333 nps (25,636 ÷ 0.003 s). That’s why `nps` looks very large even though `nodes` is modest.

* **`tbhits 0`**
  Number of tablebase (Syzygy/etc.) probes/hits used during the search. `0` = no tablebase entries were consulted for this report. (If you have tablebase files available and the engine probes them, this number increases.) ([official-stockfish.github.io][3])

* **`time 3`**
  Time used so far for this search in **milliseconds**. (`time` in UCI is reported in ms.) In this line the engine has been searching ~3 ms.

* **`pv f5e6 f1d3 d5g5 g2g3 g5a5 e1g1 e8g8 h4g2 b8d7 g2f4 f8e8 a1c1 e6a2 c3a2 a5a2`**
  `pv` = *principal variation*, the sequence of moves the engine currently considers best. Moves are in coordinate (UCI) notation: `f5e6` = piece from f5 to e6, `f1d3` = piece/king/rook from f1 to d3, etc. This is the engine’s best line from the current root position according to its search so far. As search continues the PV and score may change.

### A couple of quick notes

* If you see `score mate N` instead of `score cp X`, that indicates a forced mate in N plies (positive = mate for White, negative = mate for Black). ([andreinc][4])
* `info` lines are **continuous updates**; the final choice is returned later as `bestmove ...`. Use `pv` and `score` as *current* candidates, not definitive until search ends.
