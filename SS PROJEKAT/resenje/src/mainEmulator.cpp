#include "../inc/emulator.hpp"

int main(int argc, char *argv[]){
  try{

    string op=argv[0];
    if(op=="./emulator"){
      Emulator *e1=new Emulator(argc,argv);
      delete(e1);
    }else {
      
    }


  }catch (exception& e) {

		cout << e.what();
    
	}

  return 0;
}