#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <stdlib.h>
#include <set>
#include <unistd.h>
#include <climits>

using namespace std;

class item{
public:
    set <unsigned int> str; // komponenta, není ukazatel, je potřeba dělat hluboký kopie
    item * nxt; // ukazatel na další
    item * bck; // ukazatel na předešlí
    item(){
        nxt = NULL;
        bck = NULL;
    }
};

class Stck{ // nevím, jestli se to dá nazvat zásobník, protože by stačila normální fronta nebo něco z knihovny
public:
    item * head;
    item * tail;
    unsigned int sz; // velikost zásobníku
    Stck(){
        sz = 0;
        head = NULL;
        tail = NULL;
    }
    ~Stck(){ // vzhledem k tomu, že se musí projít celý zásobník, nedává destruktor moc smysl
        while(head){
            item * tmp = head;
            head = head->nxt;
            delete tmp;
        }
    }
    void push(set <unsigned int> * stri){ // narve do zásobníku množinu vrcholů komponenty
        item * nwi = new item;
        nwi->str = *stri; // tohle provede hlubokou kopii
        if (head){
            head->bck = nwi;
            nwi->nxt = head;
            head = nwi;
        }else{
            head = nwi;
            tail = nwi;
        }
        sz ++;
    }
    set <unsigned int> pop(){ // vyndá množinu komponent ze zásobníku
        item * tmp = head;
        head = head->nxt;
        if (head != NULL)
            head->bck = NULL;
        set <unsigned int> stret = tmp->str;
        sz --;
        delete tmp; // pozor, maže po sobě
        return stret;
    }
    unsigned int size(){
        return sz;
    }
};

// Globální proměnnérelation count:
unsigned int a = 0; // načítá se z řádky
set <unsigned int> stxMin; // Podmnožina X
set <unsigned int> styMin; // Podmnožina Y
unsigned int relCntMin = UINT_MAX; // nejmenší počet relací
Stck wrkStc; // globální definice stacku, do kterýho se hází práce
//================

void print_out(){ // výpis výsledku
    cout << "X" << endl;
    set<unsigned int>::iterator iter; // iterátor množiny
    iter=stxMin.begin();
    while (iter != stxMin.end()){
        cout << *iter+1 << endl;
        iter++;
    }
    cout << "relation count: " << relCntMin << endl;
    cout << "Y" << endl;
    iter=styMin.begin();
    while (iter != styMin.end()){
        cout << *iter+1 << endl;
        iter++;
    }
    cout << "=============================" << endl;
}

void redStack(unsigned int stc, string * matrix){
    set<unsigned int>::iterator iter; // iterátor množiny
    if (wrkStc.size()){
        // vybere ze zásobníku komponentu X
        set <unsigned int> stx = wrkStc.pop();
        // vytvoření komponentu Y
        set <unsigned int> sty;
        for (unsigned int i = 0; i < stc; i++){
            if(stx.count(i) == 0){
                sty.insert(i);
            }
        }
        // spočte počet vazeb mezi podmnožinami
        unsigned int relCnt = 0;
        iter=stx.begin();
        while (iter != stx.end()){
            for (unsigned int j=0;j<stc;j++){
                if(matrix[*iter][j] == '1' && stx.count(j) == 0)
                    relCnt ++;
            }
            iter++;
        }
        if (relCnt < relCntMin){ // zvětší maxima
            relCntMin = relCnt;
            stxMin = stx;
            styMin = sty;
        }
 //       print_out();
    }
}

void gena(set <unsigned int> * str,unsigned int i,unsigned int stc,string * matrix){
    if (str->size() == a){ // pokud je velikosti množiny a, tak jej přidá na zásobník
        wrkStc.push(str); // a-tici nakopíruje do zásobníku na práci
        redStack(stc,matrix);
        return; // vygenerovaná a-tice, vrací se
    }else
    for(unsigned int j=i+1;j<stc;j++){ // přidá další hrany větší, než vstupní hrana
        str->insert(j); // přidá další hranu
        gena(str,j,stc,matrix); // pošle rekurzivně s novou hranou
        str->erase(j); // odejme přidanou hranu, aby mohl přidat další, větší
    }
}

void genAComp(unsigned int stc,string * matrix){ // pro každý vrchol vygeneruje množinu |a| vrcholů
    for(unsigned int i=0;i<stc;i++){
        set <unsigned int> * str = new set<unsigned int>;
        str->insert(i); // přidá první vrchol do množiny
        gena(str,i,stc,matrix); // přidá další vrcholy do velikosti |a|
        delete str; // množina se nakopírovala pro vložení na zásobník, může se smazat
    }
}

int main(int argc, char **argv){
    // testy vstupu
    FILE *fp = NULL;
    if (argc != 3){
         cout << "args: <matrix file> <a>" << endl;
         return 2;
    }
    fp = fopen(argv[argc-2],"r");
    if (fp == NULL){
         cout << "File doesn't exists" << endl;
         cout << argv[argc-2] << endl;
         return 1;
    }
    a = atoi(argv[argc-1]); // Načtení a
    char ch;
    unsigned int stc;
    int i = 0;
    bool frst = false;
    string stsCnt;
    string * matrix;
    // načítání vstupního souboru
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
    // výpis
    for(unsigned int j=0;j<stc;j++){
        cout << matrix[j] << endl;
    }
    // generování práce do zásobníku
    genAComp(stc,matrix);
    // práce nad zásobníkem
    redStack(stc,matrix);
    // Výpis výsledku
    print_out();
    // vyčištění
    delete [] matrix;
    return 0;
}
