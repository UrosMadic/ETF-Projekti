#include <iostream>
#include <stack>
#include <queue>
#include <string>
#define MAX_ODG 10

using namespace std;
//struct Elem {moralo bi van klase da bi moglo da se radi sa funkcijama nadji odgovor i pitanje
//    string pit_odg_kom;
//    //int ocena;
//    int* niz_ocena = new int[MAX_ODG];
//    Elem* sledpok;
//    Elem** niz_odg = new Elem * [MAX_ODG];
//    Elem* preth;
//    Elem(string& pok, Elem* spok = nullptr, Elem* p = nullptr) : pit_odg_kom(pok), sledpok(spok), preth(p) {
//        for (int i = 0; i < 10; i++) {
//            niz_odg[i] = nullptr;
//            niz_ocena[i] = 0;
//        }
//    }
//};
class ListaPitOdgKom {
    struct Elem {
        string pit_odg_kom;
        int* niz_ocena = new int[MAX_ODG];
        Elem* sledpok;
        Elem** niz_odg = new Elem * [MAX_ODG];
        Elem* preth;
        Elem(string& pok, Elem* spok = nullptr, Elem* p = nullptr) : pit_odg_kom(pok), sledpok(spok), preth(p) {
            for (int i = 0; i < 10; i++) {
                niz_odg[i] = nullptr;
                niz_ocena[i] = 0;
            }
        }
    };
    Elem* prvi, * posl;
    void kopiraj(const ListaPitOdgKom& lpok) {
        prvi = posl = nullptr;
        queue<Elem*> q1;
        queue<Elem*> q2;
        for (Elem* tek = lpok.prvi; tek; tek = tek->sledpok) {
            posl = (!prvi ? prvi : posl->sledpok) = new Elem(tek->pit_odg_kom);
            Elem* next = tek;
            q1.push(next);
            Elem* next1 = posl;
            q2.push(next1);
            while (!q1.empty()) {
                next = q1.front();
                q1.pop();
                next1 = q2.front();
                q2.pop();

                for (int i = 0; i < MAX_ODG; i++) {
                    if (next->niz_odg[i] != nullptr) {
                        next1->niz_odg[i] = new Elem(next->niz_odg[i]->pit_odg_kom);
                        next1->niz_ocena[i] = next->niz_ocena[i];
                        next1->niz_odg[i]->preth = next1;
                    }
                }
                for (int i = 0; i < MAX_ODG; i++) {
                    if (next->niz_odg[i] != nullptr) {
                        q1.push(next->niz_odg[i]);
                        q2.push(next1->niz_odg[i]);
                    }
                }
            }
        }
    }
    void premesti(ListaPitOdgKom& lpok) {
        prvi = lpok.prvi;
        posl = lpok.posl;
        lpok.prvi = nullptr;
        lpok.posl = nullptr;
    }
    void brisi() {
        queue<Elem*> q1;
        while (prvi) {
            Elem* next = prvi;
            prvi = prvi->sledpok;
            q1.push(next);
            while (!q1.empty()) {
                next = q1.front();
                q1.pop();
                for (int i = 0; i < MAX_ODG; i++) {
                    if (next->niz_odg[i] != nullptr) {
                        q1.push(next->niz_odg[i]);
                    }
                }
                delete next;
            }
        }
    }
public:
    ListaPitOdgKom() { prvi = posl = nullptr; }
    ListaPitOdgKom(const ListaPitOdgKom& lpok) { kopiraj(lpok); }
    ListaPitOdgKom(ListaPitOdgKom&& lpok) { premesti(lpok); }
    ~ListaPitOdgKom() { brisi(); }
    ListaPitOdgKom& operator=(const ListaPitOdgKom& lpok) {
        if (this != &lpok) {
            brisi();
            kopiraj(lpok);
        }
        return *this;
    }
    ListaPitOdgKom& operator=(ListaPitOdgKom&& lpok) {
        if (this != &lpok) {
            brisi();
            premesti(lpok);
        }
        return *this;
    }

