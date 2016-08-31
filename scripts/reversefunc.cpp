#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

int main()
{
    char s[1000];
    vector<string> inst;
    ifstream ifs("cgpop.real.ll");
    ofstream ofs("reverse.ll");
    ifs.getline(s,1000);
    while(!ifs.eof())
    {
        if(s[0]=='d' && s[1]=='e' && s[2]=='f' && s[3]=='i' && s[4]=='n' && s[5]=='e')
        {
            ofs << s << endl;
            ifs.getline(s,1000);
            while(s[0]!='}')
            {
                inst.push_back(string(s));
                ifs.getline(s,1000);
            }
            for(vector<string>::reverse_iterator it=inst.rbegin(),end=inst.rend();it!=end;++it)
            {
                ofs << *it << endl;
            }
            inst.clear();
            ofs << s << endl << endl;
        }
        else ofs << s << endl;
        ifs.getline(s,1000);
    }
    ifs.close();
    ofs.close();
    return 0;
}
