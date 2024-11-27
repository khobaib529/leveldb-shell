#include <leveldb/db.h>
#include <iostream>
#include <sstream>
#include <regex>
#include "leveldb-shell/LevelDBShell.h"

// Macro for printing key-value pairs in a formatted manner.
#define PRINT_KEY_VALUE(key, value) \
    std::cout << "'" << key << "': '" << value << "'" << std::endl;

// Constructor for initializing the LevelDBShell with the given database path.
// It opens the LevelDB database and sets up the command map.
LevelDBShell::LevelDBShell(const std::string& db_path) {
    leveldb::Options options;
    options.create_if_missing = true;  // Create the database if it doesn't exist

    // Open the LevelDB database with the provided path.
    leveldb::Status status = leveldb::DB::Open(options, db_path, &m_db);
    if (!status.ok()) {
        std::cerr << "Failed to open LevelDB database: " << status.ToString() << std::endl;
        exit(1);  // Exit the program if the database fails to open
    }
    
    // Initialize the command map to associate commands with their handler functions.
    m_command_map["get"] = &LevelDBShell::GetCommand;
    m_command_map["put"] = &LevelDBShell::PutCommand;
    m_command_map["list"] = &LevelDBShell::ListCommand;
    m_command_map["delete"] = &LevelDBShell::DeleteCommand;
}

// Destructor to clean up resources by deleting the LevelDB database pointer.
LevelDBShell::~LevelDBShell() {
    delete m_db;
}

// Starts the shell, listening for user commands and executing them.
void LevelDBShell::Start() {
    std::cout << "LevelDB REPL Shell. Type 'exit' to quit." << std::endl;
    std::string command;
    while (true) {
        std::cout << "> ";  // Prompt for user input
        std::getline(std::cin, command);  // Read user input
        
        if (command == "exit") {
            std::cout << "Exiting the shell." << std::endl;
            break;  // Exit the shell if user types 'exit'
        }
        
        // Execute the parsed command
        ExecuteCommand(command);
    }
}

// Executes the given command by dispatching it to the corresponding handler function.
void LevelDBShell::ExecuteCommand(const std::string& command) {
    size_t space_pos = command.find(' ');  // Find the position of the first space in the command
    
    // Extract the command part (before the first space)
    std::string parsed_cmd = (space_pos != std::string::npos) ? command.substr(0, space_pos) : command;

    // Extract the arguments (after the first space), or "" if no space exists
    std::string args = (space_pos != std::string::npos) ? command.substr(space_pos + 1) : "";

    // Check if the parsed command exists in the command map
    if (m_command_map.find(parsed_cmd) != m_command_map.end()) {
        // Execute the corresponding command with its arguments
        (this->*m_command_map[parsed_cmd])(args);  // Call the function using the function pointer
    } else {
        std::cerr << "Unknown command: " << command << std::endl;  // Display error for unknown commands
    }
}

// Handles the "get" command to retrieve the value for a given key.
void LevelDBShell::GetCommand(const std::string& args) {
    std::string key = args;

    // Validate that the key is surrounded by single quotes
    if (key.size() >= 2 && key.front() == '\'' && key.back() == '\'') {
        key = key.substr(1, key.length() - 2);  // Remove quotes from the key
    } else {
        std::cerr << "Arguments must be surrounded by quotes." << std::endl;
        return;
    }

    // Retrieve the value associated with the key from the LevelDB database
    std::string value;
    leveldb::Status s = m_db->Get(leveldb::ReadOptions(), key, &value);
    if (s.ok()) {
        PRINT_KEY_VALUE(key, value);  // Print the key-value pair if retrieval is successful
    } else {
        std::cout << "Error retrieving key '" << key << "'" << std::endl;
    }
}

// Handles the "delete" command to remove a key-value pair from the database.
void LevelDBShell::DeleteCommand(const std::string& args) {
    std::string key = args;

    // Validate that the key is surrounded by single quotes
    if (key.size() >= 2 && key.front() == '\'' && key.back() == '\'') {
        key = key.substr(1, key.length() - 2);  // Remove quotes from the key
    } else {
        std::cerr << "Arguments must be surrounded by quotes." << std::endl;
        return;
    }

    // Delete the key-value pair from the LevelDB database
    leveldb::Status s = m_db->Delete(leveldb::WriteOptions(), key);
    if (s.ok()) {
        std::cout << "Deleted " << key << std::endl;  // Success message
    } else {
        std::cout << "Error: " << s.ToString() << std::endl;  // Error message if delete fails
    }
}

// Handles the "list" command to list all key-value pairs in the database.
void LevelDBShell::ListCommand(const std::string& args) {
    // Create an iterator to traverse the database
    leveldb::Iterator* it = m_db->NewIterator(leveldb::ReadOptions());
    
    // Iterate through all key-value pairs in the database
    for (it->SeekToFirst(); it->Valid(); it->Next()) {
        PRINT_KEY_VALUE(it->key().ToString(), it->value().ToString());  // Print each key-value pair
    }

    // Check for any errors encountered during the iteration
    assert(it->status().ok());
    delete it;  // Clean up the iterator
}

// Handles the "put" command to store a key-value pair in the database.
void LevelDBShell::PutCommand(const std::string& args) {
    // Regex to match key-value pairs, where quotes might be nested
    std::regex rgx("^'(.*)'\\s{1}'(.*)'$");
    std::smatch matches;

    // Attempt to match the command pattern
    if (std::regex_match(args, matches, rgx)) {
        // Extract key and value from the match groups
        std::string key = matches[1].str();
        std::string value = matches[2].str();

        // Process nested quotes (e.g., remove extra single quotes within the string)
        // We will replace '' (two single quotes) with a single quote inside the strings
        key = std::regex_replace(key, std::regex("''"), "'");
        value = std::regex_replace(value, std::regex("''"), "'");

        // Perform the "put" operation to store the key-value pair in LevelDB
        leveldb::Status s = m_db->Put(leveldb::WriteOptions(), key, value);
        if (s.ok()) {
            std::cout << "put '" << key << "' to '" << value << "'" << std::endl;  // Success message
        } else {
            std::cout << "Error: " << s.ToString() << std::endl;  // Error message if put operation fails
        }
    } else {
        // Display error if the command format is invalid
        std::cout << "Invalid 'put' command. Usage: put '<key>' '<value>'" << std::endl;
    }
}
