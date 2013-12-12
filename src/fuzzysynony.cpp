#include "fuzzysynony.hpp"

std::vector<Ngram> FuzzySynony::ExtractNgram(const int n,const std::wstring& name){
	std::vector<Ngram> ngrams;
	int name_size = name.size()-n+1;
	ngrams.resize(name_size);
	for(int i = 0; i < name_size; i++){
		ngrams.at(i).index = name.substr(i,n);
		ngrams[i].offset = i;
	}
	return ngrams;
}
