#include "../inc/asembler.hpp"
#include "../bison.tab.h"
#include "../lexer.hpp"

#include <iostream> 
using namespace std;

int Asembler::prolaz=0;
int Asembler::locationCounter=0;

int Asembler::numSymbTab=0;
string Asembler::currSection="";


map<string,SymbolTableEntry*> Asembler::SymbolTable=map<string,SymbolTableEntry*>();
vector<LiteralTableEntry*> Asembler::LiteralTable=vector<LiteralTableEntry*>();
vector<RelocationTableEntry*> Asembler::RelocationTable=vector<RelocationTableEntry*>();
vector<MashCodeOfSectionsEntry*> Asembler::MashCodeOfSections=vector<MashCodeOfSectionsEntry*>();



Asembler::Asembler(){}

Asembler::~Asembler(){
  map<string,SymbolTableEntry*>::iterator it1;
  for ( it1 =SymbolTable.begin(); it1 != SymbolTable.end(); ++it1  ){
    delete (it1->second);
  }
  vector<LiteralTableEntry*>::iterator it2;
  for ( it2 =LiteralTable.begin(); it2 != LiteralTable.end(); ++it2  ){
    delete (*it2);
  }
  vector<RelocationTableEntry*>::iterator it3;
  for(it3=RelocationTable.begin();it3!=RelocationTable.end();++it3){
    delete (*it3);
      
  }

  vector<MashCodeOfSectionsEntry*>::iterator it4;
  for(it4=MashCodeOfSections.begin();it4!=MashCodeOfSections.end();++it4){
    delete (*it4);
  }

}

Asembler::Asembler(int argc, char *argv[]){

  char* inputFile=argv[3];
  char* outputFile=argv[2];


  FILE *myfile = fopen(inputFile, "r");


  SymbolTableEntry *ste=new SymbolTableEntry(numSymbTab++,0,0,"NOTYP","LOC","UND");
  SymbolTable.insert({"/",ste});

  if (!myfile) {
  cout << "Ne moze se otvoriti dati fajl!" << endl;
  
  }else{
    yyin = myfile;
    prolaz=1;
    //printf("Prolaz: %d\n",prolaz); 
    yyparse();
   
    myfile = fopen(inputFile, "r");

    yyrestart(myfile);// mora da se restartuje buffer kada se yyin menja i kada se vise puta poziva parse
    insertLastSectionLocations();
    if(locationCounter>=0x1000)throw BigSectionSize();

    currSection="";
    locationCounter=0;
    yyin = myfile;
    prolaz=2;
    //printf("Prolaz: %d\n",prolaz);
    yyparse();
    insertLastMachCode();
    correctMashCode();
    insertSectionSize();//ubaciti krajnje velicine sekcija


 
    // cout<<"Tabela simbola"<<endl;
  
    // map<string,SymbolTableEntry*>::iterator it1;
    // for(it1 =SymbolTable.begin();it1!=SymbolTable.end();++it1){
    //   cout << it1->first<<", ";
    //   cout << it1->second->getNum()<<", ";
    //   cout << it1->second->getValue()<<", ";
    //   cout << it1->second->getSize()<<", ";
    //   cout << it1->second->getType()<<", ";
    //   cout << it1->second->getBind()<<", ";
    //   cout << it1->second->getNdx()<<", ";
    //   cout<< it1->second->getStartBaseLiteral()<<", ";
    //   cout<< it1->second->getSectionSize()<<endl;
    // }
    // cout<<endl;

    // cout<<"Tabela literala"<<endl;

    // vector<LiteralTableEntry*>::iterator it2;
    // for(it2=LiteralTable.begin();it2!=LiteralTable.end();++it2){
    //   cout<<(*it2)->getValue()<<", ";
    //   cout<<(*it2)->getLocation()<<", ";
    //   cout<<(*it2)->getSymbol()<<", ";

    //   cout<<(*it2)->getSection()<<endl;

    // }
    // cout<<endl;

    // cout<<"Tabela relokacija"<<endl;

    // vector<RelocationTableEntry*>::iterator it3;
    // for(it3=RelocationTable.begin();it3!=RelocationTable.end();++it3){
    //   cout<<(*it3)->getOffset()<<", ";
    //   cout<<(*it3)->getSymbol()<<", ";
    //   cout<<(*it3)->getSection()<<endl;

    // }
    // cout<<endl;

    // cout<<"Tabela za masinski kod"<<endl;

    // vector<MashCodeOfSectionsEntry*>::iterator it4;
    // for(it4=MashCodeOfSections.begin();it4!=MashCodeOfSections.end();++it4){
    //   cout<<(*it4)->getCode()<<endl;
    // }


    writeOutputFile(outputFile);

    
  }
}

