#include "fuzzysynony.hpp"

int FuzzySynony::ExtractNgram(const int n,const std::wstring& word, std::vector<WNgram>& ret_wngrams){
	int word_size = word.size()-n+1;
	ret_wngrams.resize(word_size);
	for(int i = 0; i < word_size; i++){
		ret_wngrams.at(i).index = word.substr(i,n);
		ret_wngrams[i].offset = i;
	}
	return true;
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

	//バイグラムインデックス
	const std::string SQL_CREATE_INDEX_BIGRAM = "CREATE INDEX BIGRAMINDEX ON BIGRAM(GRAM);";
	
	sqlite3_exec(db_handler_, SQL_CREATE_TABLE_WORDS.c_str(), NULL, NULL, NULL);
	sqlite3_exec(db_handler_, SQL_CREATE_TABLE_BIGRAM.c_str(), NULL, NULL, NULL);
	sqlite3_exec(db_handler_, SQL_CREATE_INDEX_BIGRAM.c_str(), NULL, NULL, NULL);
}


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
		std::vector<WNgram> wngrams;
		ExtractNgram(2, str_to_wstr(word), wngrams);
		std::wostringstream wss_sql_buf;
		if(0 < wngrams.size()){
			for(int i = 0; i < wngrams.size(); i++){
				wss_sql_buf << L"INSERT INTO BIGRAM VALUES('"
				<< wngrams[i].index << L"', " << word_id << L", " << wngrams[i].offset << L");";
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


int FuzzySynony::SearchWords(const std::vector<int>& wordids, std::unordered_map<int,std::string>& ret_words){
	int word_id_size = wordids.size();
	if(word_id_size == 0){
		return SQLITE_OK;
	}else{

		std::ostringstream ss_sql_buf;
		ss_sql_buf << "SELECT * FROM WORDS WHERE ID IN (";
		for(int i = 0; i < (word_id_size-1); i++){
			ss_sql_buf << wordids[i] << ", ";
		}
		ss_sql_buf << wordids.back() << ");";

		sqlite3_exec(db_handler_, ss_sql_buf.str().c_str(),
			//anonymous function begin
			[&] (void *arg_words,int cols, char **values, char **columns) -> int {
				std::unordered_map<int,std::string> *words = static_cast< std::unordered_map<int,std::string>* >(arg_words);
				IdWord idword;

				for(int i = 0; i < cols; i++){
					std::string column(columns[i]);
					if(column == "ID"){
						idword.id = atoi(values[i]);
					}else if(column == "WORD"){
						idword.word = values[i];
					}else{}
				}
				(*words)[idword.id] = idword.word;
				return SQLITE_OK;
			}//anonymous function end
			, &ret_words ,NULL);
	}
	return SQLITE_OK;
}


int FuzzySynony::SearchNgrams(const std::string word, std::vector<DbNgram>& ret_dbngrams){
	std::vector<WNgram> wngrams;
	ExtractNgram(2, str_to_wstr(word), wngrams);
	int wngrams_size = wngrams.size();
	ret_dbngrams.reserve(wngrams_size*10);
	std::ostringstream ss_sql_buf;
	ss_sql_buf << "SELECT * FROM BIGRAM WHERE GRAM IN (\"";
	for(int i = 0; i < (wngrams_size-1); i++){
		ss_sql_buf << wstr_to_str(wngrams[i].index) << "\", \"";
	}
	ss_sql_buf << wstr_to_str(wngrams.back().index) << "\");";

	sqlite3_exec(db_handler_, ss_sql_buf.str().c_str(),
		//anonymous function begin
		[&] (void *arg_dbngrams, int cols, char **values, char **columns) -> int{
			std::vector<DbNgram> *dbngrams = static_cast< std::vector<DbNgram>* >(arg_dbngrams);
			DbNgram dbngram;

			for(int i = 0; i < cols; i++){
				std::string column(columns[i]);
				if(column == "GRAM"){
					dbngram.index = values[i];
				}else if(column == "WORDID"){
					dbngram.wordid = atoi(values[i]);
				}else if(column == "OFFSET"){
					dbngram.offset = atoi(values[i]);
				}
			}
			dbngrams->push_back(dbngram);
			return SQLITE_OK;
		}//anonymous function end
		, &ret_dbngrams, NULL);
	
	return SQLITE_OK;
}

int  FuzzySynony::Search(const std::string text, std::vector<FSResult>& fsresult){
	const std::string SUM = "";
	std::vector<WNgram> wngrams;
	ExtractNgram(2, str_to_wstr(text), wngrams);
	std::unordered_map<std::string,int> text_scores;
	for(int i = 0; i < wngrams.size(); i++){
		text_scores[wstr_to_str(wngrams[i].index)]++;
		text_scores[SUM]++;
	}

	std::vector<DbNgram> dbngrams;
	SearchNgrams(text, dbngrams);
	std::unordered_map< int, std::unordered_map<std::string,int> > scores;
	for(int i = 0; i < dbngrams.size(); i++){
		if(scores[dbngrams[i].wordid][dbngrams[i].index] < text_scores[dbngrams[i].index]){
			scores[dbngrams[i].wordid][dbngrams[i].index]++;
			scores[dbngrams[i].wordid][SUM]++;
		}
	}

	std::unordered_map< int, std::unordered_map<std::string, int> >::iterator sitr = scores.begin();

	std::vector<int> wordids;
	std::vector<int> perfect_matched;
	while(sitr != scores.end()){
		wordids.push_back(sitr->first);
		if(sitr->second[SUM]==text_scores[SUM]){
			perfect_matched.push_back(sitr->first);
		}
		sitr++;
	}
	std::unordered_map<int,std::string> idwords;
	SearchWords(wordids, idwords);

	for(int i = 0; i < perfect_matched.size(); i++){
		if(idwords[perfect_matched[i]] == text){
			scores[perfect_matched[i]][SUM] = 2*scores[perfect_matched[i]][SUM] + 1;
		}
	}

	fsresult.reserve(scores.size());
	sitr = scores.begin();
	while(sitr != scores.end()){
		fsresult.push_back(FSResult(idwords[sitr->first], sitr->second[SUM]));
		sitr++;
	}
	DescSort(fsresult);
}