#include "display_info.hpp"

#include <iostream>

void print_info() {
    std::cout << "\n\tA simple CLI chess game PGN-file analyser.\n"
                 "\tGPL-3.0 License\n"
                 "\thttps://github.com/mrdcvlsc/analyse-pgn\n"
                 "\tuse the command below for more information about this program:\n\n"
                 "\t\tapgn --help\n\n";
}

void print_help() {
    std::cout << "Help menu       : apgn --help\n"
                 "Display version : apgn --version\n\n"
                 "Flags:\n"

                 "--engine [PATH] - the directory location with the\n"
                 "                   filename of your pgn file.\n\n"

                 "--player [Both|White|Black] - select one of the options.\n"
                 "\tdefault: Both\n\n"

                 "--analyse-start-on-move [N] - the move number where the engine will start the\n"
                 "                              analysis, the engine will skip moves before it.\n"
                 "\tdefault: 0\n\n"

                 "--analyse-until-move [N] - the move number where the engine stops the analysis.\n"
                 "\tdefault: 1,000 (will analyse all pgn game moves below 1,000 moves)\n\n"

                 "--depth [N] - this is how deep the chess engine will analyse\n"
                 "              the given pgn file, the larger the number the\n"
                 "              the better the analysis, but will also take\n"
                 "              more time to finish.\n"
                 "\tdefault: 11\n\n"

                 "--threads [N] - this is the number of the worker threads you want\n"
                 "                your engine to use, the more threads the faster\n"
                 "                the analysis, given that you did not exceed your CPUs\n"
                 "                number of threads, but if you did a bigger thread could\n"
                 "                also slow down the analysis\n"
                 "\tdefault: ~75% of cpu threads\n\n"

                 "--hash-size [N] - size of memory used by the chess engine's hashtables, in Megabytes (MB)\n"
                 "\tdefault: 850\n\n"

                 "\n\n\tExample using default Values:\n\n"
                 "\tapgn myGame1.pgn myGame2.pgn\n\n\n"

                 "Examples with custom value:\n\n"
                 "    apgn --player Black --threads 4 myGame1.pgn myGame2.pgn\n\n"

                 "    apgn --engine C:/User/Downloads/Stockfish8.exe --depth 25 myGame1.pgn\n\n"

                 "    NOTE: this examples will use the custom values for the specified flags\n"
                 "          and use the default values for the unspecified ones.\n";
}

void print_version() {
    int executable_bit = sizeof(size_t) == 4 ? 32 : 64;
    std::cout << "apgn " << executable_bit << "-bit : " << APGN_VERSION << "\n";
}