void Asembler::writeOutputFile(char* outputFile){
  ofstream MyFile(outputFile);
  string file=outputFile;
  MyFile<<file<<endl;
  MyFile<<"#.symtab"<<endl;
  map<string,SymbolTableEntry*>::iterator it;
  for(it=SymbolTable.begin();it!=SymbolTable.end();++it){
    MyFile<<it->first<<" ";
    MyFile<<it->second->getNum()<<" ";
    MyFile<<it->second->getValue()<<" ";
    MyFile<<it->second->getSize()<<" ";
    MyFile<<it->second->getType()<<" ";
    MyFile<<it->second->getBind()<<" ";
    MyFile<<it->second->getNdx()<<" ";
    MyFile<<it->second->getStartBaseLiteral()<<" ";
    MyFile<<it->second->getSectionSize()<<endl;

  }

  MyFile<<"#.relatab"<<endl;
  vector<RelocationTableEntry*>::iterator it1;
  for(it1=RelocationTable.begin();it1!=RelocationTable.end();++it1){
    MyFile<<(*it1)->getOffset()<<" ";
    MyFile<<(*it1)->getSymbol()<<" ";
    MyFile<<(*it1)->getSection()<<endl;

  }

  MyFile<<"#.machineCode"<<endl;
  vector<MashCodeOfSectionsEntry*>::iterator it2;
  for(it2=MashCodeOfSections.begin();it2!=MashCodeOfSections.end();++it2){

    string hexBE=(*it2)->getCode();
    char a[hexBE.length()+1];
    int j=0;
    for(int i=(hexBE.length()-1);i>=1;i-=2){
      a[j++]=hexBE[i-1];
      a[j++]=hexBE[i];
    }
    a[hexBE.length()]='\0';
    string hexLE=a;
    MyFile<<hexLE<<" ";
    MyFile<<(*it2)->getSection()<<endl;

  }




  MyFile.close();



}

void Asembler::insertSectionSize(){
  map<string,SymbolTableEntry*>::iterator it;
  for(it=SymbolTable.begin();it!=SymbolTable.end();++it){
    if(it->second->getType()=="SCTN"){
      vector<LiteralTableEntry*>::iterator it1;
      int lastLocCnt=0;
      bool hasLiteralInBase=false;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==it->first){
          lastLocCnt=(*it1)->getLocation();
          hasLiteralInBase=true;
        }
      }
      if(hasLiteralInBase){
        it->second->setSectionSize(lastLocCnt+4);
      }else{
        it->second->setSectionSize(it->second->getStartBaseLiteral());
      }

    }
  }

}

