#define _CRT_SECURE_NO_WARNINGS
//do szybszego wywolywania funkcji, bez sprawdzania stack corruption( magic number)
//#pragma runtime_checks( "", off ) 

#include <iostream>
#include <cassert>

using namespace std;


int Partition(int* tab, int lewy, int prawy)
{
    int pivot = tab[lewy];//wybor pivota
    while (true)
    {
        while (tab[lewy] < pivot)
        {
            lewy++;
        }
        while (tab[prawy] > pivot)
        {
            prawy--;
        }
        if (lewy < prawy)
        {
            int pom = tab[lewy];
            tab[lewy] = tab[prawy];
            tab[prawy] = pom;
        }
        else
        {
            return prawy;
        }

        lewy = lewy + 1;
        prawy = prawy - 1;
    }
}

void Quick(int* tab, int lewy, int prawy)
{
    if (lewy < prawy)
    {
        int q = Partition(tab, lewy, prawy);
        Quick(tab, lewy, q);
        Quick(tab, q + 1, prawy);
    }
}

void punkt1WyswietlCiagStopniowy(int **tab,int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    int* tabIlosciSasiadow = new int[iloscNiepustychWierzcholkow +1];
    for (int i = 1; i <= iloscNiepustychWierzcholkow; i++)
    {
        tabIlosciSasiadow[i] = *tab[tabelaNiepustych[i]];
    }

    Quick(tabIlosciSasiadow, 1, iloscNiepustychWierzcholkow);

    for (int i = iloscNiepustychWierzcholkow; i >= 1; i--)
    {
        cout << tabIlosciSasiadow[i] << " ";
    }
    while (iloscWierzcholkow > iloscNiepustychWierzcholkow)
    {
        cout << "0 ";
        ++iloscNiepustychWierzcholkow;
    }
    cout << endl;
    delete[] tabIlosciSasiadow;
}

void odwiedzWierzcholki(int** tab, int nrWierzcholka, int* tabOdwiedzonychWierzcholkow)
{
    if (tabOdwiedzonychWierzcholkow[nrWierzcholka] > 0)
        return;
    tabOdwiedzonychWierzcholkow[nrWierzcholka] = 1;
    int* sasiedzi = tab[nrWierzcholka];
    for (int n = 0; n < sasiedzi[0]; ++n)
    {
        odwiedzWierzcholki(tab, sasiedzi[n + 1], tabOdwiedzonychWierzcholkow);
    }
}

void punkt2WyswietlIloscSkaldowychSpojnosci(int** tab, int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    int* tabOdwiedzonychWierzcholkow = new int[iloscWierzcholkow+1];
    memset(tabOdwiedzonychWierzcholkow, 0, sizeof(int) * (iloscWierzcholkow + 1));

    //od razu zwiekszam ilosc skladowych spojnosci, bo kazdy pusty wiercholek to dodatkowa skl. spojnosci
    int iloscSk = iloscWierzcholkow- iloscNiepustychWierzcholkow; 
    for (int wIdx = 1; wIdx <= iloscNiepustychWierzcholkow; ++wIdx)
    {
        int w = tabelaNiepustych[wIdx];
        if (tabOdwiedzonychWierzcholkow[w] == 0)
        {
            ++iloscSk;
            odwiedzWierzcholki(tab, w, tabOdwiedzonychWierzcholkow);
        }
    }
    cout << iloscSk << endl;

    delete[] tabOdwiedzonychWierzcholkow;
}

bool CzyGrafDwudzielnyW(int** tab, int wierzcholek, int wartoscWierzcholka, int *tabStron)
{
    // wartoscWierzcholka moze wynosic 1 lub 2
    //w tabStron jest wpisana wartosc :
    // 0 - jeszcze nie odwiedzony,  1 - w jednej czesci dwudzielnej,  2 - w drugiej czescie dwudzielnej
    if (tabStron[wierzcholek] == 0)
    {
        tabStron[wierzcholek] = wartoscWierzcholka;
    }
    else
    {
        if (tabStron[wierzcholek] != wartoscWierzcholka)
            return false;
        else
            return true;
    }
    for (int i = 1; i <= tab[wierzcholek][0]; i++)
    {            
        int nastWierzcholek = tab[wierzcholek][i];
        if (!CzyGrafDwudzielnyW(tab, nastWierzcholek, 3 - wartoscWierzcholka, tabStron))
        {
            return false;
        }
    }

    return true;//graf dwudzielny
}

