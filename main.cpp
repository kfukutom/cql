#include <iostream>
#include <utility>
#include <exception>
#include "Field.h"
#include "Database.h"
using namespace std;

int main(int argc, char** argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    cout.tie(nullptr);

    cin >> std::boolalpha;
    cout << std::boolalpha;

    // test_field();
    Database s;
    s.getMode(argc, argv);
    if (s.isQuiet()) {
        s.enabled = true;
        //std::cout << s.enabled << endl;
    } else {
        s.enabled = false;
    }
    s.run();
    //s.cleanup(); // allocate dead mem: s.cleanup();
    return 0;
}
