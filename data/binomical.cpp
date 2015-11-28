#include <iostream>
#include <stdio.h>
#include <ctype.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include <set>
#include <unistd.h>
#include <climits>
#include "mpi.h"

#define LENGTH 100


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
unsigned int relCntMin = UINT_MAX;
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


// Debug na vypsani posilanych zprav
void PrintMessage(char * message)
{
	for (int i = 0; i < LENGTH;i++)
	{
		if (message[i] != '\0')
		{
			cout << (int)message[i] << ", ";
		}
	}
	cout << endl;
}

// Zaslani casti sve prace
void SendMoreWork(char * message, int rec_proc)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Send (message,LENGTH, MPI_CHAR, rec_proc, 100, MPI_COMM_WORLD);
	//cout << "Running Proces (" << my_rank << ") sending work for (" << rec_proc << ") = ";
	//PrintMessage(message);
}

// Proces 0 rika ostatni procesum, ze uz je konec
void SendEndOfJob(int rec_proc)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int message = 0;
	MPI_Send (&message, 1, MPI_INT, rec_proc, 600, MPI_COMM_WORLD);
	//cout << "Passive Proces (" << my_rank << ") sended token: " << message << " to (" << rec_proc << ")" << endl;
}

// Dcerine procesy (>0) zasilaji sovje lokalni vyslekdy
void SendMinimum(unsigned int local_minimum)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	unsigned int message = local_minimum;
	MPI_Send (&message, 1, MPI_UNSIGNED, 0, 600, MPI_COMM_WORLD);
	cout << "Passive Proces (" << my_rank << ") sended local minimum: " << local_minimum << endl;	
}

//Kolujici token urcujici, zda zbyva nejaka prace
void SendToken(int rec_proc, int token)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int message = token;
	MPI_Send (&message, 1, MPI_INT, rec_proc, 500, MPI_COMM_WORLD);
	//cout << "Passive Proces (" << my_rank << ") sended token: " << message << " to (" << rec_proc << ")" << endl;
}

// Request o praci
void AskForWork(int rec_proc)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int message = 0;
	MPI_Send (&message, 1, MPI_INT, rec_proc, 200, MPI_COMM_WORLD);
	//cout << "Waiting Proces (" << my_rank << ") asking for work proces (" << rec_proc << ")" << endl;
}

// Zaslani zpravy, ze nemam jiz praci
void SendNoMoreWork(int rec_proc)
{
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	int message = 0;
	MPI_Send (&message, 1, MPI_INT, rec_proc, 300, MPI_COMM_WORLD);
	//cout << "Running Proces (" << my_rank << ") has no more work for (" << rec_proc << ")" << endl;
}

// Pretvoreni mnoziny do char bufferu
char * DefiningWork(set <unsigned int> * str, int current_node)
{
	char * message = new char[LENGTH];
	set<unsigned int>::iterator iter;
    iter=str->begin();
	int counter = 0;
    while (iter != str->end())
	{
		message[counter] = *iter + '0';
		counter++;
		iter++;
	}
	message[counter] = current_node + '0';
	for (int i = counter+1; i < LENGTH;i++)
	{
		message[i] = '\0';
	}
	return message;
}


void redStack(unsigned int stc, string * matrix){
    set<unsigned int>::iterator iter; // iterátor množiny
	int my_rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
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
		//cout << "Running Proces (" << my_rank << ") working on: ";
        while (iter != stx.end()){
			//cout << *iter << ", ";
            for (unsigned int j=0;j<stc;j++){
                if(matrix[*iter][j] == '1' && stx.count(j) == 0)
                    relCnt ++;
            }
            iter++;
        }
		//cout << endl;
        if (relCnt < relCntMin){ // zvětší maxima
            relCntMin = relCnt;
            stxMin = stx;
            styMin = sty;
        }
    }
}

