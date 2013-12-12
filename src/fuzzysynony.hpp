#ifndef FUZZYSYNONY_FUZZYSYNONY_HPP_
#define FUZZYSYNONY_FUZZYSYNONY_HPP_

#include "sqlite3.h"
#include "gtest/gtest_prod.h"
#include <string>
#include <vector>
#include <algorithm>
#include <functional>
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

//TODO　固定長文字列を実装（2バイト文字も1文字と数える）
struct Ngram{
	std::wstring index;
	int offset;
	bool operator<( const Ngram& right ) const{ return offset < right.offset; }
	bool operator>( const Ngram& right ) const{ return offset > right.offset; }
};

/*
inline std::wstring str_to_wstr (const std::string& str){
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return myconv.from_bytes(str);
}

inline std::string wstr_to_str (const std::wstring& str)
{
	std::wstring_convert<std::codecvt_utf8<wchar_t>> conv;
	return conv.to_bytes(str);
}*/

class FuzzySynony{
 public:

 	FuzzySynony() : is_sqlite_opened_(false){}
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
	bool is_sqlite_opened(){ return is_sqlite_opened_;}
 protected:
	std::vector<Ngram> ExtractNgram(const int n,const std::wstring& name);
	void AscSort(std::vector<Ngram>& ngrams){ std::sort(ngrams.begin(),ngrams.end()); }
	void DescSort(std::vector<Ngram>& ngrams){ std::sort(ngrams.begin(),ngrams.end(),std::greater<Ngram>()); }
	FRIEND_TEST(FuzzySynonyTest, ExtractNgramUnigram);
	FRIEND_TEST(FuzzySynonyTest, ExtractNgramBigram);
	FRIEND_TEST(FuzzySynonyTest, ExtractNgramTrigram);
 private:
 	bool is_sqlite_opened_;
	sqlite3* db_handler_;
};

#endif // FUZZYSYNONY_FUZZYSYNONY_HPP_