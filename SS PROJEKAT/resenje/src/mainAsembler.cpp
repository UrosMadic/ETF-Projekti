#include "../inc/asembler.hpp"


Asembler *a1;

int main(int argc, char *argv[]){
  try{


    string op=argv[0];
    if(op=="./asembler"){
      a1=new Asembler(argc,argv);
      delete(a1);
    }else{
      
    }

  }catch (exception& e) {

		cout << e.what();
    
	}

  return 0;
}