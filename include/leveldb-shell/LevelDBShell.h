#ifndef LEVELDB_SHELL_H
#define LEVELDB_SHELL_H

#include <leveldb/db.h>
#include <string>
#include <map>
#include <iostream>

// LevelDBShell class provides a simple interface to interact with a LevelDB database
// through shell-like commands such as "put", "get", "set", "delete", and "list".
class LevelDBShell {
public:
    // Constructor initializes the LevelDBShell with the specified database path.
    // @param db_path: The path to the LevelDB database.
    explicit LevelDBShell(const std::string& db_path);

    // Destructor ensures proper resource cleanup and closes the database connection.
    ~LevelDBShell();

    // Starts the shell, listening for user commands and executing them.
    void Start();

private:
    // Pointer to the LevelDB database object used for all data operations.
    leveldb::DB* m_db;

    // A map that associates command strings with their respective handler functions.
    // Command strings are mapped to member function pointers.
    std::map<std::string, void (LevelDBShell::*)(const std::string&)> m_command_map;

    // Executes the given command by looking it up in the command map.
    // Dispatches to the corresponding handler function for that command.
    // @param command: The command string to be executed.
    void ExecuteCommand(const std::string& command);

    // Handles the "put" command which inserts or updates a key-value pair in the database.
    // Command format: "put <key> <value>"
    // @param args: The key-value pair to be inserted or updated.
    void PutCommand(const std::string& args);

    // Handles the "get" command which retrieves the value associated with a key.
    // Command format: "get <key>"
    // @param args: The key whose associated value is to be retrieved.
    void GetCommand(const std::string& args);

    // Handles the "set" command which updates the value of an existing key.
    // Command format: "set <key> <value>"
    // @param args: The key-value pair to update in the database.
    void SetCommand(const std::string& args);

    // Handles the "delete" command which removes a key-value pair from the database.
    // Command format: "delete <key>"
    // @param args: The key to be deleted from the database.
    void DeleteCommand(const std::string& args);

    // Handles the "list" command which lists all keys or a subset of keys in the database.
    // Command format: "list" or "list <prefix>"
    // @param args: An optional prefix to filter the keys.
    void ListCommand(const std::string& args);
};

#endif  // LEVELDB_SHELL_H