void Asembler::correctMashCode(){//sluzi da ispravimo pomeraje (najmanja 12 bita) koji su pogresno upisani prilikom 2.prolaza
  vector<MashCodeOfSectionsEntry*>::iterator it;
  for(it=MashCodeOfSections.begin();it!=MashCodeOfSections.end();++it){
    if((*it)->getCorrectionLiteral()!=0){
      if((*it)->getOffsetToBaseLiteral()){
        string code=(*it)->getCode();
        code.erase(code.length()-3);

        unsigned int lok;
        vector<LiteralTableEntry*>::iterator it2;
        for(it2=LiteralTable.begin();it2!=LiteralTable.end();++it2){
          if((*it2)->getValue()==(*it)->getCorrectionLiteral() && (*it2)->getSection()==(*it)->getSection()){
            lok=(unsigned int)(((*it2)->getLocation()-(*it)->getValueOfLocationCounter())&0xfff);
          }
        }
        char a[15];

        sprintf(a,"%03x",lok);
        code+=a;
        (*it)->setCode(code);
        
      }else{
        

      }
    }else if((*it)->getCorrectionSymbol()!=""){
      if((*it)->getOffsetToBaseLiteral()){
        string code=(*it)->getCode();
        code.erase(code.length()-3);
        unsigned int lok;
        vector<LiteralTableEntry*>::iterator it2;
        for(it2=LiteralTable.begin();it2!=LiteralTable.end();++it2){
          if((*it2)->getSymbol()==(*it)->getCorrectionSymbol() && (*it2)->getSection()==(*it)->getSection()){
            lok=(unsigned int)(((*it2)->getLocation()-(*it)->getValueOfLocationCounter())& 0xfff);
          }
        }
        char a[15];
        sprintf(a,"%03x",lok);
        code+=a;
        (*it)->setCode(code);

      }else{
        map<string,SymbolTableEntry*>::iterator it1=SymbolTable.find((*it)->getCorrectionSymbol());
        string code=(*it)->getCode();
        code.erase(code.length()-3);
        char a[15];
        unsigned int lok=(unsigned int)(((it1)->second->getValue()-(*it)->getValueOfLocationCounter())&0xfff);
        sprintf(a,"%03x",lok);
        code+=a;
        (*it)->setCode(code);
      }

    }else{//nista se ne menja kod masinskog koda

    }
  }
}

void Asembler::insertLastMachCode(){//ubaciti kod za bazen literala poslednje sekcije
  vector<LiteralTableEntry*>::iterator it;
  for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
    if((*it)->getSection()==currSection){
      char a[15];
      unsigned int lok=(unsigned int)((*it)->getValue());
      sprintf(a,"%08x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }
  }
}


void Asembler::insertLastSectionLocations(){
  map<string,SymbolTableEntry*>::iterator it1=SymbolTable.find(currSection);
  it1->second->setStartBaseLiteral(locationCounter);
  int prevLocCnt=locationCounter;
  string prevSect=currSection;
  vector<LiteralTableEntry*>::iterator it;
  for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
    if((*it)->getSection()==prevSect){
      (*it)->setLocation(prevLocCnt);
      prevLocCnt+=4;
    }
  }
  if(prevLocCnt>=0x1000)throw BigSectionSize();
}

void Asembler::llabel(char* labela){
  if(currSection=="")throw NoSection();
  string lab=labela;
  if(prolaz==1){

    if(SymbolTable.find(lab)!=SymbolTable.end())throw MultipleDefOfLab();

    SymbolTableEntry *ste=new SymbolTableEntry(numSymbTab++,locationCounter,0,"NOTYP","LOC",currSection);
    
    SymbolTable.insert({lab,ste});


  }else if(prolaz==2){
  } 

}

void Asembler::ssection(char* section){
  int prevLocCnt=locationCounter;
  if(prevLocCnt>=0x1000)throw BigSectionSize();
  string prevSect=currSection;
  locationCounter=0;
  currSection=section;
  if(prolaz==1){

    if(SymbolTable.find(currSection)!=SymbolTable.end())throw MultipleDefOfSect();
    if(prevSect!=""){
      map<string,SymbolTableEntry*>::iterator it1=SymbolTable.find(prevSect);
      it1->second->setStartBaseLiteral(prevLocCnt);
      vector<LiteralTableEntry*>::iterator it;
      for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
        if((*it)->getSection()==prevSect){
          (*it)->setLocation(prevLocCnt);
          prevLocCnt+=4;
        }
      }
      if(prevLocCnt>=0x1000)throw BigSectionSize();
    }

    SymbolTableEntry *ste=new SymbolTableEntry(numSymbTab++,locationCounter,0,"SCTN","LOC",currSection);
    
    SymbolTable.insert({currSection,ste});

  }else if(prolaz==2){
    //ubaciti za prethodnu sekciju vrednosti u bazen literala
    if(prevSect!=""){
      vector<LiteralTableEntry*>::iterator it;
      for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
        if((*it)->getSection()==prevSect){
          char a[15];
          unsigned int lok=(unsigned int)((*it)->getValue());
          sprintf(a,"%08x",lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,prevSect);
          MashCodeOfSections.push_back(mcose);
        }
      }
      
    }
    
  } 
}

void Asembler::gglobal(char *symbol){

  if(prolaz==1){

  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){
      throw NonDefinedGlobSymb();
    }else if(it->second->getType()=="SCTN"){
      throw GlobalSection();

    }else{
      it->second->setBind("GLOB");
    }
  }
}

