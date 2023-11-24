#include <iostream>


using namespace std;

class MashCodeOfSectionsEntry{
  string code;
  string section;
  string correctionSymbol="";
  int correctionLiteral=0;
  int ValueOfLocationCounter=0;
  bool offsetToBaseLiteral;// da li je pomeraj do bazena literala ili je pomeraj u sekciji do definicije simbola
  
  public:

    MashCodeOfSectionsEntry(string code1,string section1):code(code1),section(section1){}
    MashCodeOfSectionsEntry(){};

    bool getOffsetToBaseLiteral(){
      return this->offsetToBaseLiteral;
    }

    void setOffsetToBaseLieral(bool offsetToBaseLiteral){
      this->offsetToBaseLiteral=offsetToBaseLiteral;
    }


    string getCode(){
      return this->code;
    }

    void setCode(string code){
      this->code=code;
    }

    void setCorrectionSymbol(string symbol){
      this->correctionSymbol=symbol;
    }

    void setCorrectionLiteral(int literal){
      this->correctionLiteral=literal;
    }

    string getCorrectionSymbol(){
      return this->correctionSymbol;
    }

    int getCorrectionLiteral(){
      return this->correctionLiteral;
    }

    void setValueOfLocationCounter(int locCnt){
      this->ValueOfLocationCounter=locCnt;
    }

    int getValueOfLocationCounter(){
      return this->ValueOfLocationCounter;
    }

    string getSection(){
      return this->section;
    }

    void setSection(string section){
      this->section=section;
    }
};