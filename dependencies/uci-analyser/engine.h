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

#ifndef ENGINE_H
#define ENGINE_H


#include <iostream>
#ifdef __unix__
#include <stdio.h>
#include <unistd.h>
#else
#include "stdafx.h"
#include <atlstr.h>
#include <windows.h>
#endif

#include <string>
#include <map>

using namespace std;

/*
 * Handle interactions with a UCI-compatible engine.
 */

class Engine {
public:

    Engine(const string& engineName) {
        if (!startEngine(engineName)) {
            throw -1;
        }
        identity = "unknown";
    }

    virtual ~Engine() {
    }

    bool checkIsReady(void);
    string getResponse(bool& eof);
    void go(void);
    bool initEngine(int variations, int searchDepth,
            map<string, string>& options);
    void quitEngine(void);
    void searchMoves(const string& moves);
    void send(const string& str);
    void send(const char *str);
    void setPosition(const string& moves, const string& fenstring);
    void setFENPosition(const string& fenstring, const string& moves);
    void setOption(const string& name, const string& value);
    void setOption(const string& name, int value);
    void setOptions(map<string, string>& options);

    inline void startNewGame(void) {
        send("ucinewgame");
        checkIsReady();
    }
    
    inline string getIdentity(void) {
        return identity;
    }
    bool waitForResponse(const char *str);

private:
    bool setIdentity(void);
#ifdef __unix__
    // The PID of the engine process.
    pid_t enginePID;
    // Communication to and from the engine.
    FILE *toEngine, *fromEngine;
#else
	HANDLE writeToEngine;
	HANDLE readFromEngine;
#endif
    // The engine's identity.
    string identity;
    // The number of variations to analyse.
    int variations;
    // The depth to analyse.
    int searchDepth;

    bool startEngine(const string&);
};

#endif

