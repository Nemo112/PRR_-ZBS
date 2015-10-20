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

class ZasobnikUzlu
{
	private:
            int ukazatel_zasobniku;
            int * uzly;
            int * pocet_spojeni;
            int maxSize;
	public:
            ZasobnikUzlu();
            void NastavMaximum(int maxSize);
            void UlozUzel(int uzel);
            bool ObsahujeUzel(int uzel);
            void SmazUzel();
            void Update();
            int Pop();
            int See(int pozice);
            void Push(int uzel, int spojeni);
            void Clear();
            int * vybrane_uzly;
            int pocet_vybranych_uzlu;
};

void ZasobnikUzlu::Clear()
{
    ukazatel_zasobniku = 0;
    pocet_vybranych_uzlu = 0;
    uzly = new int [maxSize];
    pocet_spojeni = new int [maxSize];
    vybrane_uzly = new int[a];
}

ZasobnikUzlu::ZasobnikUzlu()
{
    ukazatel_zasobniku = 0;
    pocet_vybranych_uzlu = 0;
}

void ZasobnikUzlu::NastavMaximum(int maxSize)
{
    this->maxSize = maxSize;
    uzly = new int [maxSize];
    pocet_spojeni = new int [maxSize];
    vybrane_uzly = new int[a];
    cout << "Maximum zasobniku: " << maxSize << endl;
}

void ZasobnikUzlu::Push(int uzel, int spojeni)
{
    if (ukazatel_zasobniku == maxSize - 1 ){
	cout << "Nekde se stala chyba..." << endl;
    }
    int pozice = ukazatel_zasobniku;
    // Hledam, kam zaradim prvek, aby byly serazeny od nejmensiho poctu hran po nejvetsi
    for (int i = ukazatel_zasobniku - 1; i >= 0; i--)
    {
        if (pocet_spojeni[i] >= spojeni)
        {
            pozice = i;
        }
    }
    // Nyni prvky pouze posouvam
    int next = uzly[pozice];
    int next_value = pocet_spojeni[pozice];
    int previous = uzel;
    int previous_value = spojeni;
    for (int i = pozice; i < ukazatel_zasobniku + 1; i++)
    {
        uzly[i] = previous;
        pocet_spojeni[i] = previous_value;
        previous = next;
        previous_value = next_value;
        next = uzly[i+1];
        next_value = pocet_spojeni[i+1];
    }
    ukazatel_zasobniku++;
}

void ZasobnikUzlu::Update()
{
  //CHYBI
  // Je potreba aktualizovat zaznamy v zasobniku, napr prida se uzel 1 a informace k nemu
  // ze ma 5 hran mimo komponentu, ale pak se prida uzel 2 ,ktery ma spolecnou komponentu s 1, tak aby
  // se u zaznamu uzlu 1 upravilo, ze ma nyni 4 hrany mimo komponentu, misto 5 (ta jedna je uz vne komponenty)
}

// Ulozim uz nalezeny optimalni uzel
void ZasobnikUzlu::UlozUzel(int uzel)
{
    vybrane_uzly[pocet_vybranych_uzlu] = uzel;
    pocet_vybranych_uzlu++;
}

bool ZasobnikUzlu::ObsahujeUzel(int uzel)
{
    bool obsahuje = false;
    // Je hledany uzel v zasobniku?
    for (unsigned int i = 0; i < ukazatel_zasobniku; i++)
    {
        if (uzly[i] == uzel)
        {
            obsahuje = true;
        }
    }
    // Je hledany uzel v poli vybranych uzlu?
    for (unsigned int i = 0; i < pocet_vybranych_uzlu; i++)
    {
        if (vybrane_uzly[i] == uzel)
        {
            obsahuje = true;
        }
    }
    // Pokud ani jedno = neobsahuje a mohu znovu ulozid do zasobniku
    return obsahuje;
}

void ZasobnikUzlu::SmazUzel()
{
    pocet_vybranych_uzlu--;
}

int ZasobnikUzlu::Pop()
{
    if (ukazatel_zasobniku == 0) {
        return -1;
    }
    else {
        ukazatel_zasobniku--;
        return uzly[ukazatel_zasobniku];
    }
}