// recursion_cycle = # rekurzi, kdy se nekontrolovali zpravy, a_start = pocatecni hloubka mnoziny a 
void gena(set <unsigned int> * str,unsigned int i,unsigned int stc,string * matrix, int recursion_cycle, unsigned int a_start){
	int flag, my_rank, process_count;
    MPI_Status status;
	MPI_Status recv_status;
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
    if (str->size() == a) // pokud je velikosti množiny a, tak jej přidá na zásobník
	{ 
        wrkStc.push(str); // a-tici nakopíruje do zásobníku na práci
        redStack(stc,matrix);
        return; // vygenerovaná a-tice, vrací se
    }	
	else 
	{
		for(unsigned int j=i+1;j<stc;j++)
		{
			if (recursion_cycle > 100 && str->size() > 0 && str->size() + 1 < a)
			{
				//cout << "Running Proces (" << my_rank << ") checking messages" << endl;
				// Nuluji pocitac rekurzi, kdy nebylo kontrolovano
				recursion_cycle = -1;
				// Prochazim zpravy od vsech procesu
				for (int process = 0; process < process_count; process++)
				{
					// Overuji zda si neposilam sam sobe zpravy
					if (process != my_rank)
					{
						// Overeni zda prisla zprava s tagem 200 = work request
						MPI_Iprobe(process, 200, MPI_COMM_WORLD, &flag, &status);
						if (flag)
						{
							// Zprava prisla, zpracovavam
							int message;
							MPI_Recv(&message, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &recv_status);
							//cout << "Running Proces (" << my_rank << ") got request for job from ("  << process << ")" << endl;
							if (j+1 < stc)
							{
								// Tvorba bufferu ze zpravou - zkomibuji co je v str + aktualni node 'j'
								char * send_message = DefiningWork(str, j);
								// Zaslani zpravy
								SendMoreWork(send_message, process);
								delete send_message;
								j = j+1;
							}
							// Nema cenu nic zasilat, zaslu zpravu ze neni co poslat
							else
							{
								SendNoMoreWork(process);
							}
						}
					}
				}
			}
			str->insert(j); // přidá další hranu
			gena(str,j,stc,matrix,++recursion_cycle, a_start); // pošle rekurzivně s novou hranou
			if (str->size() > a_start)
			{
				str->erase(j); // odejme přidanou hranu, aby mohl přidat další, větší
			}
			
		}
    }
}