    ListaPitOdgKom& DodajPitanje(string pitanje) {
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            if (tek->pit_odg_kom != pitanje) {
                continue;
            }
            else {
                cout << "Uneto pitanje vec postoji u listi" << endl;
                return *this;
            }
        }
        posl = (!prvi ? prvi : posl->sledpok) = new Elem(pitanje);
        return *this;
    }

    ListaPitOdgKom& DodajOdgovor(string pitanje_odgovor, string odgovor) {
        queue<Elem*> q1;
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            Elem* next = tek;
            q1.push(next);
            while (!q1.empty()) {
                next = q1.front();
                q1.pop();
                if (next->pit_odg_kom != pitanje_odgovor) {
                    for (int i = 0; i < MAX_ODG; i++) {
                        if (next->niz_odg[i] != nullptr) {
                            q1.push(next->niz_odg[i]);
                        }
                    }
                }
                else {
                    int i = 0;
                    for (; i < MAX_ODG; i++) {
                        if (next->niz_odg[i] == nullptr) {
                            next->niz_odg[i] = new Elem(odgovor);
                            next->niz_odg[i]->preth = next;
                            break;
                        }
                    }
                    if (i == 10) {
                        cout << "Unesen je veci broj odgovora nego sto treba!";
                        exit(1);
                    }
                }
            }
        }
        return *this;
    }

    Elem* pretraziPitanje(string pitanje)const {
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            if (tek->pit_odg_kom == pitanje) {
                return tek;
            }
        }
        cout << "Ne postoji pitanje!"<<endl;
        exit(1);
    }

    Elem* pretraziOdgovor(string pitanje, string odgovor)const {
        queue<Elem*> q;
        Elem* next = pretraziPitanje(pitanje);
        q.push(next);
        while (!q.empty()) {
            next = q.front();
            q.pop();
            for (int i = 0; i < MAX_ODG; i++) {
                if (next->niz_odg[i] != nullptr) {
                    if (next->niz_odg[i]->pit_odg_kom == odgovor) {
                        return next->niz_odg[i];
                    }
                    q.push(next->niz_odg[i]);
                }
            }
        }
        cout << "Ne postoji odgovor!" << endl;
        exit(1);
    }

    ListaPitOdgKom& povecanjeOcene(string pitanje, string odgovor, int i) {
        Elem* tek = pretraziOdgovor(pitanje, odgovor)->preth;
        int j = 0;
        for (; j < MAX_ODG; j++) {
            if (tek->niz_odg[j] == pretraziOdgovor(pitanje, odgovor)) {
                tek->niz_ocena[j] += i;
                break;
            }
        }
        return *this;
    }

    ListaPitOdgKom& uredjivanjeOdgovora() {
        queue<Elem*> q;
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            Elem* next = tek;
            q.push(next);
            while (!q.empty()) {
                next = q.front();
                q.pop();
                for (int i = 0; i < MAX_ODG - 1; i++) {
                    for (int j = 0; j < MAX_ODG - i - 1; j++) {
                        if (next->niz_odg[j] != nullptr && next->niz_odg[j + 1] != nullptr) {
                            if (next->niz_ocena[j] < next->niz_ocena[j + 1]) {
                                Elem* tmp1 = next->niz_odg[j];
                                int tmp2 = next->niz_ocena[j];
                                next->niz_odg[j] = next->niz_odg[j + 1];
                                next->niz_ocena[j] = next->niz_ocena[j + 1];
                                next->niz_odg[j + 1] = tmp1;
                                next->niz_ocena[j + 1] = tmp2;
                            }
                        }
                        else if (next->niz_odg[j + 1] != nullptr) {
                            Elem* tmp = next->niz_odg[j];
                            next->niz_odg[j] = next->niz_odg[j + 1];
                            next->niz_odg[j + 1] = tmp;
                        }
                    }
                }
                for (int i = 0; i < MAX_ODG; i++) {
                    if (next->niz_odg[i] != nullptr) {
                        q.push(next->niz_odg[i]);
                    }
                }
            }
        }
        return *this;
    }

    void ispisSvihPitanja()const {
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            cout << tek->pit_odg_kom << endl;
        }
    }

    void ispis(string pitanje)const {
        queue<Elem*> q1;
        Elem* tek = pretraziPitanje(pitanje);
        cout << tek->pit_odg_kom << endl;
        Elem* next = tek;
        q1.push(next);
        q1.push(nullptr);
        while (q1.front() != nullptr) {
            next = q1.front();
            q1.pop();
            for (int i = 0; i < MAX_ODG; i++) {
                if (next->niz_odg[i] != nullptr) {
                    cout << "(";
                    cout << next->niz_odg[i]->pit_odg_kom << " ";
                    cout << next->niz_ocena[i] << "; ";
                    cout << next->pit_odg_kom << ")";
                    cout << " ";
                    q1.push(next->niz_odg[i]);
                }
            }
            if (q1.front() == nullptr) {
                cout << endl;
                q1.push(nullptr);
                q1.pop();
            }
        }
        q1.pop();
    }

    ListaPitOdgKom& brisiOdgovor(string pitanje, string odgovor) {
        Elem* tek = pretraziOdgovor(pitanje, odgovor);
        queue<Elem*> q1;
        Elem* next = tek;
        q1.push(next);
        while (!q1.empty()) {
            next = q1.front();
            q1.pop();
            for (int i = 0; i < MAX_ODG; i++) {
                if (next->niz_odg[i] != nullptr) {
                    q1.push(next->niz_odg[i]);
                }
            }
            if (next->pit_odg_kom == odgovor) {
                Elem* pom = next->preth;
                for (int i = 0; i < MAX_ODG; i++) {
                    if (pom->niz_odg[i] == next) {
                        pom->niz_ocena[i] = 0;
                        pom->niz_odg[i] = nullptr;
                        break;
                    }
                }
            }
            delete next;
        }
        return *this;
    }
    ListaPitOdgKom& brisiPitanje(string pitanje) {
        queue<Elem*>q;
        Elem* next;
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            if (tek->sledpok->pit_odg_kom == pitanje) {
                next = tek->sledpok;
                q.push(next);
                tek->sledpok = tek->sledpok->sledpok;
                break;
            }
            else if (prvi->pit_odg_kom == pitanje) {
                next = prvi;
                q.push(next);
                prvi = prvi->sledpok;
                break;
            }
        }
        while (!q.empty()) {
            next = q.front();
            q.pop();
            for (int i = 0; i < MAX_ODG; i++) {
                if (next->niz_odg[i] != nullptr) {
                    q.push(next->niz_odg[i]);
                }
            }
            delete next;
        }
        return*this;
    }

    Elem* pitanjeSaOdgSaNajvisomOcenom()const {
        int max = 0;
        queue<Elem*>q;
        Elem* max_pitanje;
        for (Elem* tek = prvi; tek; tek = tek->sledpok) {
            Elem* next = tek;
            q.push(next);
            while (!q.empty()) {
                next = q.front();
                q.pop();
                for (int i = 0; i < MAX_ODG; i++) {
                    if (next->niz_odg[i] != nullptr) {
                        if (next->niz_ocena[i] > max) {
                            max = next->niz_ocena[i];
                            max_pitanje = tek;
                        }
                        q.push(next->niz_odg[i]);
                    }
                }
            }
        }
        return max_pitanje;
    }
};

