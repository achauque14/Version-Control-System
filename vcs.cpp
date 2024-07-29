#include <iostream>
#include <filesystem>
#include <string>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <vector>
#include <functional>
#include "vcs.h"


using namespace std;
namespace fs = std::filesystem;

VersionControlSystem::VersionControlSystem()
    : repo_directory(".vcs"),
      files_directory(repo_directory + "/files"),
      commits_directory(repo_directory + "/commits"),
      index_path(repo_directory + "/index"),
      log_path(repo_directory + "/log") {}


string read_file(const string& filename){
    ifstream file (filename);
    ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
};

string get_hash(const string& content){
    size_t hashValue = hash<string>{}(content);
    //converting to hexadecimal string
    stringstream ss;
    ss << hex << setw(16) << setfill('0') << hashValue;
    return ss.str();
};

void writeToFile(const string& filepath, const string& content) {
    ofstream outFile(filepath, ios::binary);
    outFile << content;
    outFile.close();
}

string getCurrentTime() {
    auto now = chrono::system_clock::now();
    auto now_c = chrono::system_clock::to_time_t(now);
    stringstream ss;
    ss << put_time(localtime(&now_c), "%Y-%m-%d %H:%M:%S");
    return ss.str();
};

void VersionControlSystem::init(){
    if (fs::exists(repo_directory)) {
        cout << "Repository already exists." << endl;
        return;
    }

    //Creating example text file 
    string filename = "example_file.txt";
    string file_message = "This is an automatically generated example file for the version control systems \n";

    ofstream example_file(filename);

    if (example_file.is_open()){
        example_file << file_message;
        example_file.close();
        cout << "Example text file created successfully \n";
    }else{
        cout << "Error creating file \n";
    };

    //Repo directory
    fs::create_directory(repo_directory);

    //Subdirectories
    fs::create_directory(files_directory);
    fs::create_directory(commits_directory);

    ofstream indexFile(index_path);
    indexFile.close();
    
    ofstream logFile(log_path);
    logFile.close();
    cout << "Repository initiated \n";
};

void VersionControlSystem::add(const string& filename){
    string file_content = read_file(filename);
    string fileHash = get_hash(file_content);
    string hashedFilePath = files_directory + "/" + fileHash;

    ofstream files(hashedFilePath);
    files << file_content;

    ofstream indexFile(index_path, ios::app);
    if (indexFile.is_open()){
        indexFile << fileHash << " " << filename << endl;
    };

    cout << filename << " has been added to the staging area. \n";
};

void VersionControlSystem::commit(){
    ifstream indexFile(index_path);
    if (!indexFile.is_open()){
        cout << "Error opening Index file" << endl;
        return;
    }

    unordered_map <string, string> stagedFiles;
    string fileHash, filename;
    while (indexFile >> fileHash >> filename) {
        stagedFiles[filename]= fileHash;
    }
    indexFile.close();

    if (stagedFiles.empty()) {
        cout << "No files to commit." << endl;
        return;
    }

    string commitMessage;
    cout << "Enter commit message: ";
    cin.ignore();
    getline(cin, commitMessage);

    string commitTime = getCurrentTime();
    stringstream commitContent;
    commitContent << "Commit Time: " << commitTime << "\n";
    commitContent << "Commit Message: " << commitMessage << "\n";
    commitContent << "Files:\n";
    for (const auto& file : stagedFiles) {
        commitContent << file.first << " " << file.second << "\n";
    }

    string commitHash = get_hash(commitContent.str());
    string commitFilePath = commits_directory + "/" + commitHash;
    writeToFile(commitFilePath, commitContent.str());

    ofstream logFile(log_path, ios::app);
    if (logFile.is_open()) {
        logFile << commitHash << " " << commitTime << " " << commitMessage << endl;
        logFile.close();
    }

    ofstream clearIndex(index_path, ios::trunc);
    clearIndex.close();

    cout << "Your " << commitHash << " Commit was successfull." <<endl;
};

void VersionControlSystem::log(){
    ifstream logFile(log_path);
    if (!logFile.is_open()) {
        cout << "Error opening log file." << endl;
        return;
    }

    string line;
    while (getline(logFile, line)) {
        cout << line << endl;
    }
    logFile.close();
};

void VersionControlSystem::revert(const string& commitHash, const string& filename) {
// Open the commit file to find the file hash
    ifstream commitFile(commits_directory + "/" + commitHash);
    string line, fileHash;

    while (getline(commitFile, line)) {
        istringstream iss(line);
        string tempHash, tempFilename;
        iss >> tempHash >> tempFilename;
        if (tempFilename == filename) {
            fileHash = tempHash;
            break;
        }
    }

    if (fileHash.empty()) {
        cout << "No file found with specified commit." << endl;
        return;
    }

    // Read the content of the file from the VCS storage and write it to the working directory
    ifstream file(files_directory + "/" + fileHash, ios::binary);
    ofstream outFile(filename, ios::binary);
    outFile << file.rdbuf();

    cout << "File '" << filename << "' has been reverted successfully to commit " << commitHash << "." << endl;
};

