#include <iostream>
#include <map>
#include <vector>
#include <exception>
#include <fstream>
#include <sys/mman.h>
#include <cstring>


using namespace std;

class NoDefHexFile:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Nije moguce procitati hex fajl!\n";
	  }
};

class MmapError:public exception {
  public:
	  virtual const char* what() const throw () {
		  return "Greska: Mmap!\n";
	  }
};

class Emulator{

  const int pc=15;
  const int sp=14;
  const int status=0;
  const int handler=1;
  const int cause=2;

  //operacioni kodovi
  const string hhalt="00";
  const string iint="10";
  const string ccalldir="20";
  const string ccallmem="21";
  const string jjmpdir="30";
  const string jjmpmem="38";
  const string bbeqdir="31";
  const string bbeqmem="39";
  const string bbnedir="32";
  const string bbnemem="3A";
  const string bbgtdir="33";
  const string bbgtmem="3B";
  const string ppush="81";
  const string ppop="93";
  const string xxchg="40";
  const string aadd="50";
  const string ssub="51";
  const string mmul="52";
  const string ddiv="53";
  const string nnot="60";
  const string aand="61";
  const string oor="62";
  const string xxor="63";
  const string sshl="70";
  const string sshr="71";
  const string llddir="91";
  const string lldmemdir="92";
  const string sstmemdir="80";
  const string sstmemmemdir="82";
  const string ccsrrd="90";
  const string ccsrwr="94";
  const string ccsrwrmem="96";




  int gpregisters[16];
  int csregisters[3];

  unsigned char* memory=(unsigned char*)mmap(NULL, 1UL<<32, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);



  public:
    Emulator();
    Emulator(int argc, char *argv[]);
    ~Emulator();

    void printRegisters();
};