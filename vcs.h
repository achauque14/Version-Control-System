#ifndef VCS_H
#define VCS_H

#include <string>
#include <unordered_map>

using namespace std;

struct Commit {
    string commit_time;
    string commit_message;
    unordered_map<string, string> files;
};

class VersionControlSystem {
    const string repo_directory = ".vcs";
    const string files_directory = repo_directory + "/files";
    const string commits_directory = repo_directory + "/commits";
    const string index_path = repo_directory + "/index";
    const string log_path = repo_directory + "/log";

public:
    VersionControlSystem();
    void init();
    void add(const string& filename);
    void commit();
    void log();
    void revert(const string& commitHash, const string& filename);
};

string read_file(const string& filename);
string get_hash(const string& content);
void write_to_file(const string& filepath, const string& content);
string get_current_time();

#endif // VCS_H
