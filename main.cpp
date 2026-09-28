#include <iostream>
#include <unistd.h>
#include <cstdlib>

using namespace std;


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

    for (int i = optind + 1; i<argc; i++) {
        
        pid_t pid = fork();

        switch (pid)
        {
            case -1: /* error */
                cout << "fork failed" << endl;
                return EXIT_FAILURE;

            case 0: /* child */
                cout << "Child searches for: " << argv[i] << endl;
                exit(EXIT_SUCCESS);
            
            default: /* parent */
                cout << "Parent created child for: "<< argv[i] << endl;
                break;
        }

    }


    return 0;

}