void Asembler::eextern(char *symbol){
  string symb=symbol;
  if(prolaz==1){
    if(SymbolTable.find(symb)!=SymbolTable.end())throw DefOfSymbol();
    SymbolTableEntry *ste=new SymbolTableEntry(numSymbTab++,0,0,"NOTYP","GLOB","UND");
    SymbolTable.insert({symb,ste});

  }else if(prolaz==2){
    

  }
}

void Asembler::wword(char *symbol){
  locationCounter+=4;
  string symb=symbol;
  if(prolaz==1){


  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symb);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else {
      RelocationTableEntry *rte=new RelocationTableEntry(locationCounter-4,it->first,currSection);
      RelocationTable.push_back(rte);
      string code="00000000";
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
  

    }


  }

}

void Asembler::wword(int literal){
  locationCounter+=4;
  if(prolaz==1){
    
    
  }else if(prolaz==2){
    char a[15];
    unsigned int lit=(unsigned int)literal;
    sprintf(a,"%08x",lit);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  
  }

}

void Asembler::sskip(int literal){

  locationCounter+=literal;
  if(prolaz==1){


  }else if(prolaz==2){
    char a[literal*2+5];
    string b="";
    string c="00";
    for(int i=0;i<literal;i++){
      b.append(c);
      
    }
    string code(b);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
    

  }


}

void Asembler::eend(){

  if(prolaz==1){

  }else if(prolaz==2){

  }

}

void Asembler::hhalt(){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    
    string code="00000000";
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }

}

void Asembler::iint(){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    string code="10000000";
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::iiret(){
  if(currSection=="")throw NoSection();
  locationCounter+=8; 
  if(prolaz==1){
    
  }else if(prolaz==2){
    //string code1="93fe0004";
    string code1="960e0004";
    MashCodeOfSectionsEntry* mcose1=new MashCodeOfSectionsEntry(code1,currSection);
    MashCodeOfSections.push_back(mcose1);
    //string code2="970e0004";
    string code2="93fe0008";
    MashCodeOfSectionsEntry* mcose2=new MashCodeOfSectionsEntry(code2,currSection);
    MashCodeOfSections.push_back(mcose2);
  }

}

void Asembler::ccall(char *symbol){
  if(currSection=="")throw NoSection();
  string symb=symbol;
  locationCounter+=4;
  if(prolaz==1){

  
  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "20f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false);    
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "21f00%03x",lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true);  
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "21f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 

      MashCodeOfSections.push_back(mcose);
    }


  }

}

void Asembler::ccall(int literal){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  bool hasLiteral=false;
  if(prolaz==1){

    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    
    if(hasLiteral){

    }else{
      if(l<max){
        

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "20000%03x",literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "21f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 

      MashCodeOfSections.push_back(mcose);

    }



  }

}

void Asembler::rret(){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){

  }else if(prolaz==2){
 
    string code="93fe0004";
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
    

  }

}

void Asembler::jjmp(char *symbol){
  if(currSection=="")throw NoSection();
  string symb=symbol;

  locationCounter+=4;
  if(prolaz==1){

  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "30f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "38f00%03x",lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "38f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);
    }

  }

}

void Asembler::jjmp(int literal){
  if(currSection=="")throw NoSection();
  bool hasLiteral=false;
  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){


    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "30000%03x",literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "38f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);


    }

  }

}



void Asembler::bbeq(int reg1, int reg2, char *symbol){
  if(currSection=="")throw NoSection();
  string symb=symbol;

  locationCounter+=4;
  if(prolaz==1){

  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "31f%01x%01x%03x",reg1,reg2,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "39f%01x%01x%03x",reg1,reg2,lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "39f%01x%01x%03x",reg1,reg2,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);
    }


  }

}

void Asembler::bbeq(int re, int reg2,int literal){
  if(currSection=="")throw NoSection();

  bool hasLiteral=false;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){

    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "31000%03x",literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "39f00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);


    }

  }

}

