#include <iostream>


using namespace std;


class SymbolTableEntry{

  int num;
  int value;
  int size;
  string type;// notype ili sctn
  string bind;// lokalni ili globalni simbol
  string ndx;//kojoj sekciji pripada(num od sekcije ili naziv)
  int startBaseLiteral=0;//start bazena literala za sekcije(ovo polje ce imati samo sekcije)
  int sectionSize=0;//oznacava velicinu sekcije ako je tipa SCTN
  int sectionStart=0;//od koje adrese pocinje sekcija
public:
  SymbolTableEntry(int num1,int value1,int size1,string type1,string bind1,string ndx1):num(num1),value(value1),
  size(size1),type(type1),bind(bind1),ndx(ndx1){}; 
  
  SymbolTableEntry(){};


  int getSectionSize(){
    return this->sectionSize;
  }

  void setSectionSize(int sectionSize){
    this->sectionSize=sectionSize;
  }

  int getStartBaseLiteral(){
    return this->startBaseLiteral;
  }

  void setStartBaseLiteral(int startBaseLiteral){
    this->startBaseLiteral=startBaseLiteral;
  }

  int getNum(){
    return this->num;
  }

  void setNum(int num){
    this->num=num;
  }

  int getValue(){
    return this->value;
  } 

  void setValue(int value){
    this->value=value;
  }

  int getSize(){
    return this->size;
  }

  void setSize(int size){
    this->size=size;
  }

  string getType(){
    return this->type;
  }

  void setType(string type){
    this->type=type;
  }

  string getBind(){
    return this->bind;

  }

  void setBind(string bind){
    this->bind=bind;
  }


  string getNdx(){
    return this->ndx;
    
  }

  void setNdx(string ndx){
    this->ndx=ndx;
  }

 

};