#include <iostream>
#include <unistd.h>

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


    return 0;

}