void Asembler::bbne(int reg1, int reg2, char *symbol){
  if(currSection=="")throw NoSection();
  string symb=symbol;

  locationCounter+=4;
  if(prolaz==1){

  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "32f%01x%01x%03x",reg1,reg2,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "3af%01x%01x%03x",reg1,reg2,lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "3af%01x%01x%03x",reg1,reg2,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);
    }


  }

}

void Asembler::bbne(int reg1, int reg2, int literal){
  if(currSection=="")throw NoSection();
  bool hasLiteral=false;
  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){

    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "32000%03x",literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "3af00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);


    }

  }

}

void Asembler::bbgt(int reg1, int reg2, char *symbol){
  if(currSection=="")throw NoSection();
  string symb=symbol;

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "33f%01x%01x%03x",reg1,reg2,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "3bf%01x%01x%03x",reg1,reg2,lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "3bf%01x%01x%03x",reg1,reg2,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);
    }

  }

}

void Asembler::bbgt(int reg1, int reg2, int literal){
  if(currSection=="")throw NoSection();
  bool hasLiteral=false;
  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){

    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "33000%03x",literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "3bf00%03x",lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);


    }

  }

}

void Asembler::ppush(int r){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "81e0%01xffc",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::ppop(int r){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "93%01xe0004",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::xxchg(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "400%01x%01x000",rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }
}

void Asembler::aadd(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "50%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }
}

void Asembler::ssub(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;  
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "51%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}

void Asembler::mmul(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
   
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "52%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}

void Asembler::ddiv(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "53%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}


void Asembler::nnot(int r){
  if(currSection=="")throw NoSection();
  
  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "60%01x%01x0000",r ,r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}

void Asembler::aand(int rS, int rD){
  if(currSection=="")throw NoSection();
  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "61%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }
}

void Asembler::oor(int rS, int rD){
  if(currSection=="")throw NoSection();
  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "62%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}

void Asembler::xxor(int rS, int rD){
  if(currSection=="")throw NoSection();
  locationCounter+=4;
  if(prolaz==1){
   
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "63%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}

void Asembler::sshl(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "70%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }
}

void Asembler::sshr(int rS, int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "71%01x%01x%01x000",rD,rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }
}

void Asembler::lldNeposredno(char *symbol, int r){
  if(currSection=="")throw NoSection();
  string symb=symbol;

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "91%01xf0%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "92%01xf0%03x",r,lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "92%01xf0%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);
    }


  }
}

void Asembler::lldNeposredno(int literal, int r){
  if(currSection=="")throw NoSection();
  bool hasLiteral=false;
  locationCounter+=4; 
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){
 
    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "91%01x00%03x",r,literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "92%01xf0%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);


    }

  }
}

void Asembler::lldMemdir(char *symbol, int r){
  if(currSection=="")throw NoSection();
  string symb=symbol;
  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "92%01xf0%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);//da bi dohvatili pocetak bazena literala
      vector<LiteralTableEntry*>::iterator it1;
      map<string, SymbolTableEntry*>::iterator it3;
      vector<RelocationTableEntry*>::iterator it4;
      //naredne tri for petlje sluze da azuriraju lokacije posle navodjenja masinske instrukcije za ovu
      //instrukciju jer u prvom prolazu se povecao locCnt za 4 posto se nije znalo da li ce ova 
      //instrukcija zauzimati 2 masinska koda ili 1 pa mora da se poveca locCnt na ostatku u tabeli simbola za
      //tu sekciju kao i tabelu literala da pomeri 
      it2->second->setStartBaseLiteral(it2->second->getStartBaseLiteral()+4);//poveca se pocetak bazena literala za 4 prvo
      for(it3=SymbolTable.begin();it3!=SymbolTable.end();++it3){
        if(it3->second->getNdx()==currSection && it3->second->getValue()>=locationCounter){
          it3->second->setValue(it3->second->getValue()+4);
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          (*it1)->setLocation((*it1)->getLocation()+4);
        }
      }
      for(it4=RelocationTable.begin();it4!=RelocationTable.end();++it4){
        if((*it4)->getSection()==currSection){
          (*it4)->setOffset((*it4)->getOffset()+4);
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "92%01xf0%03x",r,lok);// prvo se uradi upisivanje u registar A mem[lokacija u bazenu literala]
          string code1(a);
          MashCodeOfSectionsEntry* mcose1=new MashCodeOfSectionsEntry(code1,currSection);
          mcose1->setCorrectionSymbol(symb);
          mcose1->setValueOfLocationCounter(locationCounter);
          mcose1->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose1);
          locationCounter+=4;

          char b[15];
          sprintf(b, "92%01x%01x0000",r,r);//drugo se u regA upise mem[regA]
          string code2(b);
          MashCodeOfSectionsEntry* mcose2=new MashCodeOfSectionsEntry(code2,currSection);
          MashCodeOfSections.push_back(mcose2);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "92%01xf0%03x",r,lok);// prvo se uradi upisivanje u registar A mem[lokacija u bazenu literala]
      string code1(a);
      MashCodeOfSectionsEntry* mcose1=new MashCodeOfSectionsEntry(code1,currSection);
      mcose1->setCorrectionSymbol(symb);
      mcose1->setValueOfLocationCounter(locationCounter);
      mcose1->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose1);
      locationCounter+=4;
      char b[15];
      sprintf(b, "92%01x%01x0000",r,r);//drugo se u regA upise mem[regA]
      string code2(b);
      MashCodeOfSectionsEntry* mcose2=new MashCodeOfSectionsEntry(code2,currSection);
      MashCodeOfSections.push_back(mcose2);
    
    }


  }

}

