#include "convert.hpp"

namespace apgn_convert
{
    int run_process(const std::string& program, const std::vector<std::string>& args)
    {
        std::cout << "---------------------\n";
        std::cout << "run sub-program: " << program << '\n';
        std::cout << "      arguments:\n";
        for (const auto& arg : args) {
            std::cout << "\t\t" << arg << '\n';
        }
        std::cout << '\n';

        if (!std::filesystem::exists(program)) {
            throw std::logic_error("'" + program + "' executable file not found");
        }

        boost::asio::io_context io_context;
        
        // Set up pipes for stdout and stderr
        boost::process::ipstream pipe_out;
        boost::process::ipstream pipe_err;
        
        // Start the process
        boost::process::child c(
            program,
            boost::process::args(args),
            boost::process::std_out > pipe_out,
            boost::process::std_err > pipe_err
        );

        // Read and print output (synchronously)
        std::string line;
        while (pipe_out && std::getline(pipe_out, line)) {
            std::cout << line << '\n';
        }

        while (pipe_err && std::getline(pipe_err, line)) {
            std::cerr << line << '\n';
        }
        
        // Wait for process to complete
        c.wait();
        
        std::cout << "=====================\n";

        return c.exit_code();
    }

    void uci_to_pgn(const std::string& input, const std::string& output)
    {
        auto program = (std::filesystem::path(apgnFileSys::getExecpath()) / "bin" / "pgn-extract" / 
            #if defined(_WIN32)
                "pgn-extract.exe"
            #else
                "pgn-extract"
            #endif
        ).string();
        
        std::vector<std::string> args = {
            "-WsanPNBRQK",
            "--output", output,
            input
        };

        run_process(program, args);
    }

    void pgn_to_uci(const std::string& input, const std::string& output)
    {
        auto program = (std::filesystem::path(apgnFileSys::getExecpath()) / "bin" / "pgn-extract" / 
            #if defined(_WIN32)
                "pgn-extract.exe"
            #else
                "pgn-extract"
            #endif
        ).string();

        std::vector<std::string> args = {
            "-Wuci",
            "--output", output,
            input
        };

        run_process(program, args);
    }

    void analyse_game(
        const std::string& input,
        const std::string& output,
        const std::string& engine,
        int search_depth,
        int threads,
        int opening_move_skips,
        int moves_until,
        char color
    )
    {
        std::cout << "==>>>>> game analysis\n";

        auto analyse_executable = (std::filesystem::path(apgnFileSys::getExecpath()) / "bin" / "analyse" / 
            #if defined(_WIN32)
                "analyse.exe"
            #else
                "analyse"
            #endif
        ).string();

        // Base arguments that are always used
        std::vector<std::string> args = {
            "--engine", engine,
            "--searchdepth", std::to_string(search_depth),
            "--bookdepth", std::to_string(opening_move_skips),
            "--movesuntil", std::to_string(moves_until),
            "--setoption", "Threads", std::to_string(threads),
            "--annotatePGN"
        };

        // Add color-specific flags
        switch (color)
        {
            case COLOR::WHITE:
                args.push_back("--whiteonly");
                break;
            case COLOR::BLACK:
                args.push_back("--blackonly");
                break;
            case COLOR::ALL:
                // No additional flags needed for analyzing all moves
                break;
            default:
                throw std::invalid_argument("Invalid color option");
        }

        args.push_back(input);

        if (!std::filesystem::exists(analyse_executable)) {
            throw std::logic_error("'" + analyse_executable + "' executable file not found");
        }

        // check if output file exist
        std::ifstream readf;
        readf.open(output);

        if(readf.fail()){
            readf.close();

            std::ofstream file;
            file.open(output);
            file.close();
        }

        std::ofstream outfile;
        outfile.open(output,std::ios_base::trunc);

        // Run analyse executable

        boost::asio::io_context io_context;
        
        boost::process::ipstream pipe_out;
        
        std::cout << "==>>>>> analyse executable : " << analyse_executable << '\n';

        for (const auto& eeee: args) {
            std::cout << eeee << ' ';
        }

        std::cout << '\n';

        boost::process::child c(
            analyse_executable,
            boost::process::args(args),
            boost::process::std_out > pipe_out
        );

        std::string line;

        while (pipe_out && std::getline(pipe_out, line)) {
            std::cout << "reading analyse standard output...\n";
            std::cout << "line = " << line << '\n';
            outfile << line << '\n';
        }
        
        // Wait for process to complete
        c.wait();

        // outfile<<result_pgn;
        outfile.close();
    }
}