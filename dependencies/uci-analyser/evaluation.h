/*
 *  This file is part of uci-analyser: a UCI-based Chess Game Analyser
 *  Copyright (C) 2013-2017 David J. Barnes
 *
 *  uci-analyser is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  uci-analyser is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with uci-analyser.  If not, see <http://www.gnu.org/licenses/>.
 *
 *  David J. Barnes may be contacted as d.j.barnes@kent.ac.uk
 *  https://www.cs.kent.ac.uk/people/staff/djb/
 */

#ifndef EVALUATION_H
#define EVALUATION_H

#include <string>
#include <vector>

using namespace std;

/*
 * Details of the evaluation of a line.
 */
class Evaluation
{
  public:
    Evaluation(const vector<string> &tokens, const string &info)
    {
        extractInfo(tokens, info);
    }

    virtual ~Evaluation()
    {
    }

    inline unsigned getVariation() const
    {
        return variation;
    }
    
    /* Return the depth of the line. */
    inline int getDepth() const
    {
        return depth;
    }

    /* Return the value in centipawns. */
    inline int getValue() const
    {
        return cpValue;
    }

    inline bool isForcedMate() const
    {
        return forcedMate;
    }

    inline bool isLowerBound() const
    {
        return lowerBound;
    }

    inline bool isUpperBound() const
    {
        return upperBound;
    }

    inline int getNumMateMoves() const
    {
        return mateInMoves;
    }

    inline string getFirstMove() const
    {
        return moves[0];
    }

    inline string getTime() const
    {
        return time;
    }

  private:
      // The variation number.
      unsigned variation;
      // The depth.
      int depth;
      // The value in centi-pawns.
      int cpValue;
      // This value is a lower bound.
      bool lowerBound;
      // This value is an upper bound.
      bool upperBound;
      // The number of nodes.
      int nodes;
      // The time
      string time;
      // Whether the move gives forced mate.
      bool forcedMate;
      // Number of moves to mate (if forcedMate)
      int mateInMoves;
      // The moves of the line being evaluated.
      vector<string> moves;

      void extractInfo(const vector<string> &tokens, const string &info);
};

#endif