void Asembler::lldMemdir(int literal, int r){
  if(currSection=="")throw NoSection();
  bool hasLiteral=false;
  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){

    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        locationCounter+=4;
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "92%01x00%03x",r,literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{

      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "92%01xf0%03x",r,lok);//upisivanje sadrzaja na lokaciji u bazenu literala u regA 
     
      string code1(a);
      MashCodeOfSectionsEntry* mcose1=new MashCodeOfSectionsEntry(code1,currSection);
      mcose1->setCorrectionLiteral(literal);
      mcose1->setValueOfLocationCounter(locationCounter);
      mcose1->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose1);
      locationCounter+=4;
      char b[15];
      sprintf(b, "92%01x%01x0000",r,r);//drugo se u regA upise mem[regA]
      string code2(b);
      MashCodeOfSectionsEntry* mcose2=new MashCodeOfSectionsEntry(code2,currSection);
      MashCodeOfSections.push_back(mcose2);


    }

  }
}

void Asembler::lldRegdir(int rS,int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
    
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "91%01x%01x0000",rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);


  }

}

void Asembler::lldRegind(int rS,int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
    
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "92%01x%01x0000",rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }

}

void Asembler::lldRegindpom(int rS,char* symbol,int rD){

  throw NoDefSymb();

}

void Asembler::lldRegindpom(int rS,int literal,int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){
    
    if(l>=max)throw LitteralSize();

  }else if(prolaz==2){

    char a[15];
      
    sprintf(a, "92%01x%01x0%03x",rD,rS,literal);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
   
  }

}

void Asembler::sstNeposredno(int r,char* symbol){
  throw WrongInstruction();
}

void Asembler::sstNeposredno(int r,int literal){
  throw WrongInstruction();
}

void Asembler::sstMemdir(int r,char* symbol){
  if(currSection=="")throw NoSection();
  string symb=symbol;

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    map<string,SymbolTableEntry*>::iterator it=SymbolTable.find(symbol);
    if(it==SymbolTable.end()){//simbol nije definisan nigde u modulu
      throw NoSymbDef();
    }else if(it!=SymbolTable.end() && it->second->getNdx()==currSection){//simbol se koristi al je i definisan u istoj sekciji
      char a[15];
      unsigned int lok=(unsigned int)((it->second->getValue()-locationCounter)&0xfff);
      sprintf(a, "80f0%01x%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(false); 
      MashCodeOfSections.push_back(mcose);

    }else{//simbol se koristi ali nije definisan u istoj sekciji gde se koristi
      int lastLocCnt;
      bool emptyBase=true;
      map<string,SymbolTableEntry*>::iterator it2=SymbolTable.find(currSection);
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getSymbol()==symb){
          char a[15];
          unsigned int lok=(unsigned int)(((*it1)->getLocation()-locationCounter)&0xfff);
          sprintf(a, "82f0%01x%03x",r,lok);
          string code(a);
          MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
          mcose->setCorrectionSymbol(symb);
          mcose->setValueOfLocationCounter(locationCounter);
          mcose->setOffsetToBaseLieral(true); 
          MashCodeOfSections.push_back(mcose);
          return;
        }
      }
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection){
          lastLocCnt=(*it1)->getLocation();
          emptyBase=false;
        }
      }
      if(emptyBase){
        lastLocCnt=it2->second->getStartBaseLiteral();
      }else{
        lastLocCnt+=4;//da ubacimo 4 bajta posle poslednjeg literala ili simbola u tabeli literala
      }
      if(lastLocCnt>=0x1000)throw BigSectionSize();
      LiteralTableEntry* lte=new LiteralTableEntry(0,lastLocCnt,currSection);
      lte->setSymbol(symb);
      LiteralTable.push_back(lte);
      RelocationTableEntry *rte=new RelocationTableEntry(lastLocCnt,it->first,currSection);
  
      RelocationTable.push_back(rte);
      char a[15];
      unsigned int lok=(unsigned int)((lastLocCnt-locationCounter)&0xfff);
      sprintf(a, "82f0%01x%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionSymbol(symb);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);
    }

  }

}