// pouze pro debug
int ZasobnikUzlu::See(int pozice)
{
    if (pozice < 1 || pozice > maxSize - 1) {
        return -1;
    }
    else {
        for (int i = 0; i < ukazatel_zasobniku; i++)
        {
            cout << "Pozice: " << i << ", hodnota: " << uzly[i] << ", spojeni: " << pocet_spojeni[i] << endl;
        }
        return uzly[pozice];
    }
}

int cntTrans (string line){
    int cnl = 0;
    for(unsigned int i = 0; i < line.length(); i++){
        if (line[i] == '1'){
            cnl ++;
        }
    }
    return cnl;
}

int PocetUzluMimo (string line, int picked[], int size)
{
    int cnl = 0;
    for(unsigned int i = 0; i < line.length(); i++){
        if (line[i] == '1' && picked[i] != 1){
            cnl ++;
        }
    }
    return cnl;

}

// Overi, zda jsou komponenty spojite
bool GrafJeSpojity(string * matrix, int vybrane_uzly[], int velikost_vsech, int velikost_vybranych_uzlu)
{
    bool * over_integritu = new bool[velikost_vsech];
    bool nevybrany_usel = true;
    for (unsigned int i = 0; i < velikost_vsech; i++)
    {
        over_integritu[i] = false;
    }
    // Cyklus pro vsechny dostupne uzly, ktere jsou v zadani
    for (unsigned int i = 0; i < velikost_vsech; i++)
    {
        nevybrany_usel = true;
        // Cyklus pres vsechny uzly, ktere jsou jiz vybrany
        for (unsigned int j = 0; j < velikost_vybranych_uzlu; j++)
        {
            // Vyberu pouze ty, ktere nejsou vybrany (u vybranych uzlu spojitost zajistuje sam o sobe algoritmus) - jsou v komponente n-a
            if (vybrane_uzly[j] == i)
            {
                nevybrany_usel = false;
            }
        }
        if (nevybrany_usel)
        {
            string line = matrix[i];
            for(unsigned int k = 0; k < line.length(); k++){
                // Hledam jednicky a pokud najdu, pripisu k danemu indexu true
                if (line[k] == '1')
                {
                    over_integritu[k] = true;
                }
            }
        }
    }
    // Prochazim veskere indexy u druhe komponenty, zda maj vsechny true = tzn existuje alepson jedna cesta na vsechny uzly
    for (unsigned int i = 0; i < velikost_vsech; i++)
    {
        nevybrany_usel = true;
        for (unsigned int j = 0; j < velikost_vybranych_uzlu; j++)
        {
            if (vybrane_uzly[j] == i)
            {
                nevybrany_usel = false;
            }
        }
        if (over_integritu[i] == false)
        {

            return false;
        }
    }
    return true;
}

// Zda prvek je v poli
bool Contains(int prvek, int pole[], int velikost_pole)
{
    bool obsahuje = false;
    for (unsigned int i = 0; i < velikost_pole;i++)
    {
        if (pole[i] == prvek)
        {
            obsahuje = true;
        }
    }
    return obsahuje;
}

// Vypocet, kolik hran komponenty obsahuji
int CelkovyPocetHranKomponent(string * matrix, int vybrane_uzly[])
{
    int pocet_a_hran = 0;
    int pocet_b_hran = 0;
    for (unsigned int i = 0; i < matrix->length() - 1;i++)
    {
        string line = matrix[i];
        for (unsigned int j = i; j < matrix->length() - 1; j++)
        {
            if (line[j] == '1')
            {
                if (Contains(i, vybrane_uzly, a) == 0 && Contains(j, vybrane_uzly, a) == 0 )
                {
                    pocet_b_hran++;
                }
                else if (Contains(i, vybrane_uzly, a) == 1 && Contains(j, vybrane_uzly, a) == 1 )
                {
                    pocet_a_hran++;
                }
            }
        }
    }
    cout << "Hran komponenty 'a' = " << pocet_a_hran << ", hran komponenty 'n-a' = " << pocet_b_hran << endl;
    cout << "Celkem: " << (pocet_a_hran) + (pocet_b_hran) << endl;
    return (pocet_a_hran) + (pocet_b_hran);
}



// Jsem prase a globalni deklarace tridy
ZasobnikUzlu zasobnik;

