#include "../inc/linker.hpp"

vector<LinkerSymbolTableEntry*> Linker::SymbolTable=vector<LinkerSymbolTableEntry*>();
vector<RelocationTableEntry*> Linker::RelocationTable=vector<RelocationTableEntry*>();
vector<MashCodeOfSectionsEntry*> Linker::MashCodeOfSections=vector<MashCodeOfSectionsEntry*>();


Linker::Linker(){};

Linker::~Linker(){
  vector<LinkerSymbolTableEntry*>::iterator it1;
  for ( it1 =SymbolTable.begin(); it1 != SymbolTable.end(); ++it1  ){
    delete (*it1);
  }
  vector<RelocationTableEntry*>::iterator it2;
  for(it2=RelocationTable.begin();it2!=RelocationTable.end();++it2){
    delete (*it2);
      
  }
  vector<MashCodeOfSectionsEntry*>::iterator it3;
  for(it3=MashCodeOfSections.begin();it3!=MashCodeOfSections.end();++it3){
    delete (*it3);
  }

}

Linker::Linker(int argc, char *argv[]){

  int num=1;
  for(int i=0;i<argc;i++){

    map<string,int>correctRelaTab=map<string,int>();//sluzi za povecavanje lokacija u relokacionoj tabeli za sekcije koje se ponavljaju
    string line;
    string table;
    string module;
    int cnt=0;
    string fajl=argv[i];
    if(fajl.substr(fajl.length()-2)==".o"){
      ifstream MyReadFile(argv[i]);
      while(getline(MyReadFile,line)){
        if(cnt==0){
          module=line;
          vector<LinkerSymbolTableEntry*>::iterator it4;
          bool sameModule=false;
          for ( it4 =SymbolTable.begin(); it4 != SymbolTable.end(); ++it4 ){
            if((*it4)->getModule()==module){
              sameModule=true;
              break;
            }
          }
          if(sameModule)break;
          cnt++;
          continue;
        }
        if(line=="#.symtab"){
          table="symtab";
          continue;
        }else if(line=="#.relatab"){
          table="relatab";
          continue;
        }else if(line=="#.machineCode"){
          table="machineCode";
          continue;
        }
        if(table=="symtab"){
          int k=0;
          string data="";
          string name="";
          LinkerSymbolTableEntry *lste=new LinkerSymbolTableEntry();

          for(int j=0;j<(line.length());j++){
            char a=line[j];
            string b(1, a);
            if(b!=" "){
              data+=line[j];
            }else{
              if(k==0){
                name=data;
                lste->setName(data);
                data="";
                k++;
              }else if(k==1){
                
                data="";
                k++;
              }else if(k==2){
                lste->setValue(stoi(data));
                data="";
                k++;
              }else if(k==3){
                lste->setSize(stoi(data));
                data="";
                k++;
              }else if(k==4){
                lste->setType(data);
                data="";
                k++;
              }else if(k==5){
                lste->setBind(data);
                data="";
                k++;
              }else if(k==6){
                lste->setNdx(data);
                data="";
                k++;
              }else if(k==7){
                lste->setStartBaseLiteral(stoi(data));
                data="";
                k++;
              }
            }
          }
          lste->setSectionSize(stoi(data));
          bool sameSection=false;
          vector<LinkerSymbolTableEntry*>::iterator it2;
          for(it2=SymbolTable.begin();it2!=SymbolTable.end();++it2){//ubaciti u mapu ako postoji sekcija sa istim nazivom da bi se nadovezala
            if((*it2)->getName()==name && (*it2)->getType()=="SCTN" && lste->getType()=="SCTN"){
              correctRelaTab.insert({name,(*it2)->getSectionSize()});
              (*it2)->setSectionSize((*it2)->getSectionSize()+lste->getSectionSize());
              sameSection=true;
            }
          }

          for(it2=SymbolTable.begin();it2!=SymbolTable.end();++it2){
            if((*it2)->getName()==name && (*it2)->getBind()=="GLOB" &&
            lste->getBind()=="GLOB" && (*it2)->getNdx()!="UND" && lste->getNdx()!="UND"){
              throw MultiplyGlobalDef();}
          }
          if(!sameSection && lste->getName()!="/"){
            lste->setModule(module);
            lste->setNum(num++);
            SymbolTable.push_back(lste);
          }
        }else if(table=="relatab"){
          int k=0;
          string data="";
          RelocationTableEntry *rte=new RelocationTableEntry();
          for(int j=0;j<(line.length());j++){
            char a=line[j];
            string b(1, a);
            if(b!=" " ){
              data+=line[j];
            }else{
              if(k==0){
                rte->setOffset(stoi(data));
                data="";
                k++;
              }else if(k==1){
                rte->setSymbol(data);
                data="";
                k++;
              }
              
            }
          }
          rte->setSection(data);
          map<string,int>::iterator it;
          for(it=correctRelaTab.begin();it!=correctRelaTab.end();++it){
            if(rte->getSection()==it->first){
              rte->setOffset(rte->getOffset()+it->second);
              break;
            }
          }
          rte->setModule(module);
          RelocationTable.push_back(rte);
        }else if(table=="machineCode"){
          int k=0;
          string data="";
          MashCodeOfSectionsEntry *mcose=new MashCodeOfSectionsEntry();

          for(int j=0;j<(line.length());j++){
            char a=line[j];
            string b(1, a);
            if(b!=" "){
              data+=line[j];
            }else{
              if(k==0){
                mcose->setCode(data);
                data="";
                k++;
              }
            }
          }
          mcose->setSection(data);
          MashCodeOfSections.push_back(mcose);

        }
      }
      vector<LinkerSymbolTableEntry*>::iterator iter;
      map<string,int>::iterator iter1;

      for(iter=SymbolTable.begin();iter!=SymbolTable.end();++iter){//povecati ostalim simbolima vrednost ako se spoje 2 sekcije
        if((*iter)->getModule()==module && correctRelaTab.find((*iter)->getNdx())!=correctRelaTab.end()){
          iter1=correctRelaTab.find((*iter)->getNdx());
          (*iter)->setValue((*iter)->getValue()+iter1->second);
        }
      }


    }

  }

  checkExtern();
  map<string,int>startOfSections=map<string,int>();
  for(int i=0;i<argc;i++){
    string fajl=argv[i];
    
    if(fajl.substr(0,7)=="-place="){
      int k=0;
      string data=fajl.substr(7);
      string section="";
      string location="";

      for(int j=0;j<data.length();j++){
        if(k==0){
          char a=data[j];
          string b(1, a);
          if(b!="@"){
            section+=data[j];
          }else{
            k++;
          }
        }else{
          location+=data[j];
        }
      }
      if(location.length()>10){
        cout<<"Simbol/Sekcija: "<<section<<endl;
        throw OverlappingAdresSpace();
      }
      bool hasSection=false;
      vector<LinkerSymbolTableEntry*>::iterator it;

      for(it=SymbolTable.begin();it!=SymbolTable.end();++it){
        if((*it)->getName()==section && (*it)->getType()=="SCTN"){
          string loc=location.substr(2);
          if(startOfSections.find(section)!=startOfSections.end())throw MoreDefOfStartAdressSect();
          startOfSections.insert({section,(unsigned int)stol(loc,0,16)});
          (*it)->setSectionStart((unsigned int)stol(loc,0,16));
          hasSection=true;
          break;
        }
      }
      if(!hasSection)throw NoSectnDef();
    }

  }
  if(startOfSections.size()>1){//proverimo da li ce se preklapati sekcije kojima je zadata pocetna adresa
    vector<LinkerSymbolTableEntry*>::iterator it;
    for(it=SymbolTable.begin();it!=SymbolTable.end();++it){
      if((*it)->getSectionStart()!=0){
        if(((unsigned int)((*it)->getSectionStart()+(*it)->getSectionSize()))<((unsigned int)(*it)->getSectionStart())){
          cout<<"Simbol/Sekcija: "<<(*it)->getName()<<endl;
          throw OverlappingAdresSpace();
      }
      
        vector<LinkerSymbolTableEntry*>::iterator it2;
        for(it2=SymbolTable.begin();it2!=SymbolTable.end();++it2){
          if((*it2)->getSectionStart()!=0 && (*it)->getName()!=(*it2)->getName()){
            if((*it2)->getSectionStart()>(*it)->getSectionStart()){
              if(((*it)->getSectionStart()+(*it)->getSectionSize())>(*it2)->getSectionStart()){
                cout<<"Simbol/Sekcija: "<<(*it)->getName()<<endl;
                throw OverlappingAdresSpace();
              }
            }else{
              if(((*it2)->getSectionStart()+(*it2)->getSectionSize())>(*it)->getSectionStart()){
                cout<<"Simbol/Sekcija: "<<(*it2)->getName()<<endl;
                throw OverlappingAdresSpace();
              }
            }
          }
        }
      }
    }

  }


  unsigned int startForNonDefSect=0;//najvecu adresu da upisemo da smestamo ostale sekcije kojima nije definisana pocetna adresa
  string firstSection="";
  map<string,int>::iterator it;
  for(it=startOfSections.begin();it!=startOfSections.end();++it){
     if(it->second>startForNonDefSect){
          startForNonDefSect=it->second;
          firstSection=it->first;
     }
  }

  vector<string>orderSectionsInCode=vector<string>();//mora vektor ovde da bi se odrazo redosled sekcija
  if(firstSection!=""){
    orderSectionsInCode.push_back(firstSection);
  }
  vector<MashCodeOfSectionsEntry*>::iterator ite;
  for(ite=MashCodeOfSections.begin();ite!=MashCodeOfSections.end();++ite){
    vector<string>::iterator i;
    bool hasInOrder=false;//proverimo da li je sekcija vec navedena u redosledu
    for(i=orderSectionsInCode.begin();i!=orderSectionsInCode.end();++i){
      if((*ite)->getSection()==(*i)){
        hasInOrder=true;
        break;
      }
    }
    if(!hasInOrder && startOfSections.find((*ite)->getSection())==startOfSections.end()){       
        orderSectionsInCode.push_back((*ite)->getSection());
    }
  }
  
  
  vector<string>::iterator itr;

  //popuniti pocetne adrese ostalim sekcijama 
  for(itr=orderSectionsInCode.begin();itr!=orderSectionsInCode.end();++itr){
    vector<LinkerSymbolTableEntry*>::iterator i;
    for(i=SymbolTable.begin();i!=SymbolTable.end();++i){
      if((*i)->getName()==(*itr) && (*i)->getType()=="SCTN"){
        (*i)->setSectionStart(startForNonDefSect);
        startForNonDefSect+=(*i)->getSectionSize();
        if(startForNonDefSect<((*i)->getSectionStart())){
          cout<<"Simbol/Sekcija: "<<(*i)->getName()<<endl;
          throw OverlappingAdresSpace();
        }
        break;
      }
    }
  }


  //popuniti vrednosti ostalim simbolima kad znamo pocetne adrese za svaku sekciju
  //moze bez popunjavanja vrednosti za sve simbole moze samo za simbole iz tabele relokacija i to da upisemo u masinski kod


  vector<RelocationTableEntry*>::iterator iti;
  for(iti=RelocationTable.begin();iti!=RelocationTable.end();++iti){
    vector<LinkerSymbolTableEntry*>::iterator it;
    bool hasInSameModule=false;//ovo je ako se desi da ima dve istoimene sekcije sa definicijom 2 ista simbola da se prvo proveri
    //dal je u istom modulu ako ne onda se trazi taj simbol sa global
    for(it=SymbolTable.begin();it!=SymbolTable.end();++it){
      if((*iti)->getSymbol()==(*it)->getName() && (*iti)->getModule()==(*it)->getModule()){
        vector<LinkerSymbolTableEntry*>::iterator i;
        for(i=SymbolTable.begin();i!=SymbolTable.end();++i){
          if((*i)->getName()==(*it)->getNdx()){
            (*iti)->setValue((unsigned int)((*i)->getSectionStart()+(*it)->getValue()));
            hasInSameModule=true;
            break;
          }
        }
        break;
      }
    }
    if(!hasInSameModule){
      for(it=SymbolTable.begin();it!=SymbolTable.end();++it){
      if((*iti)->getSymbol()==(*it)->getName() && (*it)->getBind()=="GLOB" && (*it)->getNdx()!="UND"){
        vector<LinkerSymbolTableEntry*>::iterator i;
        for(i=SymbolTable.begin();i!=SymbolTable.end();++i){
          if((*i)->getName()==(*it)->getNdx()){
            (*iti)->setValue((unsigned int)((*i)->getSectionStart()+(*it)->getValue()));
            break;
          }
        }
        break;
      }
    }

    }

  }


  //popuniti masinski kod sa vrednostima za simbole iz tabele relokacija
  for(iti=RelocationTable.begin();iti!=RelocationTable.end();++iti){
    vector<MashCodeOfSectionsEntry*>::iterator it;
    int cnt=0;
    for(it=MashCodeOfSections.begin();it!=MashCodeOfSections.end();++it){
      if((*it)->getSection()==(*iti)->getSection()){
        if(cnt==(*iti)->getOffset()){
          char b[15];
          unsigned int val=(unsigned int)((*iti)->getValue());
          sprintf(b,"%08x",val);
          string hex(b);
          char a[9];
          int j=0;
          for(int i=(hex.length()-1);i>=1;i-=2){
            a[j++]=hex[i-1];
            a[j++]=hex[i];
          }
          string code=a;
          (*it)->setCode(code.substr(0,8));
          break;
          
        }
        cnt+=(((*it)->getCode().length())/2);
      }
    }
  }


  
  vector<string>newOrderSectionsInCode=vector<string>();
  int cntr=0;
  while(cntr<startOfSections.size()){
    unsigned int max=0xFFFFFFFF;
    string sect="";
    map<string,int>::iterator it;
    for(it=startOfSections.begin();it!=startOfSections.end();++it){
      //cout<<"1"<<endl;
      vector<string>::iterator itr;
      bool hasInOrder=false;
      bool hasInNewOrder=false;
      for(itr=orderSectionsInCode.begin();itr!=orderSectionsInCode.end();++itr){
      
        if((*itr)==it->first){
          hasInOrder=true;
          break;
        }
      }
      vector<string>::iterator itr1;
      
      for(itr1=newOrderSectionsInCode.begin();itr1!=newOrderSectionsInCode.end();++itr1){
        
        if((*itr1)==it->first){
          hasInNewOrder=true;
          break;
        }
      }
      if(it->second<max && !hasInOrder && !hasInNewOrder){
        
        max=it->second;
        sect=it->first;
      }
    }
    if(sect!=""){
      newOrderSectionsInCode.push_back(sect);
    }
    cntr++;
  }


  for(itr=orderSectionsInCode.begin();itr!=orderSectionsInCode.end();++itr){
    newOrderSectionsInCode.push_back((*itr));
  }


  // for(itr=orderSectionsInCode.begin();itr!=orderSectionsInCode.end();++itr){
  //   cout<<(*itr)<<endl;
  // }

  // for(itr=newOrderSectionsInCode.begin();itr!=newOrderSectionsInCode.end();++itr){
  //   cout<<(*itr)<<endl;
  // }
  

  bool hasHex=false;
  for(int i=0;i<argc;i++){
    string fajl=argv[i];
    if(fajl=="-hex"){
      hasHex=true;
      break;
    }
  }

  string output;  
  //upisivanje u hex fajl
  if(hasHex){
    for(int i=0;i<argc;i++){
      string fajl=argv[i];
      if(fajl.size()>4){
        if(fajl.substr(fajl.length()-4)==".hex"){
          output=argv[i];
          break;
        }
      }
    }
    ofstream MyFile(output);
    vector<string>::iterator itr;
    int cntNumByte=0;
    int cntNum=0;//brojac za upisivanje 8 bajtova u redu, posle toga prelazi u novi
    unsigned int last=0;//da proverimo da li je sledeca sekcija nastavak od ove ako prethodna ne popuni red do kraja ova da nastavi
    for(itr=newOrderSectionsInCode.begin();itr!=newOrderSectionsInCode.end();++itr){
      cntNumByte=0;
      int k=0;
      string start;
      vector<LinkerSymbolTableEntry*>::iterator iti;
      for(iti=SymbolTable.begin();iti!=SymbolTable.end();++iti){
        if((*iti)->getName()==(*itr) && (*iti)->getType()=="SCTN"){
          char a[15];
          sprintf(a,"%08X",(unsigned int)(*iti)->getSectionStart());
          if(last!=(unsigned int)((*iti)->getSectionStart()) && cntNum!=0){//provera da li ako se sekcija ne zavrsi na kraju dal da
          //se krene odma gde je stalo ili novo;
            MyFile<<endl;
            cntNum=0;
          }
          start=a;
        }
      }
      vector<MashCodeOfSectionsEntry*>::iterator ite;
      for(ite=MashCodeOfSections.begin();ite!=MashCodeOfSections.end();++ite){
        if((*ite)->getSection()==(*itr)){
          char b[((*ite)->getCode().length())+1];
          unsigned int cod=stol((*ite)->getCode(),0,16);
          int vel=(*ite)->getCode().length();
          sprintf(b,"%.*X",vel,(unsigned int)cod);
          string z(b);
          for(int j=0;j<z.length();j+=2){
            if(cntNum==7){
              MyFile<<z[j]<<z[j+1]<<endl;
              int start1=stol(start,0,16);
              cntNumByte++;
              start1+=cntNumByte;
              char a[15];
              sprintf(a,"%08X",(unsigned int)(start1));
              start=a;
              cntNumByte=0;
              cntNum=0;
            }
            else {
              if(cntNum==0){
                MyFile<<start+": "<<z[j]<<z[j+1]<<" ";
              }else{
                MyFile<<z[j]<<z[j+1]<<" ";
              }
              cntNum++;
              cntNumByte++;

            }


          }
        }
      }
      for(iti=SymbolTable.begin();iti!=SymbolTable.end();++iti){
        if((*iti)->getName()==(*itr) && (*iti)->getType()=="SCTN"){
          last=(unsigned int)((*iti)->getSectionStart()+(*iti)->getSectionSize());
        }
      }
      
      
    }
    
  
  }


  // cout<<endl;

  // cout<<"Tabela simbola"<<endl;

  // vector<LinkerSymbolTableEntry*>::iterator it1;
  // for(it1 =SymbolTable.begin();it1!=SymbolTable.end();++it1){
  //   cout << (*it1)->getName()<<", ";
  //   cout << (*it1)->getNum()<<", ";
  //   cout << (*it1)->getValue()<<", ";
  //   cout << (*it1)->getSize()<<", ";
  //   cout << (*it1)->getType()<<", ";
  //   cout << (*it1)->getBind()<<", ";
  //   cout << (*it1)->getNdx()<<", ";
  //   cout<< (*it1)->getSectionStart()<<", ";
  //   cout<< (*it1)->getSectionSize()<<", ";
  //   cout<< (*it1)->getModule()<<endl;
  // }

  // cout<<endl;
  // cout<<"Tabela relokacija"<<endl;

  // vector<RelocationTableEntry*>::iterator it3;
  // for(it3=RelocationTable.begin();it3!=RelocationTable.end();++it3){
  //   cout<<(*it3)->getOffset()<<", ";
  //   cout<<(*it3)->getSymbol()<<", ";
  //   cout<<(*it3)->getSection()<<", ";
  //   cout<< (*it3)->getModule()<<", ";
  //   cout<< (*it3)->getValue()<<endl;

  // }
  // cout<<endl;
  // cout<<"Tabela za masinski kod"<<endl;

  // vector<MashCodeOfSectionsEntry*>::iterator it4;
  // for(it4=MashCodeOfSections.begin();it4!=MashCodeOfSections.end();++it4){
  //   cout<<(*it4)->getCode()<<", ";
  //   cout<<((*it4)->getCode().length())/2<<", ";
  //   cout<<(*it4)->getSection()<<endl;
    
  // }

}



void Linker::checkExtern(){
  vector<LinkerSymbolTableEntry*>::iterator it1;
  for(it1 =SymbolTable.begin();it1!=SymbolTable.end();++it1){
    if((*it1)->getBind()=="GLOB" && (*it1)->getNdx()=="UND"){
      vector<LinkerSymbolTableEntry*>::iterator it2;
      bool hasGlobal=false;
      for(it2 =SymbolTable.begin();it2!=SymbolTable.end();++it2){
        if((*it1)->getName()==(*it2)->getName() && (*it2)->getBind()=="GLOB" && (*it2)->getNdx()!="UND"){
          hasGlobal=true;
        }
      }
      if(!hasGlobal)throw NoGlobalDef();

    }
  }

}