void Asembler::sstMemdir(int r,int literal){
  if(currSection=="")throw NoSection();
  bool hasLiteral=false;
  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){
 
    vector<LiteralTableEntry*>::iterator it;
    for(it=LiteralTable.begin();it!=LiteralTable.end();++it){
      if((*it)->getValue()==literal){
        hasLiteral=true;
      }
    }
    if(hasLiteral){

    }else{
      if(l<max){

      }else{
        LiteralTableEntry *lte=new LiteralTableEntry(literal,0,currSection);
        LiteralTable.push_back(lte);
      }
    }
  }else if(prolaz==2){
    if(l<max){
      char a[15];
      
      sprintf(a, "8000%01x%03x",r,literal);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      MashCodeOfSections.push_back(mcose);
    }else{
      int location;
      vector<LiteralTableEntry*>::iterator it1;
      for(it1=LiteralTable.begin();it1!=LiteralTable.end();++it1){
        if((*it1)->getSection()==currSection && (*it1)->getValue()==literal){
          location=(*it1)->getLocation();
          break;
        }
      }
      char a[15];
      unsigned int lok=(unsigned int)((location-locationCounter)&0xfff);
      sprintf(a, "82f0%01x%03x",r,lok);
      string code(a);
      MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
      mcose->setCorrectionLiteral(literal);
      mcose->setValueOfLocationCounter(locationCounter);
      mcose->setOffsetToBaseLieral(true); 
      MashCodeOfSections.push_back(mcose);


    }

  }

}

void Asembler::sstRegdir(int rS,int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "91%01x%01x0000",rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);
  }

}

void Asembler::sstRegind(int rS,int rD){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){

  }else if(prolaz==2){
    char a[15];
    sprintf(a, "80%01x0%01x000",rD,rS);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::sstRegindpom(int rS,int rD,char* symbol){

  throw NoDefSymb();

}

void Asembler::sstRegindpom(int rS,int rD,int literal){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  unsigned int l=literal;
  unsigned int max=0x1000;
  if(prolaz==1){

    if(l>=max)throw LitteralSize();

  }else if(prolaz==2){
    char a[15];
    sprintf(a, "80%01x0%01x%03x",rD,rS,literal);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);


  }

}

void Asembler::ccsrrdStatus(int r){//0
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
 
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "90%01x00000",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::ccsrrdHandler(int r){//1
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "90%01x10000",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::ccsrrdCause(int r){//2
  if(currSection=="")throw NoSection();
  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "90%01x20000",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::ccsrwrStatus(int r){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "940%01x0000",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}

void Asembler::ccsrwrHandler(int r){
  if(currSection=="")throw NoSection();

  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "941%01x0000",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}


void Asembler::ccsrwrCause(int r){
  if(currSection=="")throw NoSection();
  
  locationCounter+=4;
  if(prolaz==1){
  
  }else if(prolaz==2){
    char a[15];
    sprintf(a, "942%01x0000",r);
    string code(a);
    MashCodeOfSectionsEntry* mcose=new MashCodeOfSectionsEntry(code,currSection);
    MashCodeOfSections.push_back(mcose);

  }

}
