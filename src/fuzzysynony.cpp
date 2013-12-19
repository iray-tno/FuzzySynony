#include "fuzzysynony.hpp"

std::vector<Ngram> FuzzySynony::ExtractNgram(const int n,const std::wstring& word){
	std::vector<Ngram> ngrams;
	int word_size = word.size()-n+1;
	ngrams.resize(word_size);
	for(int i = 0; i < word_size; i++){
		ngrams.at(i).index = word.substr(i,n);
		ngrams[i].offset = i;
	}
	return ngrams;
}

void FuzzySynony::ClearTable(){
	// 検索対象文字列テーブル
	const std::string SQL_CREATE_TABLE_WORDS = "CREATE TABLE WORDS("
	    "ID INTEGER PRIMARY KEY UNIQUE,"
	    "WORD TEXT NOT NULL UNIQUE ON CONFLICT IGNORE"
	");";
	//バイグラムテーブル
	const std::string SQL_CREATE_TABLE_BIGRAM = "CREATE TABLE BIGRAM("
	    "GRAM TEXT NOT NULL,"
	    "WORDID INTEGER NOT NULL,"
	    "OFFSET INTEGER NOT NULL,"
	    "PRIMARY KEY(WORDID, OFFSET) ON CONFLICT IGNORE"
	");";

	const std::string SQL_CREATE_INDEX_BIGRAM = "CREATE INDEX BIGRAMINDEX ON BIGRAM(GRAM);";
	//CREATE TABLE BIGRAM(GRAM TEXT NOT NULL,WORDID INTEGER NOT NULL,OFFSET INTEGER NOT NULL,PRIMARY KEY(WORDID, OFFSET) ON CONFLICT IGNORE);
	
	sqlite3_exec(db_handler_, SQL_CREATE_TABLE_WORDS.c_str(), NULL, NULL, NULL);
	sqlite3_exec(db_handler_, SQL_CREATE_TABLE_BIGRAM.c_str(), NULL, NULL, NULL);
	sqlite3_exec(db_handler_, SQL_CREATE_INDEX_BIGRAM.c_str(), NULL, NULL, NULL);
}

//void FuzzySynony::AddWord(const std::wstring word){}


void FuzzySynony::AddWord(const std::string word){
	const std::string SQL_INSERT_WORD = "INSERT INTO WORDS VALUES(NULL,'"+word+"');";
	const std::string SQL_SELECT_WORD_ID = "SELECT ID FROM WORDS WHERE WORD = '"+word+"';";
	int word_id = 0;

	sqlite3_exec(db_handler_,SQL_INSERT_WORD.c_str() ,NULL, NULL, NULL);
	sqlite3_exec(db_handler_, SQL_SELECT_WORD_ID.c_str(),
		//anonymous function begin
		[&] (void *arg_id ,int cols, char **values, char **columns) -> int {
			int *id = static_cast<int*>(arg_id);
			for(int i = 0; i < cols; i++){
				*id = atoi(values[i]);
			}
			return SQLITE_OK;
		}//anonymous function end
	, &word_id ,NULL);

	if(is_bigram_available_){
		std::vector<Ngram> ngrams = ExtractNgram(2,str_to_wstr(word));
		std::wostringstream wss_sql_buf;
		if(0 < ngrams.size()){
			for(int i = 0; i < ngrams.size(); i++){
				wss_sql_buf << L"INSERT INTO BIGRAM VALUES('"
				<< ngrams[i].index << L"', " << word_id << L", " << ngrams[i].offset << L");";
std::cout << wstr_to_str(wss_sql_buf.str()).c_str() << std::endl;
				sqlite3_exec(db_handler_, wstr_to_str(wss_sql_buf.str()).c_str(), NULL, NULL, NULL);
				wss_sql_buf.str(L"");
				wss_sql_buf.clear(); 
			}
		}else{}
		/*
		if(2 <= ngrams.size()){

			wss_sql_buf << L"INSERT INTO BIGRAM VALUES";
			for(int i = 0; i < ngrams.size()-1; i++){
			wss_sql_buf << L"('" << ngrams[i].index << L"', " << word_id << L", " << ngrams[i].offset << L"), ";
			}

			wss_sql_buf << L"('" << ngrams.back().index << L"', " << word_id << L", " << ngrams.back().offset << L"));";
			sqlite3_exec(db_handler_, wstr_to_str(wss_sql_buf.str()).c_str(), NULL, NULL, NULL);
			std::cout << wstr_to_str(wss_sql_buf.str());
		}else if(1 == ngrams.size()){

			wss_sql_buf << L"INSERT INTO BIGRAM VALUES('"
				<< ngrams.front().index << L"', " << word_id << L", " << ngrams.front().offset << L");";
			sqlite3_exec(db_handler_, wstr_to_str(wss_sql_buf.str()).c_str(), NULL, NULL, NULL);

		}else{}
		*/
	}
}
