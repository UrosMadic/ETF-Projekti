#include "../inc/emulator.hpp"


Emulator::~Emulator(){

  munmap(memory,1UL<<32);

}

Emulator::Emulator(int argc, char *argv[]){


  //inicijalizujemo registre na 0
  for(int i=0;i<16;i++){
    gpregisters[i]=0;
  }
  for(int i=0;i<3;i++){
    csregisters[i]=0;
  }


  if(memory==MAP_FAILED)throw MmapError();
  string file=argv[1];

  ifstream MyReadFile(argv[1]);

  if(!MyReadFile.is_open())throw NoDefHexFile();

  string line;

  while(getline(MyReadFile,line)){

    gpregisters[15]=stol(line.substr(0,8),0,16);
    char code[2];
    code[0]=line[10];
    code[1]=line[11];
    memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);
    gpregisters[15]+=1;
    if(line.length()>13){
      code[0]=line[13];
      code[1]=line[14];
      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);
      gpregisters[15]+=1;
    }
    if(line.length()>16){
      code[0]=line[16];
      code[1]=line[17];
      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);

      gpregisters[15]+=1;
    }
    if(line.length()>19){
      code[0]=line[19];
      code[1]=line[20];

      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);
      gpregisters[15]+=1;
    }
    if(line.length()>22){
      code[0]=line[22];
      code[1]=line[23];

      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);
      gpregisters[15]+=1;
    }
    if(line.length()>25){
      code[0]=line[25];
      code[1]=line[26];

      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);
      gpregisters[15]+=1;
    }
    if(line.length()>28){
      code[0]=line[28];
      code[1]=line[29];

      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);
      gpregisters[15]+=1;
    }
    if(line.length()>31){
      code[0]=line[31];
      code[1]=line[32];

      memory[(unsigned int)gpregisters[15]]=(unsigned char)stol(code,0,16);

    }
  
  }


  string instruction="";
  gpregisters[pc]=0x40000000;
  bool hasHalt=false;
  int k=0;
  while(!hasHalt){
  //while(k!=23){  
    instruction="";
    for(int i=3;i>=0;i--){
      char a[3];
      sprintf(a,"%02X",(int)memory[(unsigned int)gpregisters[pc]+i]);
      instruction+=a;
    }
    gpregisters[pc]+=4;
    if(instruction.substr(0,2)==hhalt){
      hasHalt=true;
    }else if(instruction.substr(0,2)==iint){
      gpregisters[sp]-=4;
      char a[9];
      sprintf(a,"%08X",csregisters[status]);
      string data(a);
      int j=0;
      for(int i=3;i>=0;i--){        
        memory[(unsigned int)gpregisters[sp]+i]=stoi(data.substr(j,2),0,16);
        j+=2;
      }
      j=0;
      gpregisters[sp]-=4;

      char a1[9];
      sprintf(a1,"%08X",gpregisters[pc]);
      string data1(a1);
      for(int i=3;i>=0;i--){        
        memory[(unsigned int)gpregisters[sp]+i]=stoi(data1.substr(j,2),0,16);
        j+=2;
      }
      j=0;
      csregisters[cause]=4;
      csregisters[status]=csregisters[status]&(~0x1);
      gpregisters[pc]=csregisters[handler];



    }else if(instruction.substr(0,2)==ccalldir){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      gpregisters[sp]-=4;
      char a[9];
      sprintf(a,"%08X",gpregisters[pc]);
      string data(a);
      int j=0;
      for(int i=3;i>=0;i--){
        memory[(unsigned int)gpregisters[sp]+i]=stoi(data.substr(j,2),0,16);
        j+=2;
      }
      j=0;
      gpregisters[pc]=gpregisters[stoi(instruction.substr(2,1),0,16)]+gpregisters[stoi(instruction.substr(3,1),0,16)]+offset;
    }else if (instruction.substr(0,2)==ccallmem){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      gpregisters[sp]-=4;
      char a[9];
      sprintf(a,"%08X",gpregisters[pc]);
      string data(a);
  

      int j=0;
      for(int i=3;i>=0;i--){
        memory[(unsigned int)gpregisters[sp]+i]=stoi(data.substr(j,2),0,16);

        j+=2;
      }
      j=0;
      string data1="";
      for(int i=3;i>=0;i--){
        char a[3];
        sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,1),0,16)]+gpregisters[stoi(instruction.substr(3,1),0,16)]+offset+i)]));
        data1+=a;
      }
      gpregisters[pc]=stol(data1,0,16);

    }else if(instruction.substr(0,2)==jjmpdir){

      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }

      gpregisters[pc]=gpregisters[stoi(instruction.substr(2,1),0,16)]+offset;
      
    }else if(instruction.substr(0,2)==jjmpmem){

      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }

      string data="";
      for(int i=3;i>=0;i--){
        char a[3];
        sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,3),0,16)]+offset+i)]));
        data+=a;
      }
      gpregisters[pc]=stol(data,0,16);
      
    }else if(instruction.substr(0,2)==bbeqdir){

      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      if(gpregisters[stoi(instruction.substr(3,1),0,16)]==gpregisters[stoi(instruction.substr(4,1),0,16)]){
        gpregisters[pc]=gpregisters[stoi(instruction.substr(2,1),0,16)]+offset;
      }



      
    }else if(instruction.substr(0,2)==bbeqmem){

      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }

      if(gpregisters[stoi(instruction.substr(3,1),0,16)]==gpregisters[stoi(instruction.substr(4,1),0,16)]){
        string data="";
        for(int i=3;i>=0;i--){
          char a[3];
          sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,1),0,16)]+offset+i)]));
          data+=a;
        }
        gpregisters[pc]=stol(data,0,16);
      }


      
    }else if(instruction.substr(0,2)==bbnedir){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      if(gpregisters[stoi(instruction.substr(3,1),0,16)]!=gpregisters[stoi(instruction.substr(4,1),0,16)]){
        gpregisters[pc]=gpregisters[stoi(instruction.substr(2,1),0,16)]+offset;
      }
      
    }else if(instruction.substr(0,2)==bbnemem){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      if(gpregisters[stoi(instruction.substr(3,1),0,16)]!=gpregisters[stoi(instruction.substr(4,1),0,16)]){
        string data="";
        for(int i=3;i>=0;i--){
          char a[3];
          sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,1),0,16)]+offset+i)]));
          data+=a;

        }
        gpregisters[pc]=stol(data,0,16);
      }
      
    }else if(instruction.substr(0,2)==bbgtdir){

      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      
      if(gpregisters[stoi(instruction.substr(3,1),0,16)]>gpregisters[stoi(instruction.substr(4,1),0,16)]){
        gpregisters[pc]=gpregisters[stoi(instruction.substr(2,1),0,16)]+offset;
      }

      
    }else if(instruction.substr(0,2)==bbgtmem){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      
      if(gpregisters[stoi(instruction.substr(3,1),0,16)]>gpregisters[stoi(instruction.substr(4,1),0,16)]){
        string data="";
        for(int i=3;i>=0;i--){
          char a[3];
          sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,1),0,16)]+offset+i)]));
          data+=a;

        }
        gpregisters[pc]=stol(data,0,16);

      }

      
    }else if(instruction.substr(0,2)==ppush){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      
      gpregisters[stoi(instruction.substr(2,1),0,16)]+=offset;
      char a[9];
      sprintf(a,"%08X",gpregisters[stoi(instruction.substr(4,1),0,16)]);
      string data(a);
      int j=0;
      for(int i=3;i>=0;i--){        
        memory[(unsigned int)gpregisters[sp]+i]=stoi(data.substr(j,2),0,16);
        j+=2;
      }
      j=0;


    }else if(instruction.substr(0,2)==ppop){
      string data="";
      for(int i=3;i>=0;i--){
        char a[3];
        sprintf(a,"%02X",(int)(memory[(unsigned int)gpregisters[stoi(instruction.substr(3,1),0,16)]+i]));
        data+=a;
      }


      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }
      gpregisters[stoi(instruction.substr(2,1),0,16)]=stol(data,0,16);
      gpregisters[stoi(instruction.substr(3,1),0,16)]+=offset;
      
      
    }else if(instruction.substr(0,2)==xxchg){

      unsigned int temp=gpregisters[stoi(instruction.substr(3,1),0,16)];
      gpregisters[stoi(instruction.substr(3,1),0,16)]=gpregisters[stoi(instruction.substr(4,1),0,16)];
      gpregisters[stoi(instruction.substr(4,1),0,16)]=temp;

      
    }else if(instruction.substr(0,2)==aadd){


      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]+gpregisters[stoi(instruction.substr(4,1),0,16)];
      
    }else if(instruction.substr(0,2)==ssub){

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]-gpregisters[stoi(instruction.substr(4,1),0,16)];
      
    }else if(instruction.substr(0,2)==mmul){
      
      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]*gpregisters[stoi(instruction.substr(4,1),0,16)];

    }else if(instruction.substr(0,2)==ddiv){

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]/gpregisters[stoi(instruction.substr(4,1),0,16)];

    }else if(instruction.substr(0,2)==nnot){
      
      gpregisters[stoi(instruction.substr(2,1),0,16)]=~gpregisters[stoi(instruction.substr(3,1),0,16)];

    }else if(instruction.substr(0,2)==aand){
      
      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]&gpregisters[stoi(instruction.substr(4,1),0,16)];

    }else if(instruction.substr(0,2)==oor){

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]|gpregisters[stoi(instruction.substr(4,1),0,16)];
      
    }else if(instruction.substr(0,2)==xxor){

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]^gpregisters[stoi(instruction.substr(4,1),0,16)];
      
    }else if(instruction.substr(0,2)==sshl){

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]<<gpregisters[stoi(instruction.substr(4,1),0,16)];
      
    }else if(instruction.substr(0,2)==sshr){

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]>>gpregisters[stoi(instruction.substr(4,1),0,16)];
      
    }else if(instruction.substr(0,2)==llddir){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }

      gpregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)]+offset;
      

    }else if(instruction.substr(0,2)==lldmemdir){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }


      string data="";
      for(int i=3;i>=0;i--){
        char a[3];
        sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(3,1),0,16)]+offset+i)]));
        data+=a;
      }

      gpregisters[stoi(instruction.substr(2,1),0,16)]=stol(data,0,16);



      
    }else if(instruction.substr(0,2)==sstmemdir){

      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }

      char a[9];
      sprintf(a,"%08X",gpregisters[stoi(instruction.substr(4,1),0,16)]);
      string data(a);
      int j=0;
      for(int i=3;i>=0;i--){        
        memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,1),0,16)]+gpregisters[stoi(instruction.substr(3,1),0,16)]+offset+i)]=stol(data.substr(j,2),0,16);
        j+=2;
      }
      j=0;

      
    }else if(instruction.substr(0,2)==sstmemmemdir){
      unsigned int offset=0;
      if(stoi(instruction.substr(5,3),0,16)>>11==1){
        offset=(0xfffff000|(stol(instruction.substr(5,3),0,16)&0xfff));
      }else{
        offset=stol(instruction.substr(5,3),0,16);
      }

      string data="";
      for(int i=3;i>=0;i--){
        char a[3];
        sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(2,1),0,16)]+gpregisters[stoi(instruction.substr(3,1),0,16)]+offset+i)]));
        data+=a;
      }

      char b[9];
      sprintf(b,"%08X",gpregisters[stoi(instruction.substr(4,1),0,16)]);
      string data1(b);
      int j=0;
      for(int i=3;i>=0;i--){
        memory[(unsigned int)((stol)(data,0,16)+i)]=stol(data1.substr(j,2),0,16);
        j+=2;
      }
      j=0;
   

    }else if(instruction.substr(0,2)==ccsrrd){
      
      gpregisters[stoi(instruction.substr(2,1),0,16)]=csregisters[stoi(instruction.substr(3,1),0,16)];

    }else if(instruction.substr(0,2)==ccsrwr){

      csregisters[stoi(instruction.substr(2,1),0,16)]=gpregisters[stoi(instruction.substr(3,1),0,16)];

    }else if(instruction.substr(0,2)==ccsrwrmem){

      string data="";
      for(int i=3;i>=0;i--){
        char a[3];
        sprintf(a,"%02X",(int)(memory[(unsigned int)(gpregisters[stoi(instruction.substr(3,1),0,16)]+gpregisters[stoi(instruction.substr(4,1),0,16)]+stoi(instruction.substr(5,3),0,16)+i)]));
        data+=a;
      }

      csregisters[stoi(instruction.substr(2,1),0,16)]=stol(data,0,16);

    }

    k++;

  }

  printRegisters();
    
}


