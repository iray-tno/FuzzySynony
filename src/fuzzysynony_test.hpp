#ifndef FUZZYSYNONY_FUZZYSYNONY_TEST_HPP_
#define FUZZYSYNONY_FUZZYSYNONY_TEST_HPP_

#include <string>
#include "gtest/gtest.h"
#include "sqlite3.h"
#include "fuzzysynony.hpp"


TEST(FuzzySynonyTest, ExtractNgramUnigram){

	std::wstring aiueo = L"あいうえお";
	int n = 1;
	int offset;

	FuzzySynony fuzzysynony;
	std::vector<WNgram> ret_wngrams;
	fuzzysynony.ExtractNgram(n, aiueo, ret_wngrams);

	ASSERT_EQ(aiueo.length(), ret_wngrams.size());
	//あ
	offset = 0;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);

	//う
	offset = 2;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);

	//お
	offset = 4;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);
}

TEST(FuzzySynonyTest, ExtractNgramBigram){

	std::wstring aiueo = L"あいうえお";
	int n = 2;
	int offset;

	FuzzySynony fuzzysynony;
	std::vector<WNgram> ret_wngrams;
	fuzzysynony.ExtractNgram(n, aiueo, ret_wngrams);

	ASSERT_EQ(aiueo.length()-n+1, ret_wngrams.size());
	//あい
	offset = 0;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);

	//うえ
	offset = 2;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);

}

TEST(FuzzySynonyTest, ExtractNgramTrigram){

	std::wstring aiueo = L"あいうえお";
	int n = 3;
	int offset;

	FuzzySynony fuzzysynony;
	std::vector<WNgram> ret_wngrams;
	fuzzysynony.ExtractNgram(n, aiueo, ret_wngrams);

	ASSERT_EQ(aiueo.length()-n+1, ret_wngrams.size());
	//あいう
	offset = 0;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);

	//うえお
	offset = 2;
	EXPECT_EQ(aiueo.substr(offset,n), ret_wngrams[offset].index);
	EXPECT_EQ(offset, ret_wngrams[offset].offset);

}

TEST(FuzzySynonyTest, SearchWords){
	std::vector<std::string> words;
	std::vector<int> ids;
	std::vector<IdWord> idwords;

	std::vector<IdWord> ret;

	FuzzySynony fs("SearchWords.db");
	fs.ClearTable();
	words.push_back("あいうえお");
	words.push_back("あかさたな");
	words.push_back("かきくけこ");
	words.push_back("あいaえお");
	words.push_back("あか塩たな");
	words.push_back("かきがけこ");
	words.push_back("高橋");
	words.push_back("田中");
	words.push_back("鈴木");
	words.push_back("中田");

	for(int i = 0; i < words.size(); i++){
		IdWord idword;
		idword.id = i+1;
		idword.word = words[i];
		ids.push_back(idword.id);
		idwords.push_back(idword);
		fs.AddWord(idword.word);
	}

	fs.SearchWords(ids, ret);

	EXPECT_EQ(idwords.size(), ret.size());

	for(int i = 0; i < idwords.size(); i++){
		//TODO: サイズ比較とソートで一意になるようにする
		EXPECT_EQ(idwords[i].id,ret[i].id);
		EXPECT_EQ(idwords[i].word,ret[i].word);
	}

}

TEST(FuzzySynonyTest, SearchNgrams){
	std::vector<std::string> words;
	std::vector<int> ids;
	std::vector<IdWord> idwords;

	std::vector<DbNgram> ret_dbngrams;

	FuzzySynony fs("SearchNgrams.db");
	fs.ClearTable();
	words.push_back("あいうえお");
	words.push_back("あかさたな");
	words.push_back("かきくけこ");
	words.push_back("あいaえお");
	words.push_back("あか塩たな");
	words.push_back("かきがけこ");
	words.push_back("高橋");
	words.push_back("田中");
	words.push_back("鈴木");
	words.push_back("中田");

	for(int i = 0; i < words.size(); i++){
		IdWord idword;
		idword.id = i+1;
		idword.word = words[i];
		ids.push_back(idword.id);
		idwords.push_back(idword);
		fs.AddWord(idword.word);
	}

	std::vector<DbNgram> expected_dbngrams;
	expected_dbngrams.resize(6);
	expected_dbngrams[0].index = "あい";
	expected_dbngrams[0].wordid = 1;
	expected_dbngrams[0].offset = 0;
	expected_dbngrams[1].index = "あい";
	expected_dbngrams[1].wordid = 4;
	expected_dbngrams[1].offset = 0;
	expected_dbngrams[2].index = "いう";
	expected_dbngrams[2].wordid = 1;
	expected_dbngrams[2].offset = 1;
	expected_dbngrams[3].index = "うえ";
	expected_dbngrams[3].wordid = 1;
	expected_dbngrams[3].offset = 2;
	expected_dbngrams[4].index = "えお";
	expected_dbngrams[4].wordid = 1;
	expected_dbngrams[4].offset = 3;
	expected_dbngrams[5].index = "えお";
	expected_dbngrams[5].wordid = 4;
	expected_dbngrams[5].offset = 3;


	fs.SearchNgrams("あいうえお", ret_dbngrams);

	EXPECT_EQ(expected_dbngrams.size(), ret_dbngrams.size());

	for(int i = 0; i < ret_dbngrams.size(); i++){
		//TODO: サイズ比較とソートで一意になるようにする
		EXPECT_EQ(expected_dbngrams[i].index, ret_dbngrams[i].index);
		EXPECT_EQ(expected_dbngrams[i].wordid, ret_dbngrams[i].wordid);
		EXPECT_EQ(expected_dbngrams[i].offset, ret_dbngrams[i].offset);
	}
	//TODO: サイズ比較とソートで一意になるようにする

}

#endif // FUZZYSYNONY_FUZZYSYNONY_TEST_HPP_