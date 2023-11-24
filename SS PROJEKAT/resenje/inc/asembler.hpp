#include <iostream>
#include <map>
#include <vector>
#include "RelocationTableEntry.hpp"
#include "SymbolTableEntry.hpp"
#include "LiteralTableEntry.hpp"
#include "MashCodeOfSectionsEntry.hpp"
#include <exception>
#include <fstream>

using namespace std;

class MultipleDefOfSect:public exception {
  public:

	  virtual const char* what() const throw () {
		  return "Greska: Visestruka definicija sekcije!\n";
	  }
};

class MultipleDefOfLab:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Visestruka definicija labele!\n";
	  }
};
class NonDefinedGlobSymb:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Nije definisan globalni simbol!\n";
	  }
};

class NoDefSymb:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Konacna vrednost simbola nije poznata u trenutku asembliranja!\n";
	  }
};
class NoSymbDef:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Ne postoji definicija simbola!\n";
	  }
};

class LitteralSize:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Prekoracenje vrednosti literala!\n";
	  }
};

class WrongInstruction:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Nemoguce izvrsiti instrukciju!\n";
	  }
};

class NoSection:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Naredba se ne nalazi u sekciji!\n";
	  }
};

class DefOfSymbol:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Uvezen simbol je definisan u modulu!\n";
	  }
};

class BigSectionSize:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Prekoracena velicina sekcije. Ogranicena na 4KB!\n";
	  }
};

class GlobalSection:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Pokusaj proglasavanja simbola sekcije za globalan simbol!\n";
	  }
};


 class Asembler{
    static int prolaz;
    static int locationCounter;
    static int numSymbTab;
    static string currSection;
 
    

    static map<string,SymbolTableEntry*>SymbolTable;//naziv simbola kljuc ostali atributi je ulaz simbola

    static vector<LiteralTableEntry*>LiteralTable;
    static vector<RelocationTableEntry*>RelocationTable;
    static vector<MashCodeOfSectionsEntry*>MashCodeOfSections;

  public:
    Asembler();
    Asembler(int argc, char *argv[]);
    ~Asembler();


    void writeOutputFile(char* outputFile);
    void insertSectionSize();
    void insertLastSectionLocations();
    void insertLastMachCode();
    void correctMashCode();


    void llabel(char* labela);
    void gglobal(char* symbol);
    void eextern(char* symbol);
    void ssection(char* section);
    void wword(char* symbol);
    void wword(int literal);
    void sskip(int literal);
    void eend();
    void hhalt();
    void iint();
    void iiret();
    void ccall(char* symbol);
    void ccall(int literal);
    void rret();
    void jjmp(char* symbol);
    void jjmp(int literal);
    void bbeq(int r1,int r2,char* symbol);
    void bbeq(int r1,int r2,int literal);
    void bbne(int r1,int r2,char* symbol);
    void bbne(int r1,int r2,int literal);
    void bbgt(int r1,int r2,char* symbol);
    void bbgt(int r1,int r2,int literal);
    void ppush(int r);
    void ppop(int r);
    void xxchg(int rS,int rD);
    void aadd(int rS,int rD);
    void ssub(int rS,int rD);
    void mmul(int rS,int rD);
    void ddiv(int rS,int rD);
    void nnot(int r);
    void aand(int rS,int rD);
    void oor(int rS,int rD);
    void xxor(int rS,int rD);
    void sshl(int rS,int rD);
    void sshr(int rS,int rD);
    void lldNeposredno(char* symbol,int r);
    void lldNeposredno(int literal,int r);
    void lldMemdir(char* symbol,int r);
    void lldMemdir(int literal,int r);
    void lldRegdir(int rS,int rD);
    void lldRegind(int rS,int rD);
    void lldRegindpom(int rS,char* symbol,int rD);
    void lldRegindpom(int rS,int literal,int rD);
    void sstNeposredno(int r,char* symbol);
    void sstNeposredno(int r,int literal);
    void sstMemdir(int r,char* symbol);
    void sstMemdir(int r,int literal);
    void sstRegdir(int rD,int rS);
    void sstRegind(int rD,int rS);
    void sstRegindpom(int rD,int rS,char* symbol);
    void sstRegindpom(int rD,int rS,int literal);
    void ccsrrdStatus(int r);
    void ccsrrdHandler(int r);
    void ccsrrdCause(int r);
    void ccsrwrStatus(int r);
    void ccsrwrHandler(int r);
    void ccsrwrCause(int r);

  






   
 };