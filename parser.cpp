#include <parser.h>
#include <vector>

std::shared_ptr<Object> Read(Tokenizer* tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Empty input");
    }

    auto result = ReadObject(tokenizer);

    if (!tokenizer->IsEnd()) {
        throw SyntaxError("Incorrect input");
    }

    return result;
}

std::shared_ptr<Object> ReadObject(Tokenizer* tokenizer) {
    Token cur_token = tokenizer->GetToken();
    tokenizer->Next();

    switch (cur_token.index()) {
        case 0:  // ConstantToken
        {
            return std::make_shared<Number>(Number(cur_token));
        }
        case 1:  // BracketToken
        {
            auto bracket = std::get<BracketToken>(cur_token);
            if (bracket == BracketToken::CLOSE) {
                throw SyntaxError("Wrong brackets order");
            }
            return ReadList(tokenizer);
        }
        case 2:  // SymbolToken
        {
            return std::make_shared<Symbol>(Symbol(cur_token));
        }
        case 3:  // QuoteToken
        {
            auto cell = std::make_shared<Cell>();
            cell->SetFirst(std::make_shared<Symbol>(Symbol(SymbolToken{.name = "quote"})));

            if (tokenizer->IsEnd()) {
                throw SyntaxError("Incorrect quote syntax");
            }

            cell->SetSecond(ReadObject(tokenizer));
            return cell;
        }
        case 4:  // DotToken
        {
            return std::make_shared<Dot>();
        }
        case 5:  // Boolean
        {
            return std::make_shared<Bool>(Bool(cur_token));
        }
        case std::variant_npos: {
            throw SyntaxError("Token missing");
        }
    }
    throw SyntaxError("Unknown Token");
}

std::shared_ptr<Object> ParseObjectList(const ObjectList& list, size_t start_ind = 0) {
    auto cell = std::make_shared<Cell>();
    if (!start_ind) {
        cell->SetExterior(true);
    }

    if (Is<Dot>(list[start_ind])) {
        throw SyntaxError("Incorrect Pair Syntax");
    }

    cell->SetFirst(list[start_ind++]);

    if (start_ind == list.size()) {
        cell->SetSecond(nullptr);
        return cell;
    }

    // pair or improper list
    if (Is<Dot>(list[start_ind])) {
        if (++start_ind == list.size() || start_ind != list.size() - 1) {
            throw SyntaxError("Incorrect Pair Syntax");
        }

        // not empty pair
        if (Is<Cell>(list[start_ind]) && !As<Cell>(list[start_ind])->GetFirst().get() &&
            !As<Cell>(list[start_ind])->GetSecond().get()) {
            cell->SetSecond(nullptr);
            return cell;
        }

        // short form for lists
        cell->SetSecond(list[start_ind]);
        if (Is<Cell>(cell->GetSecond()) && !Is<Cell>(cell->GetFirst())) {
            As<Cell>(cell->GetSecond())->SetExterior(false);
        }
        return cell;
    }

    cell->SetSecond(ParseObjectList(list, start_ind));
    return cell;
}

std::shared_ptr<Object> ReadList(Tokenizer* tokenizer) {
    ObjectList list;
    bool was_closing_bracket = false;
    while (!tokenizer->IsEnd()) {
        if (tokenizer->GetToken().index() == 1 &&
            std::get<BracketToken>(tokenizer->GetToken()) == BracketToken::CLOSE) {
            was_closing_bracket = true;
            tokenizer->Next();
            break;
        }
        list.push_back(ReadObject(tokenizer));
    }

    if (!was_closing_bracket) {
        throw SyntaxError("Incorrect list");
    }

    if (list.empty()) {
        return nullptr;
    }
    return ParseObjectList(list);
}
