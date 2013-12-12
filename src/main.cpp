#include <iostream>
#include "sqlite3.h"

#include "fuzzysynony.hpp"

int main(){
	FuzzySynony fs("testa.db");
	sqlite3* pDB;
	std::cout << sqlite3_open("test.db", &pDB) << std::endl
	          << SQLITE_OK << std::endl
	          << sqlite3_close(pDB) << std::endl;
	std::cout <<"helloworld" << std::endl;
	return 0;
}