bool punkt3CzyGrafDwudzielny(int** tab, int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    int* tabStron = new int[iloscWierzcholkow + 1];
    memset(tabStron, 0, sizeof(int) * (iloscWierzcholkow + 1));
    for (int wIdx = 1; wIdx <= iloscNiepustychWierzcholkow; ++wIdx)
    {
        int wierzch = tabelaNiepustych[wIdx];
        if (tabStron[wierzch] == 0 )
        {
            if (!CzyGrafDwudzielnyW(tab, wierzch, 1, tabStron))
                return false;
        }
    }
    delete[] tabStron;
    return true;
}

struct acent
{
    int wierzch;
    int glebokosc;
};

void dodajDoKolejki(acent* kolejka, acent* startPtr, acent** endPtr, int rozmiar, int w, int g)
{
    (*endPtr)->wierzch = w;
    (*endPtr)->glebokosc = g;
    ++(*endPtr); 
    if ((*endPtr) - kolejka >= rozmiar)
        (*endPtr) = kolejka;
    //jezeli zostanie przekroczony maksymalny rozmiar kolejki
    //to program wywali sie na assert
    assert(startPtr != (*endPtr));
}

void punkt4AcentrycznoscWierzcholkow(int** tab, int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    //najkrotsza droga do najdalszego punktu, BFS*
    const int rozmiarKolejki = 256 * 256;
    acent* kolejka = new acent[rozmiarKolejki];

    acent* startPtr = kolejka;
    acent* endPtr = kolejka;

    char* odwiedzone = new char[iloscWierzcholkow + 1];
    // badam acentrycznosc tylko dla wierzcholkow "niepustych"
    // wierzDruk sluzy do drukowania "0" dla "pustych" wierzcholkow, dla ktorych nie badam acentrycznosci
    int wierzDruk = 1; 
    for (int wIdx = 1; wIdx <= iloscNiepustychWierzcholkow; ++wIdx)
    {
        int poczWierz = tabelaNiepustych[wIdx];
        int maksGl = 0;
        memset(odwiedzone, 0, sizeof(char) * (iloscWierzcholkow + 1));

        dodajDoKolejki(kolejka, startPtr, &endPtr, rozmiarKolejki, poczWierz, 0);
        odwiedzone[poczWierz] = 1;

        while (startPtr != endPtr)
        {
            acent* w = startPtr;
            ++startPtr;
            if (startPtr - kolejka >= rozmiarKolejki)
            {
                startPtr = kolejka;
            }
            //bez tego warunku dzialalo wolniej
            if (w->glebokosc >= maksGl)
            {
                maksGl = w->glebokosc;
                int* ptr = tab[w->wierzch];
                // cnt zawiera ilosc sasiadow wierzcholka w
                int cnt = *ptr;
                ++ptr;
                // *ptr pokazuje na pierwszego sasiada wierzcholka w
                for (int n = 1; n <= cnt; ++n)
                {
                    if (!odwiedzone[*ptr])
                    {
                        if (tab[*ptr][0] > 0)
                        {
                            dodajDoKolejki(kolejka, startPtr, &endPtr, rozmiarKolejki, *ptr, w->glebokosc + 1);
                        }
                        odwiedzone[*ptr] = 1;
                    }
                    //ptr pokazuje na nastepnego sasiada wierzcholka w
                    ++ptr;
                }
            }
        }
        while (wierzDruk < poczWierz)
        {
            cout << "0 ";
            ++wierzDruk;
        }
        cout << maksGl << " ";
        ++wierzDruk;
    }
    while (wierzDruk <= iloscWierzcholkow)
    {
        cout << "0 ";
        ++wierzDruk;
    }
    cout << endl;
    delete[] odwiedzone;
    delete[] kolejka;

}