void genAComp(unsigned int start_note, unsigned int stc,string * matrix){ // pro každý vrchol vygeneruje množinu |a| vrcholů
    set <unsigned int> * str = new set<unsigned int>;
    str->insert(start_note); // přidá první vrchol do množiny
    gena(str,start_note,stc,matrix, 0, 0); // přidá další vrcholy do velikosti |a|
    delete str; // množina se nakopírovala pro vložení na zásobník, může se smazat
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

    int process_count;
    int my_rank;
     /* start up MPI */
    MPI_Init( &argc, &argv );
	double start = MPI_Wtime();
    /* find out process rank */
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    /* find out number of processes */
    MPI_Comm_size(MPI_COMM_WORLD, &process_count);
	MPI_Status status;
	MPI_Status recv_status;
	if (my_rank == 0)
	{
		cout << "a = " << a << endl;
		for(unsigned int j=0;j<stc;j++){
			cout << matrix[j] << endl;
		}
	}
    // Každému procesu rozdělím rovnoměrně počáteční strom (každý bude ale jinak velký)
    for (unsigned int i = 0; i < stc; i++)
    {
        if ((unsigned int)my_rank == (i % process_count))
        {
            // a-tcie tvořím z počáteční hrany i, kterou má každý proces jinou
            genAComp(i,stc,matrix);           
        }
    }
    
    // Přidělování další práce
	unsigned int msg;
	int token = 0;
	int flag;
	bool SendWorkRequest = true;
	
	// Po skonceni prace procesu 0 necham obihat token
	if (my_rank == 0)
	{
		SendToken((my_rank + 1) % process_count, token);
	}
	
	// Nekonecna smycka pro procesy
	for (int process = (my_rank + 1) % process_count;process < process_count;) 
	{
		if (my_rank == process && process_count != 1)
		{
			process = (process + 1) % process_count;
			continue;
		}
		// Zazadam o praci (pouze jednou - to hlida ten bool)
		if (SendWorkRequest && process_count != 1)
		{
			AskForWork(process);
			SendWorkRequest = false;
		}
		MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
		if (flag) 
		{
			// Rozdeleni reakce na prichozi zpravu podle tagu
			if (status.MPI_TAG == 100)  // Prisla prace
			{
				char * message = new char[LENGTH];
				MPI_Recv(message, LENGTH, MPI_CHAR, status.MPI_SOURCE, status.MPI_TAG, MPI_COMM_WORLD, &recv_status);
				//cout << "Proces (" << my_rank << ") started working again" << endl;
				set <unsigned int> * str = new set<unsigned int>;
				unsigned int a_start = 0;
				for (int i = 0; i < LENGTH;i++)
				{
					if (message[i] != '\0')
					{
						str->insert(message[i] - '0'); // pridam vrchol do nove mnoziny
					}				
					else
					{
						// smazu posledni uzel a dam ho funkci gena jako parametr
						str->erase(message[i-1]);
						// pocatecni velikost a, pod kterou nesmim jit (jinak by se ta prace delala 2x)
						a_start = i;
						break;
					}
				}			
				gena(str,(message[a_start-1] - '0'),stc,matrix, 0, a_start);
				delete str; // množina se nakopírovala pro vložení na zásobník, může se smazat
				delete message;	
				SendWorkRequest = true;
				token = 1;
			}
			else if (status.MPI_TAG == 200) // Prisla zadost o praci
			{
				MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG , MPI_COMM_WORLD, &recv_status);
				SendNoMoreWork(recv_status.MPI_SOURCE);
			}
			else if (status.MPI_TAG == 300) // Prislo upozorneni od porcesu, ze nema praci
			{
				MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG , MPI_COMM_WORLD, &recv_status);
				process = (process + 1) % process_count;
				SendWorkRequest = true;
			}
			else if (status.MPI_TAG == 500) // Prisla zprava s tokenem
			{
				MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG , MPI_COMM_WORLD, &recv_status);
				// Token udelal koelcko s procesory a vratil se bily = proces 0 konci
				if (my_rank == 0 && msg == 0) break;
				// Poslu token dal
				else SendToken((my_rank + 1) % process_count, token);
				token = 0;
			}
			else if (status.MPI_TAG == 600) // Prisla zprava z zadosti o poslani lokalniho minima - konci ostatni procesy
			{
				MPI_Recv(&msg, 1, MPI_INT, status.MPI_SOURCE, status.MPI_TAG , MPI_COMM_WORLD, &recv_status);
				break;
			}
		}
	}
			
	cout << "Proces (" << my_rank << ") finished" << endl;
	// Sber lokalniho minima
	if (my_rank == 0)
	{
		unsigned int global_min = relCntMin;
		for (int p = 1; p < process_count; p++)
		{
			SendEndOfJob(p); // Zaslani zpravy s tagem = 600
		}
		for (int p = 1; p < process_count; p++)
		{
			MPI_Probe(p, 600, MPI_COMM_WORLD, &status); // Cekam na zpravy od ostatnich procesu s lokalni minimy
			if (flag) 
			{
				MPI_Recv(&msg, 1, MPI_UNSIGNED, status.MPI_SOURCE, status.MPI_TAG , MPI_COMM_WORLD, &recv_status);
				cout << "Proces (" << p << ") local minimum: " << msg << endl;
				if (msg < global_min)
				{
					global_min = msg;
				}
			}
		}
		cout << "Result relations: " << global_min << endl;
	}
	else
	{
		// Zaslani minima procesy 1 - p-1
		SendMinimum(relCntMin);
	}
	double end = MPI_Wtime();
	if (my_rank == 0)
	{
		cout << end-start << endl;
	}
    delete [] matrix;
	MPI_Finalize();
    return 0;
}