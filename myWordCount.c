#include<iostream>
#include<fstream>
#include<sstream>
#include<string>
#include<vector>
#include<map>
#include<pthread.h>

std::vector<std::string> split(const std::string str, char delimiter)
{
    using namespace std;
    vector<std::string> vec;
    stringstream ss(str);
    string token;
    while(getline(ss, token, delimiter))
        vec.push_back(token);
    return vec;
}

int readfile(const char* filename, std::map<std::string, int>& d);

int main(int argc, char** argv){
    
    std::map<std::string, int> wdict;
    for ( int i=2; i<argc; i++ ){
        readfile( argv[i], wdict );
    }

    std::ofstream fout ( argv[1] );
    if ( fout.is_open() )
        for ( std::map<std::string, int>::iterator it = wdict.begin(); it!=wdict.end(); it++)
        {
            fout << it->second << '\t' << it->first << '\n';
        }
    return 0;
}
int readfile(const char* filename, std::map<std::string, int>& wdict){
    using namespace std;
    ifstream fin (filename);
    if (fin.is_open())
    {
        cout<<filename<<endl;
        string line;
        while( getline(fin, line) )
        {
            vector<string> elems = split(line, '\001');
            if ( elems.size()<3 ) continue;
            vector<string> words = split(elems[2], ' ');
            for(vector<string>::iterator it = words.begin(); it!=words.end(); it++)
                ++wdict[*it];
        }
        fin.close();
    }   
    return 0;
}
