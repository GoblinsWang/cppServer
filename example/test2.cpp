#include <iostream>
#include <string>
using namespace std;

int main()
{
    char *a = new char[10];
    a = (char *)"asanab";
    cout << a << endl;
    string s = a;
    cout << a << endl;
    delete[] a;
    cout << s << endl;

    return 0;
}