#pragma once
#include <string>

namespace utils {

	class StringTokenizer;

	class TokenIterator {
		const StringTokenizer& tokenizer_;
		std::size_t prev, pos;

	public:
		TokenIterator(const StringTokenizer& tokenizer, std::size_t start);

		//set binary mode
		void operator++();

		std::string operator*();
		std::string operator->() { return this->operator*(); }

		bool operator==(const TokenIterator& other);
		bool operator!=(const TokenIterator& other) { return !((*this) == other); }

	};

}