void printResult(int iloscWierzcholkow, int* result)
{
    //pierwszy wierzcholek w tabeli result pod ineksem pierwszym
    ++result;
    //do punktu 6a i 6b i 6c
    for (int w = 1; w <= iloscWierzcholkow; w++)
    {
        // jezeli w result jest zero, to znaczy, ze wierzcholek nie ma sasiadow i nie byl analizowany przez algorytm
        // i drukuje "1" - pierwszy kolor
        if (!*result)
            cout << "1 ";
        else
            cout << *result << " ";
        ++result;
    }
    cout << endl;
}

void punkt6aKolorowanieZachlanne(int** tab, int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    int* result = new int[iloscWierzcholkow +1];
    memset(result, 0, sizeof(int) * (iloscWierzcholkow +1));
    char* available = new char[iloscNiepustychWierzcholkow +1];

    for (int w = 1; w <= iloscNiepustychWierzcholkow; w++)
    {
        int wierzch = tabelaNiepustych[w];
        int* ptrSasiedzi = tab[wierzch];
        int cntSasiedzi = *ptrSasiedzi;
        ++ptrSasiedzi;
        if (result[wierzch] == 0)
        {
            memset(available, true, sizeof(char) * (iloscNiepustychWierzcholkow + 1));

            while (cntSasiedzi--)
            {
                int kolor = result[*ptrSasiedzi];
                if (kolor != 0)
                {
                    available[kolor] = false;
                }
                ++ptrSasiedzi;
            }

            for (int kolor = 1; kolor <= iloscWierzcholkow; kolor++) 
            {
                if (available[kolor])
                {
                    result[wierzch] = kolor;
                    break;
                }
            }
            
        }
    }

    printResult(iloscWierzcholkow, result);

    delete[] available;
    delete[] result;
}

void punkt6bKolorowanie_LF(int** tab, int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    int* result = new int[iloscWierzcholkow + 1];
    memset(result, 0, sizeof(int) * (iloscWierzcholkow + 1));

    char* kolorDostepny = new char[iloscWierzcholkow + 1];

    while (true)
    {
        //wybieram wierzcholek do kolorowania wedlug sortowania LF, czyli wierzcholek z najwieksza iloscia sasiadow     
        int wierzchDoKolorowania = -1;
        for (int n = 1; n <= iloscNiepustychWierzcholkow; ++n)
        {
            int wierzch = tabelaNiepustych[n];
            if (result[wierzch] == 0)
            {
                // jezeli wierzcholek jeszcze niepokolorowany
                if (wierzchDoKolorowania == -1)
                {
                    wierzchDoKolorowania = wierzch;
                }
                else
                {
                    if (tab[wierzch][0] > tab[wierzchDoKolorowania][0])
                    {
                        wierzchDoKolorowania = wierzch;
                    }
                }
            }
        }
        if (wierzchDoKolorowania == -1)
        {
            //wszystkie wierzcholki juz kolorowane
            break;
        }

        memset(kolorDostepny, 1, sizeof(char) * (iloscNiepustychWierzcholkow + 1));
        int* ptrSasiedzi = tab[wierzchDoKolorowania];
        int cntSasiedzi = *ptrSasiedzi;
        ++ptrSasiedzi;
        for (int n = 1; n <= cntSasiedzi; ++n)
        {
            int res = result[*ptrSasiedzi];
            if ( res != 0)
            {
                kolorDostepny[res] = 0;
            }
            ++ptrSasiedzi;
        }
        char* nowyKolor = kolorDostepny + 1;
        int nrKoloru = 1;
        while(true)
        {
            if (*nowyKolor)
                break;
            ++nowyKolor;
            ++nrKoloru;
        }
        result[wierzchDoKolorowania] = nrKoloru;
    }

    printResult(iloscWierzcholkow, result);

    delete[] kolorDostepny;
    delete[] result;
}

