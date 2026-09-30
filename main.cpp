#include <iostream>
#include <unistd.h>
#include <cstdlib>
#include <vector>
#include <sys/wait.h>
#include <filesystem>
#include <cctype>


using namespace std;
namespace fs = filesystem;



string toLower(const string& text){
    // TODO: Alle Zeichen in Kleinbuchstaben umwandeln du kannst tolower(c) verwenden
    

}

bool filenamesMatch(const string& currentName, const string& filename, bool ignoreCase){
    // TODO: Dateinamen vergleichen
    //wenn ignoreCase false(!ignoreCase) ist, normal vergleichen.
    //Wenn ignoreCase true ist, Groß-/Kleinschreibung ignoeieren. 
    
}


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

    int pipefd[2];
    //TODO: unnamed Pipe erstellen
    //beim fehler EXIT_FAILURE zurückgeben


    for (int i = optind + 1; i<argc; i++) {
        
        pid_t pid = fork();

        switch (pid)
        {
            case -1: // error
                cout << "fork failed" << endl;
                return EXIT_FAILURE;

            case 0: { // child
                cout << "Child searches for: " << argv[i] << endl;

                //TODO: nicht benötigten Read-Descriptor schließen

                string results = searchFile(argv[optind], argv[i], recursive, ignoreCase);
                //TODO: nach der Suche die Ergebnisse über die Pipe seenden mit write() dann Write-Descriptor schließen
                
                
                exit(EXIT_SUCCESS);
            }

            default: // parent returns child PID
                cout << "Parent created child for: "<< argv[i] << endl;
                childPids.push_back(pid);
                break;
        }

    }

    //TODO: nicht benötigten Write-Descriptors schließen 
    //TODO: Ergebnisse aus der Pipe lesen 
    //TODO: Gelesne Ergebnisse auf stdout ausgeben und Read-Descriptor schließen

    // Parent waits for child to finish and prevents a zomnbie process
    for (pid_t childPid : childPids)
    {
        waitpid(childPid, nullptr, 0);
    }


    return 0;

}