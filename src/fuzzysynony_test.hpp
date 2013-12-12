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
	std::vector<Ngram> ngrams;
	ngrams = fuzzysynony.ExtractNgram(n, aiueo);

	ASSERT_EQ(aiueo.length(), ngrams.size());
	//あ
	offset = 0;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);

	//う
	offset = 2;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);

	//お
	offset = 4;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);
}

TEST(FuzzySynonyTest, ExtractNgramBigram){

	std::wstring aiueo = L"あいうえお";
	int n = 2;
	int offset;

	FuzzySynony fuzzysynony;
	std::vector<Ngram> ngrams;
	ngrams = fuzzysynony.ExtractNgram(n, aiueo);

	ASSERT_EQ(aiueo.length()-n+1, ngrams.size());
	//あい
	offset = 0;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);

	//うえ
	offset = 2;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);

}

TEST(FuzzySynonyTest, ExtractNgramTrigram){

	std::wstring aiueo = L"あいうえお";
	int n = 3;
	int offset;

	FuzzySynony fuzzysynony;
	std::vector<Ngram> ngrams;
	ngrams = fuzzysynony.ExtractNgram(n, aiueo);

	ASSERT_EQ(aiueo.length()-n+1, ngrams.size());
	//あいう
	offset = 0;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);

	//うえお
	offset = 2;
	EXPECT_EQ(aiueo.substr(offset,n), ngrams[offset].index);
	EXPECT_EQ(offset, ngrams[offset].offset);

}

#endif // FUZZYSYNONY_FUZZYSYNONY_TEST_HPP_