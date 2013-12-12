#include "gtest/gtest.h"
#include "sqlite3.h"

#include "fuzzysynony_test.hpp"

TEST(SQLiteTest, SQLiteOpenClose){
	sqlite3* pDB;
	EXPECT_EQ(SQLITE_OK, sqlite3_open("test.db", &pDB));
	EXPECT_EQ(SQLITE_OK, sqlite3_close(pDB));
}

int main(int argc, char **argv){
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}