void Emulator::printRegisters(){
  char r0[9];
  char r1[9];
  char r2[9];
  char r3[9];
  char r4[9];
  char r5[9];
  char r6[9];
  char r7[9];
  char r8[9];
  char r9[9];
  char r10[9];
  char r11[9];
  char r12[9];
  char r13[9];
  char r14[9];
  char r15[9];
  
  sprintf(r0,"%08x",gpregisters[0]);
  sprintf(r1,"%08x",gpregisters[1]);
  sprintf(r2,"%08x",gpregisters[2]);
  sprintf(r3,"%08x",gpregisters[3]);
  sprintf(r4,"%08x",gpregisters[4]);
  sprintf(r5,"%08x",gpregisters[5]);
  sprintf(r6,"%08x",gpregisters[6]);
  sprintf(r7,"%08x",gpregisters[7]);
  sprintf(r8,"%08x",gpregisters[8]);
  sprintf(r9,"%08x",gpregisters[9]);
  sprintf(r10,"%08x",gpregisters[10]);
  sprintf(r11,"%08x",gpregisters[11]);
  sprintf(r12,"%08x",gpregisters[12]);
  sprintf(r13,"%08x",gpregisters[13]);
  sprintf(r14,"%08x",gpregisters[14]);
  sprintf(r15,"%08x",gpregisters[15]);


  cout<<"--------------------------------------------------------------------"<<endl;
  cout<<"Emulated processor executed halt instruction"<<endl;
  cout<<"Emulated processor state:" << endl;
  cout<<" r0=0x"<<r0<< "    ";
  cout<<"r1=0x" <<r1<< "    ";
  cout<<"r2=0x" <<r2<< "    ";
  cout<<"r3=0x" <<r3<< endl;
  cout<<" r4=0x" <<r4<<"    ";
  cout<< "r5=0x" <<r5<< "    ";
  cout<<"r6=0x" <<r6<< "    ";
  cout<<"r7=0x" <<r7<< endl;
  cout<<" r8=0x" <<r8<< "    ";
  cout<<"r9=0x" <<r9<< "   ";
  cout<<"r10=0x" <<r10<< "   ";
  cout<<"r11=0x" <<r11<< endl;
  cout<<"r12=0x" <<r12<< "   ";
  cout<<"r13=0x" <<r13<< "   ";
  cout<<"r14=0x" <<r14<< "   ";
  cout<<"r15=0x" <<r15<<endl;

}