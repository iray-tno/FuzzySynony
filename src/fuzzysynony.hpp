#ifndef FUZZYSYNONY_FUZZYSYNONY_HPP_
#define FUZZYSYNONY_FUZZYSYNONY_HPP_

#include "sqlite3.h"
#include "gtest/gtest_prod.h"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>

#include <cstdlib>

#include <iostream>
#include <sstream>
//#include <codecvt>
/*
あいまい検索実装

このクラスが担うこと
検索ワードからのあいまい検索
結果からシノニム一覧抽出（又は一般名だけ引き出す）
検索対象を追加（N-gramに分割と一般名との紐づけ）
以上

必要な機能
①検索ワードをN-gramに分割してSQL文生成→検索
②検索結果からヒットしたものにスコアをつけてスコア順に羅列
③
以上
*/

//SQL
//http://idocsq.net/page/511

//SQLiteで複数プライマリキーの設定
//http://isakado.blog106.fc2.com/blog-entry-17.html

//wchar char wstring string相互変換
//http://nullnull.hatenablog.com/entry/20120626/1340703148

/*
曖昧検索で拾いたいもの
例 "あいうえお"で検索して
　完全一致が最上位 "あいうえお"
（カタカナなどの相互変換　またはどちらかに統一する）
　文字追加　端の追加は順位高く　"カあいうえお" "あいうえおカ"　"あいうカえお"
　
*/
//①初期化
//②検索ワードからあいまい検索
//③そこからシノニムを抽出
//④バイグラムを検索対象から作成

//TODO　固定長文字列を実装（2バイト文字も1文字と数える） クラスにする？
struct Ngram{
	std::wstring index;
	int offset;
	bool operator<( const Ngram& right ) const{ return offset < right.offset; }
	bool operator>( const Ngram& right ) const{ return offset > right.offset; }
};

/*
inline std::wstring str_to_wstr (const std::string& str){
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.from_bytes(str);
}

inline std::string wstr_to_str (const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(str);
}*/

inline std::wstring str_to_wstr (const std::string& str){
	setlocale( LC_CTYPE, "" );
	wchar_t *wstr_wchar = new wchar_t[str.length() + 1];
	mbstowcs(wstr_wchar, str.c_str(), str.length() + 1);
	std::wstring wstr = wstr_wchar;
	delete [] wstr_wchar;
	return wstr;
}

inline std::string wstr_to_str (const std::wstring& wstr){
	setlocale( LC_CTYPE, "" );
	char *str_char = new char[wstr.length() * MB_CUR_MAX + 1];
	wcstombs(str_char, wstr.c_str(), wstr.length() * MB_CUR_MAX + 1);
	std::string str = str_char;
	delete [] str_char;
	return str;
}

class FuzzySynony{
 public:

	FuzzySynony() : is_sqlite_opened_(false),
	                is_monogram_available_(false),
	                is_bigram_available_(true),
	                is_trigram_available_(false){};
	FuzzySynony(const std::string& db_name) : is_sqlite_opened_(false){ OpenSQLite(db_name); }
	~FuzzySynony(){ CloseSQLite(); }

	void OpenSQLite(const std::string& db_name){
		if(SQLITE_OK == sqlite3_open(db_name.c_str(),&db_handler_)){
			is_sqlite_opened_ = true;
		}
	}

	void CloseSQLite(){
		if(is_sqlite_opened_ == true){
			sqlite3_close(db_handler_);
			is_sqlite_opened_ = false;
		}
	}

	//アクセサ
	bool is_sqlite_opened(){        return is_sqlite_opened_;}
	bool is_monogram_available(){   return is_monogram_available_;}
	bool is_bigram_available(){     return is_bigram_available_;}
	bool is_trigram_available(){    return is_trigram_available_;}


	void ClearTable();

	void AddWord(const std::string word);
	void AddWord(const std::wstring word){ AddWord(wstr_to_str(word)); };

 protected:
	std::vector<Ngram> ExtractNgram(const int n,const std::wstring& word);
	void AscSort(std::vector<Ngram>& ngrams){ std::sort(ngrams.begin(),ngrams.end()); }
	void DescSort(std::vector<Ngram>& ngrams){ std::sort(ngrams.begin(),ngrams.end(),std::greater<Ngram>()); }
	FRIEND_TEST(FuzzySynonyTest, ExtractNgramUnigram);
	FRIEND_TEST(FuzzySynonyTest, ExtractNgramBigram);
	FRIEND_TEST(FuzzySynonyTest, ExtractNgramTrigram);

 private:
	bool is_sqlite_opened_;
	bool is_monogram_available_;
	bool is_bigram_available_;
	bool is_trigram_available_;
	sqlite3* db_handler_;
};

#endif // FUZZYSYNONY_FUZZYSYNONY_HPP_