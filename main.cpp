#include <iostream>
#include "vcs.h"

using namespace std;

int main(){
    VersionControlSystem vcs;
    string command, hash, filename;
    
    while (true) {
        cout << "Choose one of the following commands (init, add, commit, log, revert, exit): ";
        cin >> command;
        
        if (command == "init") {
            vcs.init();
        } else if (command == "add") {
            cout << "Enter filename to add: ";
            cin >> filename;
            vcs.add(filename);
        } else if (command == "commit") {
            vcs.commit();
        } else if (command == "log") {
            vcs.log();
        } else if (command  == "revert") {
            cout << "Enter commit hash to revert: ";
            cin >> hash;
            cout << "Enter filename to revert: ";
            cin >> filename;
            vcs.revert(hash, filename);
        } else if (command == "exit"){
            break;
        };
    }
    return 0;
};