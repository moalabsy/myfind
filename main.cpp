#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <sys/wait.h>
#include <filesystem>
#include <cctype>


using namespace std;
namespace fs = filesystem;


string toLower(const string& text);
bool filenamesMatch(const string& currentName, const string& filename, bool ignoreCase);

string checkEntry(const fs::directory_entry& entry, const string& filename, bool ignoreCase){

    if (entry.is_regular_file()){

        string currentName = entry.path().filename().string();
        if (filenamesMatch(currentName, filename, ignoreCase)){

            return to_string(getpid()) + ": " + filename + ": " + fs::absolute(entry.path()).string() + "\n";
        }    
            
    }

    return "";
}

string searchDirectory(const string& searchPath, const string& filename, bool ignoreCase){

    string results;
    //Go through all entries in the search directory
    for(const auto& entry : fs::directory_iterator(searchPath)) {
        results += checkEntry(entry, filename, ignoreCase);
    }

    return results;
}


string searchRecursive(const string& searchPath, const string& filename, bool ignoreCase){

    string results;
    for(const auto& entry : fs::recursive_directory_iterator(searchPath)) {
        results += checkEntry(entry, filename, ignoreCase);
    }

    return results;
}


string searchFile(const string& searchPath, const string& filename, bool recursive, bool ignoreCase) {
    
    if(recursive){
        return searchRecursive(searchPath, filename, ignoreCase);
    } else {
        return searchDirectory(searchPath, filename, ignoreCase);
    }

}

string toLower(const string& text)
{
    string result = text;
    for(char &c : result)
    {
        c = std::tolower(static_cast<unsigned char>(c));
    }
    return result;
}

bool filenamesMatch(const string& currentName, const string& filename, bool ignoreCase)
{
    if(!ignoreCase)
    {
        return currentName == filename;
    }
    return toLower(currentName) == toLower(filename);
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
    // The searchpath and at least one filename are required
    if (argc - optind < 2){
        cerr << "Usage: " << argv[0] << "[-R] [-i] searchpath filename1 filename2 ...\n";
    }

    vector<pid_t> childPids;

    int pipefd[2];
    // create the unnamed pipe and return EXIT_FAILURE on error
    if(pipe(pipefd) == -1)
    {
        perror("pipe failed");
        return EXIT_FAILURE;
    }


    for (int i = optind + 1; i<argc; i++) {
        
        pid_t pid = fork();

        switch (pid)
        {
            case -1: // error
                cout << "fork failed" << endl;
                return EXIT_FAILURE;

            case 0: { // child
                // close the unused read descriptor
                close(pipefd[0]);

                string results = searchFile(argv[optind], argv[i], recursive, ignoreCase);
                // send the results through the pipe using write()
                if (write(pipefd[1], results.c_str(), results.size()) == -1){
                    perror("write failed");
                    exit(EXIT_FAILURE);
                }

                close(pipefd[1]);
                exit(EXIT_SUCCESS);
            }

            default: // parent returns child PID
                childPids.push_back(pid);
                break;
        }

    }

    // close the unused write descriptor
     close(pipefd[1]);

    // print the results read from the pipe to stdout
    char buffer[256];
    ssize_t bytesRead;
    while((bytesRead = read(pipefd[0], buffer, sizeof(buffer) - 1)) > 0)
    {
        buffer[bytesRead] = '\0';
        cout << buffer;
    }
    // close the read descriptor
    close(pipefd[0]);

    // Parent waits for child to finish and prevents a zomnbie process
    for (pid_t childPid : childPids)
    {
        waitpid(childPid, nullptr, 0);
    }


    return 0;

}