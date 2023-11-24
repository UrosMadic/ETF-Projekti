#include <iostream>


using namespace std;


class RelocationTableEntry{

  int offset;//pomeraj u sekciji
  int value=0;//vrednost koju cemo da upisemo u kod
  //string type;//tip necemo imati posto ce biti isti tip relokacije uvek
  string symbol;//naziv simbola u tabeli simbola
  string section;//naziv sekcije
  string module;//naziv modula u kom je simbol za relokaciju

  public:
    RelocationTableEntry(int offset1,string symbol1,string section1):offset(offset1),symbol(symbol1),section(section1){}
    RelocationTableEntry(){};


    string getModule(){
      return this->module;
    }

    void setModule(string module){
      this->module=module;
    }

    int getOffset(){
      return this->offset;
    }

    void setOffset(int offset){
      this->offset=offset;
    }

    int getValue(){
      return this->value;
    }

    void setValue(int value){
      this->value=value;
    }

    string getSymbol(){
      return this->symbol;
    }

    void setSymbol(string symbol){
      this->symbol=symbol;
    }

    string getSection(){
      return this->section;
    }

    void setSection(string section){
      this->section=section;
    }

};