void punkt6cKolorowanie_SLF(int** tab, unsigned int iloscWierzcholkow, int* tabelaNiepustych, int iloscNiepustychWierzcholkow)
{
    int* saturationDegreeTable = new int[iloscWierzcholkow + 1];
    memset(saturationDegreeTable, 0, sizeof(int) * (iloscWierzcholkow + 1));
    
    int* result = new int[iloscWierzcholkow + 1];
    memset(result, 0, sizeof(int) * (iloscWierzcholkow + 1));
    
    char** dostepneKolory = new char* [iloscWierzcholkow + 1];

    for (int n = 1; n <= iloscNiepustychWierzcholkow; ++n)
    {
        //pamiec alokuje tylko dla wierzcholkow, ktore maja sasiadow
        dostepneKolory[tabelaNiepustych[n]] = new char[iloscNiepustychWierzcholkow + 1];
        memset(dostepneKolory[tabelaNiepustych[n]], 0, sizeof(unsigned char) * (iloscNiepustychWierzcholkow + 1));
    }

    while(true)
    {
        //wybieram wierzcholek do kolorowania wedlug sortowania SLF        
        int wierzchDoKolorowania = -1;
        for (int n = 1; n <= iloscNiepustychWierzcholkow; ++n)
        {            
            int wierzch = tabelaNiepustych[n];
            if (result[wierzch] == 0)
            {
                // jezeli wierzcholek jeszcze niepokolorowany
                if (wierzchDoKolorowania == -1)
                {
                    wierzchDoKolorowania = wierzch;
                }
                else
                {
                    if (saturationDegreeTable[wierzch] > saturationDegreeTable[wierzchDoKolorowania])
                    {
                        wierzchDoKolorowania = wierzch;
                    }
                    else
                    {
                        if (saturationDegreeTable[wierzch] == saturationDegreeTable[wierzchDoKolorowania] && tab[wierzch][0] > tab[wierzchDoKolorowania][0])
                        {
                            wierzchDoKolorowania = wierzch;
                        }
                    }
                }
            }
        }
        if (wierzchDoKolorowania == -1)
        {
            //wszystkie wierzcholki juz kolorowane
            break;
        }

        unsigned int nowyKolor = 0;
        for (unsigned int kolor = 1; kolor <= iloscWierzcholkow; kolor++) {
            if ((!dostepneKolory[wierzchDoKolorowania][kolor]))
            {                
                nowyKolor = kolor;
                break;
            }
        }
        int* ptrSasiedzi = tab[wierzchDoKolorowania];
        int cntSasiedzi = *ptrSasiedzi;
        ++ptrSasiedzi;
        //sprawdzam wszystkich sasiadow, czy kolor "nowyKolor" jest ju¿ gdzies ustawiony jako niedostepny
        //jezeli wszedzie jest dostepny, trzeba zmienic saturation degree
        for (int n = 1; n <= cntSasiedzi; ++n)
        {            
            //aktualizue saturationDegree tylko gdy kolor wierzcholka nie jest jeszcze ustawiony
            if (result[*ptrSasiedzi] == 0)
            {
                if (!dostepneKolory[*ptrSasiedzi][nowyKolor])
                {
                    saturationDegreeTable[*ptrSasiedzi]++;
                    dostepneKolory[*ptrSasiedzi][nowyKolor] = 1;
                }
            }
            ++ptrSasiedzi;
        }
        //dopiero teraz moge ustawic kolor jako niedostepny, bo inaczej nie dzialalaby wczesniejsza aktualizacja saturationDegree
        result[wierzchDoKolorowania] = nowyKolor;
    }

    printResult(iloscWierzcholkow, result);

    for (unsigned int n = 1; n <= iloscWierzcholkow; ++n)
    {
        if (tab[n][0])
            delete[] dostepneKolory[n];
    }
    delete[] dostepneKolory;
    delete[] result;
    delete[] saturationDegreeTable;
}

