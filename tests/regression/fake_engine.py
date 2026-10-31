#!/usr/bin/env python3
"""A scripted UCI engine for apgn regression tests.

apgn starts the engine with no arguments, so the behaviour is driven by
environment variables (inherited from apgn):

FAKE_ENGINE_PLAN
    Path to a plan file. Each non-empty line not starting with '#' is

        <ply> <bestmove> <best_score> <player_score>

    <ply> is the number of moves in the current position (0 = start).
    Scores are written as cp:<n> or mate:<n>, from the side to move.
    A plain `go` answers with <bestmove> and <best_score>; a
    `go ... searchmoves <move>` answers with <move> and <player_score>.

FAKE_ENGINE_MODE
    normal                 (default) behave as described above.
    exit_on_start          exit immediately, before printing anything.
    info_string_after_pv   after the scored `info` line, print an
                           `info string` line before `bestmove`, as
                           real engines do (e.g. Stockfish NNUE notes).
"""

import os
import sys


def load_plan(path):
    plan = {}
    if not path:
        return plan
    with open(path, encoding="utf-8") as plan_file:
        for line in plan_file:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            ply, bestmove, best_score, player_score = line.split()
            plan[int(ply)] = (bestmove, best_score, player_score)
    return plan


def uci_score(token):
    kind, value = token.split(":")
    if kind not in ("cp", "mate"):
        raise ValueError("bad score token: " + token)
    return kind + " " + value


def send(line):
    sys.stdout.write(line + "\n")
    sys.stdout.flush()


def main():
    mode = os.environ.get("FAKE_ENGINE_MODE", "normal")
    if mode == "exit_on_start":
        return 0

    plan = load_plan(os.environ.get("FAKE_ENGINE_PLAN"))
    ply = 0

    send("Fake UCI engine for apgn tests")

    for raw in sys.stdin:
        command = raw.strip()

        if command == "uci":
            send("id name FakeEngine")
            send("id author apgn tests")
            send("uciok")
        elif command == "isready":
            send("readyok")
        elif command.startswith("position"):
            words = command.split()
            ply = len(words) - words.index("moves") - 1 if "moves" in words else 0
        elif command.startswith("go"):
            if ply not in plan:
                sys.stderr.write("fake engine: no plan for ply %d\n" % ply)
                send("bestmove (none)")
                continue
            bestmove, best_score, player_score = plan[ply]
            words = command.split()
            if "searchmoves" in words:
                move = words[words.index("searchmoves") + 1]
                score = player_score
            else:
                move = bestmove
                score = best_score
            send("info depth 1 seldepth 1 multipv 1 score %s nodes 1 pv %s" % (uci_score(score), move))
            if mode == "info_string_after_pv":
                send("info string fake engine finished searching")
            send("bestmove %s" % move)
        elif command == "quit":
            return 0

    return 0


if __name__ == "__main__":
    sys.exit(main())
