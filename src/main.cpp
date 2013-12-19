#include <iostream>
#include "sqlite3.h"

#include "fuzzysynony.hpp"

int main(){
	FuzzySynony fs("testa.db");
	fs.ClearTable();
	fs.AddWord("あいうえお");
	fs.AddWord("あかさたな");
	fs.AddWord("かきくけこ");
	fs.AddWord("あいaえお");
	fs.AddWord("あか塩たな");
	fs.AddWord("かきがけこ");
	fs.AddWord("高橋");
	fs.AddWord("田中");
	fs.AddWord("鈴木");
	fs.AddWord("中田");
	return 0;
}