long long punkt8IloscKrawedziDopelnieniaGrafu(int iloscWierz, int iloscKrawedzi)
{
    long long ilPelnegoGrafu = (long long)iloscWierz * (iloscWierz - 1) / 2;

    return ilPelnegoGrafu - iloscKrawedzi/2; //zalozenie ze graf jest nieskierowny inaczej bylby wiekszy problem z iloscia krawdzi grafu
}

int main()
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int liczbaGrafow;

    scanf("%d",&liczbaGrafow);
    
    while (liczbaGrafow--)
    {
        int iloscWierzcholkow, iloscSasiadow = -1;

        int iloscKrawedzi = 0;

        scanf("%d",&iloscWierzcholkow);
        //deklaracja dynamicznej tablicy, ktora bedzie pokazaywac na dane miejsce w danej tablicy sasiadow
        //pierwszy element tablicy jest nieuzywany, zrobione do, zeby nie trzeba bylo wszedzie odejmowac,dodawac 1 
        int** tab = new int* [iloscWierzcholkow+1];
        for (int i = 1; i <= iloscWierzcholkow; i++)
        {
            scanf("%d", &iloscSasiadow);
            int* sasiedzi = new int[iloscSasiadow + 1];//w tablicy sasiedzi 0 indeks przechowuje ilosc sasiadow danego punktu
            *sasiedzi = iloscSasiadow;

            iloscKrawedzi += iloscSasiadow;

            tab[i] = sasiedzi;
            for (int j = 1; j <= iloscSasiadow; j++)
            {
                sasiedzi[j] = 0;
                scanf("%d", &sasiedzi[j]);
            }
        }

        //wypelnienie tabeli niepustych wierzcholkow, uzywam jej do kilku podpunktow
        int* tabelaNiepustych = new int[iloscWierzcholkow + 1];
        int iloscNiepustychWierzcholkow = 0;

        for (int wierzch = 1; wierzch <= iloscWierzcholkow; ++wierzch)
        {
            if (tab[wierzch][0])
            {
                tabelaNiepustych[++iloscNiepustychWierzcholkow] = wierzch;
            }
        }

        //PODPUNKT 1-ciag stopniowy
        punkt1WyswietlCiagStopniowy(tab,iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow);
        //cout << "?" << endl;
        
        //PODPUNKT 2-liczba skladowych spojnosci
        punkt2WyswietlIloscSkaldowychSpojnosci(tab, iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow);
        //cout << "?" << endl;
        
        //PODPUNKT 3-        
        cout << (punkt3CzyGrafDwudzielny(tab, iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow) ? "T" : "F") << endl;
        //cout << "?" << endl;

        //PODPUNKT 4-
        punkt4AcentrycznoscWierzcholkow(tab, iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow);
        //cout << "?" << endl;

        ///PODPUNKT 5-
        cout << "?" << endl;

        //PODPUNKT 6(a)-
        punkt6aKolorowanieZachlanne(tab, iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow);
        //cout << "?" << endl;

        //PODPUNKT 6(b)-
        punkt6bKolorowanie_LF(tab, iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow);
        //cout << "?" << endl;

        //PODPUNKT 6(c)-
        punkt6cKolorowanie_SLF(tab, iloscWierzcholkow, tabelaNiepustych, iloscNiepustychWierzcholkow);
        //cout << "?" << endl;        

        //PODPUNKT 7-liczba ró¿nych podgrafów      
        cout << "?" << endl;

        delete[] tabelaNiepustych;
        
        //PODPUNKT 8-liczba krawêdzi dope³nienia grafu
        cout << punkt8IloscKrawedziDopelnieniaGrafu(iloscWierzcholkow, iloscKrawedzi) << endl;  
        //cout << "?" << endl;      

        for (int i = 1; i < iloscWierzcholkow; i++)//do usuwania sasiadow
        {
            delete[] tab[i];
        }
        delete[] tab;
    }
    
}

