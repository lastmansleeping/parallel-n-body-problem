/* rand example: guess the number */
#include<stdio.h>      /* printf, scanf, puts, NULL */
#include<stdlib.h>
#include<iostream>   /* srand, rand */
#include<fstream>
#include<time.h>       /* time */
#include<string>
using namespace std;

int main ()
{
    int num=100000;
    int x,y;
    ofstream myfile;
    myfile.open ("b100000.txt");
    /* initialize random seed: */
    srand (time(NULL));
    /* generate secret number between 1 and 10: */
    for(int i=1;i<=num;i++)
    {
        x = rand() % 100000;
        y = rand() % 100000;
        printf("%d %d\n",x,y);
        myfile <<x<<endl<<y<<endl;
    }
    /*
    string line;
    ifstream myfile2 ("position.txt");
    if (myfile2.is_open())
    {
        while ( getline (myfile2,line) )
        {
            cout << line << '\n';
        }
        myfile2.close();
    }
    else cout << "Unable to open file";
    */
    return 0;
}
