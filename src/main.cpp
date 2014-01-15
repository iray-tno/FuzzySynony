#include <iostream>
#include <iomanip>
#include <climits>
#include "sqlite3.h"

#include "fuzzysynony.hpp"

int main(){
	FuzzySynony fs("addbtest.db");
	fs.ClearTable();
	fs.AddWord("ドセタキセル水和物");
	fs.AddWord("パクリタキセル");
	fs.AddWord("パクリタキセル(アルブミン懸濁型)");
	fs.AddWord("酒石酸ビノレルビン");
	fs.AddWord("シクロホスファミド");
	fs.AddWord("ラパチニブ");
	fs.AddWord("イリノテカン塩酸塩水和物");
	fs.AddWord("アナストロゾール");
	fs.AddWord("エキセメスタン");
	fs.AddWord("ゴセレリン酢酸塩");
	fs.AddWord("タモキシフェンクエン酸塩");
	fs.AddWord("トレミフェンクエン酸塩");
	fs.AddWord("フルベストラント");
	fs.AddWord("メドロキシプロゲステロン酢酸エステル");
	fs.AddWord("リュープロレリン酢酸塩");
	fs.AddWord("レトロゾール");
	fs.AddWord("エピルビシン塩酸塩");
	fs.AddWord("ドキソルビシン塩酸塩");
	fs.AddWord("マイトマイシンＣ");
	fs.AddWord("カペシタビン");
	fs.AddWord("テガフール");
	fs.AddWord("テガフール・ウラシル配合剤");
	fs.AddWord("テガフール・ギメラシル・オテラシル配合剤");
	fs.AddWord("ドキシフルリジン");
	fs.AddWord("フルオロウラシル");
	fs.AddWord("メトトレキサート");
	fs.AddWord("塩酸ゲムシタビン");
	fs.AddWord("カルボプラチン");
	fs.AddWord("シスプラチン");
	fs.AddWord("トラスツズマブ");
	fs.AddWord("ベバシズマブ");
	fs.AddWord("ゾレドロン酸");
	fs.AddWord("パミドロン酸二ナトリウム");
	fs.AddWord("ナルトグラスチム");
	fs.AddWord("フィルグラスチム");
	fs.AddWord("レノグラスチム");
	fs.AddWord("アザセトロン塩酸塩");
	fs.AddWord("アプレピタント");
	fs.AddWord("インジセトロン塩酸塩");
	fs.AddWord("オンダンセトロン塩酸塩水和物");
	fs.AddWord("グラニセトロン塩酸塩");
	fs.AddWord("デキサメタゾン");
	fs.AddWord("トロピセトロン塩酸塩");
	fs.AddWord("パロノセトロン塩酸塩");
	fs.AddWord("ラモセトロン塩酸塩");
	fs.AddWord("デノスマブ");
	fs.AddWord("ストロンチウム－89");
	std::string str;
	while(true){
		std::vector<FSResult> ret_fsresult;
		ret_fsresult.clear();
		std::cout << "Enter >> ";
		std::cin.getline(str);
		while(!std::cin){
			std::cin.clear();
			std::cin.ignore(INT_MAX,'\n');
			std::cout << std::endl << "End" << std::endl;
			return 0;
		}
		if(str == "\n"|| str == ""){
			std::cout << std::endl << "End" << std::endl;
			return 0;
		}
		if(1 < str_to_wstr(str).length()){
			fs.Search(str,ret_fsresult);
		}

		std::cout << "> Result" << std::endl << " No. | Score |  Word" << std::endl;
		for(int i = 0; i < ret_fsresult.size(); i++){
			std::cout << std::setw(4) << std::right << i+1 << " |";
			std::cout << std::setw(6) << std::right << ret_fsresult[i].score << " | ";
			std::cout << ret_fsresult[i].word << std::endl;
		}
		
	}
	return 0;
}