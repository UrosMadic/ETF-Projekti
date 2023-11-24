#include <iostream>


using namespace std;

class LiteralTableEntry{
    int value;//vrednost literala
    string symbol="";//naziv simbola
    int location;//adresa prostora u bazenu literala pocev od kojeg je smestena vrednost literalne konstante
    string section;//kojoj sekciji pripada literal

  public:

  LiteralTableEntry(int value1,int location1,string section1):value(value1),location(location1),section(section1){}

  void setSymbol(string symbol){
    this->symbol=symbol;
  }

  string getSymbol(){
    return this->symbol;
  }

  void setLocation(int location){
    this->location=location;
  }

  int getValue(){
    return this->value;
  }

  int getLocation(){
    return this->location;
  }

  string getSection(){
    return this->section;
  }

};