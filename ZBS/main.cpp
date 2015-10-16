#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <stdlib.h>
#include <set>
#include <unistd.h>

// inputs from ZBS
int a = 3;
//================

using namespace std;

class CndCas{
    public:
        unsigned int cnt;
        set <int> * str;
        CndCas(unsigned int cnt,set <int> * str){
            this->cnt = cnt;
            this->str = str;
        }
};

int cntTrans (string line){
    int cnl = 0;
    for(unsigned int i = 0; i < line.length(); i++){
        if (line[i] == '1'){
            cnl ++;
        }
    }
    return cnl;
}
CndCas countMatr(string * matrix, int stc,unsigned int linc, set <int> * str, unsigned int cnr, int deep = 0){
    cout << "picked" << linc + 1 << " deep " << deep << " " << matrix[linc] << endl;
    int mnl = stc;
    int whl = linc;
    for(unsigned int k=0;k<matrix[linc].length();k++){
        if(matrix[linc][k] == '1'){
            if (linc != k && str->count(k) == 0){
                mnl = min(cntTrans(matrix[k]), mnl);
                whl = k;
            }
        }
    }
    str->insert(whl);
    sleep(1);
    if (deep + 1 == a){
        return CndCas(cnr + mnl, str);
    }else{
        countMatr(matrix, stc, whl, str, cnr + mnl, deep + 1);
    }
}
int main(int argc, char **argv){
    // input tests
    FILE *fp = NULL;
    if (argc < 2){
         cout << "Give name of relationship matrix file" << endl;
         return 2;
    }
    fp = fopen(argv[argc-1],"r");
    if (fp == NULL){
         cout << "File doesn't exists" << endl;
         return 1;
    }
    // read variables
    char ch;
    unsigned int stc;
    int i = 0;
    bool frst = false;
    string stsCnt;
    string * matrix;
    // reading input file
    ch = fgetc(fp);
    while(ch != EOF ){
        if(ch == '\n'){
            if(frst == false){
                stc=atoi(stsCnt.c_str());
                frst = true;
                matrix = new string[stc];
            }else
                i ++;
        }else{
            if (frst == false)
                stsCnt.push_back(ch);
            else
                matrix[i].push_back(ch);
        }
        ch = fgetc(fp);
    }
    fclose(fp);
    // sending to ZBS
    CndCas winner(-1, NULL);
    for(unsigned int j=0;j<stc;j++){
        cout << "==========" << endl;
        set <int> str;
        str.insert(j);
        //countMain(matrix,stc,j);
        CndCas cnas = countMatr(matrix, stc, j, &str, cntTrans(matrix[j]));
        if (cnas.cnt < winner.cnt){
            winner = cnas;
        }
    }
    cout << "Count win: " << winner.cnt << endl;
    set<int>::iterator iter;
    for(iter=winner.str->begin(); iter!=winner.str->end();++iter);{
        cout << *iter << endl;
    }
    // deleting
    delete [] matrix;
    return 0;
}
