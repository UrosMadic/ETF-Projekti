#include <iostream>
#include <map>
#include <vector>
#include "RelocationTableEntry.hpp"
#include "LinkerSymbolTableEntry.hpp"
#include "MashCodeOfSectionsEntry.hpp"
#include <exception>
#include <fstream>
#include <bits/stdc++.h> 

using namespace std;

class MultiplyGlobalDef:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Jedan simbol je definisan kao global vise puta!\n";
	  }
};

class NoGlobalDef:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Ne postoji global definicija za simbol koji je oznacen sa extern!\n";
	  }
};

class NoSectnDef:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Ne postoji sekcija za koju je uradjen place!\n";
	  }
};

class OverlappingAdresSpace:public exception {
	public:
	  virtual const char* what() const throw () {
		  return "Greska: Doslo je do prekoracenja ili preklapanja adresnog prostora!\n";
	  }
};

class MoreDefOfStartAdressSect:public exception {
  public:

	  virtual const char* what() const throw () {
		  return "Greska: Dodeljeno je vise pocetnih adresa za jednu sekciju!\n";
	  }
};



class Linker{

  static vector<LinkerSymbolTableEntry*>SymbolTable;

  static vector<RelocationTableEntry*>RelocationTable;
  static vector<MashCodeOfSectionsEntry*>MashCodeOfSections;

  public:
    Linker();
    Linker(int argc, char *argv[]);
    ~Linker();

    void checkExtern();
    void checkOverlapping();

  
};