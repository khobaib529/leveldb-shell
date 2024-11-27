#include <iostream>
#include <string>
#include <argparse/argparse.hpp>
#include "leveldb-shell/LevelDBShell.h"
#include "version.h"  // CMake generated version header

int main(int argc, char *argv[]) {
    // Create an instance of ArgumentParser to handle the command-line arguments.
    // Set the version directly in the constructor to avoid duplicates
    argparse::ArgumentParser program("leveldb-shell", LEVELDB_SHELL_VERSION_STRING);

    // Define the --db_path argument to specify the LevelDB database path
    program.add_argument("--db_path")
        .help("Path to the LevelDB database.")
        .required()
        .default_value("");

    // Parse the command-line arguments
    program.parse_args(argc, argv);

    // Retrieve the db_path argument value
    std::string db_path = program.get<std::string>("--db_path");

    // Ensure db_path is provided and not empty
    if (db_path.empty()) {
        std::cerr << "Error: --db_path is required." << std::endl;
        std::cerr << "Usage: " << argv[0] << " --db_path=<db_path>" << std::endl;
        return 1;  // Exit with an error code if db_path is missing
    }

    // Initialize the LevelDBShell with the provided db_path
    LevelDBShell shell(db_path);

    // Start the REPL (Read-Eval-Print Loop) for interacting with the database
    shell.Start();

    return 0;  // Exit successfully after starting the shell
}
