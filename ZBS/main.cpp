#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <stdlib.h>
#include <set>
#include <unistd.h>
#include <climits>

// inputs from ZBS
int a = 3;
//================

using namespace std;

class CndCas{
    public:
        unsigned int cnt;
        set <unsigned int> * str;
        CndCas(unsigned int cnt,set <unsigned int> * str){
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
CndCas * countMatr(string * matrix,unsigned int linc, set <unsigned int> * str, unsigned int cnr = 0, int deep = 0){
    //cout << "picked" << linc + 1 << " deep " << deep << " " << matrix[linc] << endl; // jen pro debug
    int mnl = INT_MAX; // proměnná pro stanovaní minima mezi přechody
    int whl = linc; // označení vrcholu
    for(unsigned int k=0;k<matrix[linc].length();k++){ // hledá se přechod do stavu, který má nejméně přechodů do dalších stavů
        if(matrix[linc][k] == '1'){
            bool isIn = (str->count(k) == 0 ? false : true); // hnusnej oneliner - pokud je vrchol v komponentě, vrátí true, jinak false
            if (linc != k && !isIn){ // pokud vrchol není ten, který se zrovna zkoumá a není v množině, potom ...
                int mnt = min(cntTrans(matrix[k]), mnl); // vypočte počet přechodů a pokud je menší, než býval, dosadí
                if (mnt <= mnl){
                    mnl = mnt;
                    whl = k; // zapamatuje si vrchol, který měl méně přechodů, než ostatní
                }
            }else if (linc != k && isIn){
                cnr ++; // připočtení vazeb v komponentě
            }
        }
    }
    //sleep(1); // jen pro debug
    if (deep + 1 == a){ // výpočet se vstupním a, tedy dokud nejsou aspoň a stavy v množině, hledá dál
        CndCas * rtp = new CndCas(cnr, str); // jinak vytvoří výstupní třídu a končí
        //set<unsigned int>::iterator iter;
        //iter=str->begin();
        //while (iter != str->end()){
        //    cout << *iter+1 << endl;
        //    iter++;
        //}
        return rtp;
    }else{
        str->insert(whl); // přihodí vybraný vrchol do množiny, aby se k němu už nevracel
        return countMatr(matrix, whl, str, cnr, deep + 1); // předá se matice přechodu; nový vrchol; kde se bude hleda; počet přechodu; hloubka v grafu
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
    // display
    for(unsigned int j=0;j<stc;j++){
        cout << matrix[j] << endl;
    }
    // Výpočet
    CndCas winner(-1, NULL); // struktura, do který se uloží vítěz
    for(unsigned int j=0;j<stc;j++){ // pro každý vrchol otestuje, jeslti není v nejměnší komponentě
        //cout << "==========" << endl; // pro debug
        set <unsigned int> * str = new set<unsigned int>; // pro každý vrchol se vytvoří nová množina
        str->insert(j); // a strčí se do ní vrchol, kterým se začíná
        CndCas * cnas = countMatr(matrix, j, str); // pošle se do výpočtu matici přechodů; vrchol u kterého začal; množinu pro už prochozené stavy
        if (cnas->cnt <= winner.cnt){ // pokud je počet vazeb menší, než u vítěze, přiřadí jej k vítězi
            winner.cnt = cnas->cnt;
            winner.str = cnas->str;
        }else{
            delete str;
            delete cnas;
        }
    }
    cout << "Count win: " << winner.cnt << endl; // vypíše vítěze
    cout << "X" << endl;
    set<unsigned int>::iterator iter;
    iter=winner.str->begin();
    while (iter != winner.str->end()){
        cout << *iter+1 << endl;
        iter++;
    }
    // vypočtení vazeb mezi zbylími komponenty
    int cnv = 0; // proměnná pro počet vazeb
    iter=winner.str->begin();
    while (iter != winner.str->end()){
        for(unsigned int k=0;k<matrix[*iter].length();k++){
            if(matrix[*iter][k] == '1'){
                bool isIn = (winner.str->count(k) == 0 ? false : true); // hnusnej oneliner - pokud je vrchol v komponentě, vrátí true, jinak false
                if (*iter != k && !isIn){  // vrchol není v komponentě, tak připočte vazbu, která je mezi
                    cnv ++; // připočtení vazeb v komponentě
                }
            }
        }
        iter++;
    }
    cout << "relation count: " << cnv << endl;
    cout << "Y" << endl;
    for(unsigned int k=0;k<stc;k++){
        bool isIn = (winner.str->count(k) == 0 ? false : true);
        if (!isIn){
            cout << k+1 << endl;
        }
    }
    // deleting
    delete [] matrix;
    delete winner.str;
    return 0;
}