int main() {
    
    ListaPitOdgKom lp;
    string pitanje;
    string pitanje_odgovor;
    string odgovor;
    string pom;
    int i;
    int oc;
    
    while (true) {
        cout << endl;
        cout << "0. Kraj programa" << endl;
        cout << "1. Dodajte novo pitanje." << endl;
        cout << "2. Dodavanje odgovora na pitanje ili odgovor." << endl;
        cout << "3. Pretraga pitanja." << endl;
        cout << "4. Pretraga odgovora." << endl;
        cout << "5. Povecanje ocene nekog odgovora." << endl;
        cout << "6. Uredjivanje odgovora prema njihovim ocenama." << endl;
        cout << "7. Ispis svih pitanja." << endl;
        cout << "8. Ispis pitanja i njegovih odgovora." << endl;
        cout << "9. Brisanje odgovora i svih njegovih potomaka." << endl;
        cout << "10. Brisanje pitanja i svih njegovih potomaka." << endl;
        cout << "11. Pronalazak pitanja ciji odgovor ili komentar imaju najvisu ocenu" << endl;
        cout << "Unesite broj za izbor operacije: ";
        cin >> i;
        cout << endl;
        if (i == 0) { break; }
        else if (i == 1) {
            getline(cin, pom);
            cout << "Unesite pitanje koje dodajete: " << endl;
            cout << "Pitanje: ";
            getline(cin, pitanje);
            lp.DodajPitanje(pitanje);
        }
        else if (i == 2) {
            getline(cin, pom);
            cout << "Unesite pitanje ili odgovor na koji dodajete odgovor kao taj odgovor: " << endl;
            cout << "Pitane-odgovor: ";
            getline(cin, pitanje_odgovor);
            cout << "Odgovr: ";
            getline(cin, odgovor);
            lp.DodajOdgovor(pitanje_odgovor, odgovor);
        }
        else if (i == 3) { continue; }
        else if (i == 4) { continue; }
        else if (i == 5) {
            getline(cin, pom);
            cout << "Unesite pitanje u kojem ce se traziti unet odgovor kojem ce se dodati uneta ocena: " << endl;
            cout << "Pitanje: ";
            getline(cin, pitanje);
            cout << "Odgovor: ";
            getline(cin, odgovor);
            cout << "Ocena: ";
            cin >> oc;
            lp.povecanjeOcene(pitanje, odgovor, oc);
        }
        else if (i == 6) {
            lp.uredjivanjeOdgovora();
        }
        else if (i == 7) {
            lp.ispisSvihPitanja();
        }
        else if (i == 8) {
            getline(cin, pom);
            cout << "Unesite pitanje koje ce se ispisati sa njegovim odgovorima :" << endl;
            cout << "Pitanje: ";
            getline(cin, pitanje);
            lp.ispis(pitanje);
        }
        else if (i == 9) {
            getline(cin, pom);
            cout << "Unesite pitanje u kojem ce se brisati unet odgovor i svi njegovi potomci : " << endl;
            cout << "Pitanje: ";
            getline(cin, pitanje);
            cout << "Odgovor: ";
            getline(cin, odgovor);
            lp.brisiOdgovor(pitanje, odgovor);
        }
        else if (i == 10) {
            getline(cin, pom);
            cout << "Unesite pitanje u kojem ce se brisati on i svi njegovi potomci : " << endl;
            cout << "Pitanje: ";
            getline(cin, pitanje);
            lp.brisiPitanje(pitanje);
        }
        else if (i == 1) {
            continue;
        }
        else {
            cout << "Izabrali ste nepostojecu operaciju" << endl;
            continue;
        }
    }
    return 0;
}

