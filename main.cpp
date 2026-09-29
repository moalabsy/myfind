#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <sys/wait.h>
#include <filesystem>

using namespace std;
namespace fs = filesystem;



void checkEntry(const fs::directory_entry& entry, const string& filename){

    if (entry.is_regular_file()){

            string currenName = entry.path().filename().string();
            if(currenName == filename){
                cout << entry.path() << endl;
            }
        }
}

void searchDirectory(const string& searchPath, const string& filename){

    //Go through all entries in the search directory
    for(const auto& entry : fs::directory_iterator(searchPath)) {
        checkEntry(entry, filename);
    }
}


void searchRecursive(const string& searchPath, const string& filename){

    for(const auto& entry : fs::recursive_directory_iterator(searchPath)) {
        checkEntry(entry, filename);
    }
}


void searchFile(const string& searchPath, const string& filename, bool recursive) {
    
    if(recursive){
        searchRecursive(searchPath, filename);
    } else {
        searchDirectory(searchPath, filename);
    }

}




int main (int argc, char* argv[])
{
    bool recursive = false;
    bool ignoreCase = false;

    int option;

    while((option = getopt(argc, argv, "Ri")) != -1)
    {
        if (option == 'R'){
            recursive = true;
        }

        if (option == 'i'){
            ignoreCase = true;
        }
    }

    cout << "recrusive = " << recursive << endl;
    cout << "ignoreCase = " << ignoreCase << endl;
    cout << "searchpath: " << argv[optind] << endl;


    vector<pid_t> childPids;

    for (int i = optind + 1; i<argc; i++) {
        
        pid_t pid = fork();

        switch (pid)
        {
            case -1: // error
                cout << "fork failed" << endl;
                return EXIT_FAILURE;

            case 0: // child
                cout << "Child searches for: " << argv[i] << endl;
                searchFile(argv[optind], argv[i], recursive);
                exit(EXIT_SUCCESS);
            
            default: // parent returns child PID
                cout << "Parent created child for: "<< argv[i] << endl;
                childPids.push_back(pid);
                break;
        }

    }

    // Parent waits for child to finish and prevents a zomnbie process
    for (pid_t childPid : childPids)
    {
        waitpid(childPid, nullptr, 0);
    }


    return 0;

}