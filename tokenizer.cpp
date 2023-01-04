#include <cctype>
#include <tokenizer.h>

Tokenizer::Tokenizer(std::istream* in) : in_(in) {
    Next();
}

bool Tokenizer::IsEnd() {
    return is_end_;
}

inline bool IsStrBeg(char c) {
    return std::isalpha(c) || ('<' <= c && c <= '>') || c == '*' || c == '/' || c == '#';
}

inline bool IsStrContains(char c) {
    return IsStrBeg(c) || std::isdigit(c) || c == '?' || c == '!' || c == '-';
}

void Tokenizer::Next() {
    if (in_->peek() == EOF) {
        is_end_ = true;
        return;
    }

    char cur_c = in_->get();
    while (std::isspace(cur_c)) {
        cur_c = in_->get();
        if (cur_c == EOF) {
            is_end_ = true;
            return;
        }
    }

    if (cur_c == '(') {
        cur_ = BracketToken::OPEN;
        return;
    }
    if (cur_c == ')') {
        cur_ = BracketToken::CLOSE;
        return;
    }
    if (cur_c == '\'') {
        cur_ = QuoteToken();
        return;
    }
    if (cur_c == '.') {
        cur_ = DotToken();
        return;
    }
    if (cur_c == '#' && in_->peek() == 'f') {
        in_->get();
        cur_ = Boolean::FALSE;
        return;
    }
    if (cur_c == '#' && in_->peek() == 't') {
        in_->get();
        cur_ = Boolean::TRUE;
        return;
    }

    // makes a number out of a string
    auto read_num = [&](int& val) -> int {
        while (std::isdigit(in_->peek())) {
            val = 10 * val;
            val += val > 0 ? in_->get() - '0' : -in_->get() + '0';
        }
        return val;
    };

    // check for number token
    if (cur_c == '-' && std::isdigit(in_->peek())) {
        int val = -(in_->get() - '0');
        read_num(val);
        cur_ = ConstantToken{.value = val};
        return;
    }
    if (cur_c == '+' && std::isdigit(in_->peek())) {
        int val = in_->get() - '0';
        read_num(val);
        cur_ = ConstantToken{.value = val};
        return;
    }
    if (std::isdigit(cur_c)) {
        int val = cur_c - '0';
        read_num(val);
        cur_ = ConstantToken{.value = val};
        return;
    }
    if (cur_c == '-') {
        cur_ = SymbolToken{.name = "-"};
        return;
    }
    if (cur_c == '+') {
        cur_ = SymbolToken{.name = "+"};
        return;
    }
    if (IsStrBeg(cur_c)) {
        std::string str;
        str += cur_c;
        while (IsStrContains(in_->peek())) {
            str += in_->get();
        }
        cur_ = SymbolToken{.name = str};
        return;
    }
    throw SyntaxError("Unknown symbol");
}

Token Tokenizer::GetToken() {
    return cur_;
}