CndCas * countMatr(string * matrix,unsigned int linc, int deep = 0){
    cout << "picked " << linc << " deep " << deep << " " << matrix[linc] << endl << endl; // jen pro debug
    bool je_spojity = GrafJeSpojity(matrix, zasobnik.vybrane_uzly, matrix->length() - 1, zasobnik.pocet_vybranych_uzlu);
    // Graf neni OK, tudy cesta nevede
    if (je_spojity == 0)
    {
        int hodnota = zasobnik.Pop();
        // Neni dalsi hodnota v zasobniku, nove pocatecni cislo
        if (hodnota == -1)
        {
            cout << "Prazdny zasobnik, chci nove pocatecni cislo ... " << endl;
            CndCas * rtp = new CndCas(0, NULL);
            zasobnik.Clear();
            return rtp;
        }
        else
        {
            zasobnik.SmazUzel();
            cout << "Stav zasobniku: " << endl;
            zasobnik.See(1);
            cout << endl;
            cout << "Graf nelze takto rozlozit, vracim se o uroven vyse ... " << endl << endl;
            return countMatr(matrix, hodnota, deep);
        }
    }
    for(unsigned int k=0;k<matrix[linc].length();k++){ // hledá se přechod do stavu, který má nejméně !!nejvice ne?!! přechodů do dalších stavů
        if(matrix[linc][k] == '1'){
            bool isIn = zasobnik.ObsahujeUzel(k); // hnusnej oneliner - pokud je vrchol v komponentě, vrátí true, jinak false
            if (linc != k && !isIn){ // pokud vrchol není ten, který se zrovna zkoumá a není v množině, potom ...
                cout << "Zpracovavam pozici: " << k;
                int mnt = PocetUzluMimo(matrix[k], zasobnik.vybrane_uzly, zasobnik.pocet_vybranych_uzlu);
                cout << " (pocet mimo uzlu: " << mnt << ")" << endl;
                zasobnik.Push(k,mnt);
            }
        }
    }
    cout << endl << "Stav zasobniku: " << endl;
    zasobnik.See(1);
    cout << endl;
    int next_value = zasobnik.Pop();
    zasobnik.UlozUzel(next_value);
    cout << "First best match uzel: " << next_value << endl << endl;
    //sleep(1); // jen pro debug
    if (deep + 1 == a){ // výpočet se vstupním a, tedy dokud nejsou aspoň a stavy v množině, hledá dál
        for (int i = 0; i < a; i++)
        {
            cout << zasobnik.vybrane_uzly[i];
            if (i != a-1)
            {
                cout << ",";
            }
            else
            {
                cout << endl;
            }
        }
        cout << endl;
        int hran = CelkovyPocetHranKomponent(matrix, zasobnik.vybrane_uzly);
        CndCas * rtp = new CndCas(hran, NULL);
        zasobnik.Clear();
        return rtp;
    }else{
        return countMatr(matrix, next_value, deep + 1); // předá se matice přechodu; nový vrchol; kde se bude hleda; počet přechodu; hloubka v grafu
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

    zasobnik.NastavMaximum((matrix->length() - 1) * (matrix->length() - 1));
    // Výpočet
    CndCas winner(0, NULL); // struktura, do který se uloží vítěz
    for(unsigned int j=0;j<stc;j++){ // pro každý vrchol otestuje, jeslti není v nejměnší komponentě
        cout << "==========" << endl;
        set <unsigned int> * str = new set<unsigned int>; // pro každý vrchol se vytvoří nová množina
        str->insert(j); // a strčí se do ní vrchol, kterým se začíná
        zasobnik.UlozUzel(j);
        CndCas * cnas = countMatr(matrix, j); // pošle se do výpočtu matici přechodů; vrchol u kterého začal; množinu pro už prochozené stavy
        if (cnas->cnt > winner.cnt){ // pokud je počet vazeb menší, než u vítěze, přiřadí jej k vítězi
            winner.cnt = cnas->cnt;
            winner.str = cnas->str;
        }else{
            delete cnas;
        }
    }
    cout << "Count win: " << winner.cnt << endl; // vypíše vítěze
    cout << "The End" << endl;
    /*set<unsigned int>::iterator iter;
    iter=winner.str->begin();
    while (iter != winner.str->end()){
        cout << *iter+1 << endl;
        iter++;
    }*/
    /*
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
    }*/
    // deleting
    delete [] matrix;
    delete winner.str;
    return 0;
}
