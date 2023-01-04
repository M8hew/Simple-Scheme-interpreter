#include "object.h"

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter"
std::shared_ptr<Object> Dot::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Dot>();
}

std::string Dot::Serialize() {
    return ".";
}

Number::Number(int64_t value) : value_(value) {
}

Number::Number(const Token& token) : value_(std::get<ConstantToken>(token).value) {
}

Number::Number(const Number& other) : value_(other.value_) {
}

Number::Number(Number&& other) noexcept : value_(other.value_) {
}

Number& Number::operator=(const Number& other) {
    value_ = other.value_;
    return *this;
}

Number& Number::operator=(Number&& other) noexcept {
    std::swap(value_, other.value_);
    return *this;
}

std::shared_ptr<Object> Number::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Number>(Number(*this));
}

std::string Number::Serialize() {
    return std::to_string(value_);
}

int64_t Number::GetValue() const {
    return value_;
}

Bool::Bool(bool inp) : value_(inp) {
}

Bool::Bool(const Token& token) : value_(std::get<Boolean>(token) == Boolean::TRUE) {
}

Bool::Bool(const Bool& other) : value_(other.value_) {
}

Bool::Bool(Bool&& other) noexcept : value_(other.value_) {
}

Bool& Bool::operator=(const Bool& other) {
    value_ = other.value_;
    return *this;
}

Bool& Bool::operator=(Bool&& other) noexcept {
    value_ = other.value_;
    return *this;
}

std::shared_ptr<Object> Bool::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Bool>(Bool(*this));
}

std::string Bool::Serialize() {
    return value_ ? "#t" : "#f";
}

bool Bool::GetBool() const {
    return value_;
}

std::shared_ptr<Object> Quote::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Quote>();
}

std::string Quote::Serialize() {
    return "quote";
}

std::shared_ptr<Object> Quote::Apply(const ObjectList& args) {
    throw RuntimeError("Special form 'Quote' should never be applied");
}

std::shared_ptr<Object> IsNum::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<IsNum>();
}

std::string IsNum::Serialize() {
    return "number?";
}

std::shared_ptr<Object> IsNum::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'number?'");
    }
    return Bool(Is<Number>(args.front())).Eval(nullptr);
}

std::shared_ptr<Object> IsBool::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<IsBool>();
}

std::string IsBool::Serialize() {
    return "boolean?";
}

std::shared_ptr<Object> IsBool::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'boolean?'");
    }
    return Bool(Is<Bool>(args.front())).Eval(nullptr);
}

std::shared_ptr<Object> Abs::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Abs>();
}

std::string Abs::Serialize() {
    return "number?";
}

std::shared_ptr<Object> Abs::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'abs'");
    }
    if (Is<Number>(args.front())) {
        int64_t result = As<Number>(args.front())->GetValue();
        result = result < 0 ? -result : result;
        return Number(result).Eval(nullptr);
    } else {
        throw RuntimeError("'abs' function only works with numbers");
    }
}

std::shared_ptr<Object> Sum::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Sum>();
}

std::string Sum::Serialize() {
    return "+";
}

std::shared_ptr<Object> Sum::Apply(const ObjectList& args) {
    int64_t sum = 0;
    for (auto ptr : args) {
        if (!Is<Number>(ptr)) {
            throw RuntimeError("Wrong type argument in '+' operator");
        }
        sum += As<Number>(ptr)->GetValue();
    }
    return std::make_shared<Number>(sum);
}

std::shared_ptr<Object> Dif::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Dif>();
}

std::string Dif::Serialize() {
    return "-";
}

std::shared_ptr<Object> Dif::Apply(const ObjectList& args) {
    if (args.empty()) {
        throw RuntimeError("Can't apply operator '-' without args");
    }
    if (!Is<Number>(args.front())) {
        throw RuntimeError("Wrong type argument in '-' operator");
    }
    int64_t dif = As<Number>(args.front())->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Wrong type argument in '-' operator");
        }
        dif -= As<Number>(args[i])->GetValue();
    }
    return std::make_shared<Number>(dif);
}

std::shared_ptr<Object> Prod::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Prod>();
}

std::string Prod::Serialize() {
    return "*";
}

std::shared_ptr<Object> Prod::Apply(const ObjectList& args) {
    int64_t prod = 1;
    for (auto ptr : args) {
        if (!Is<Number>(ptr)) {
            throw RuntimeError("Wrong type argument in '*' operator");
        }
        prod *= As<Number>(ptr)->GetValue();
    }
    return std::make_shared<Number>(prod);
}

std::shared_ptr<Object> Div::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Div>();
}

std::string Div::Serialize() {
    return "/";
}

std::shared_ptr<Object> Div::Apply(const ObjectList& args) {
    if (args.empty()) {
        throw RuntimeError("Can't apply operator '/' without args");
    }
    if (!Is<Number>(args.front())) {
        throw RuntimeError("Wrong type argument in '/' operator");
    }
    int64_t div = As<Number>(args.front())->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Wrong type argument in '/' operator");
        }
        div /= As<Number>(args[i])->GetValue();
    }
    return std::make_shared<Number>(div);
}

std::shared_ptr<Object> Min::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Min>();
}

std::string Min::Serialize() {
    return "min";
}

std::shared_ptr<Object> Min::Apply(const ObjectList& args) {
    if (args.empty()) {
        throw RuntimeError("Can't apply 'min' function without args");
    }
    if (!Is<Number>(args.front())) {
        throw RuntimeError("Wrong type 'min' argument");
    }
    int64_t min = As<Number>(args.front())->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Wrong type 'min' argument");
        }
        min = std::min(min, As<Number>(args[i])->GetValue());
    }
    return std::make_shared<Number>(min);
}

std::shared_ptr<Object> Max::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Max>();
}

std::string Max::Serialize() {
    return "max";
}

std::shared_ptr<Object> Max::Apply(const ObjectList& args) {
    if (args.empty()) {
        throw RuntimeError("Can't apply 'max' function without args");
    }
    if (!Is<Number>(args.front())) {
        throw RuntimeError("Wrong type 'max' argument");
    }
    int64_t max = As<Number>(args.front())->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Wrong type 'max' argument");
        }
        max = std::max(max, As<Number>(args[i])->GetValue());
    }
    return std::make_shared<Number>(max);
}

Comp::Comp(std::string inp) : comp_(get_lambda[inp]), type_(inp) {
}

Comp::Comp(const Comp& other) : comp_(other.comp_), type_(other.type_) {
}

Comp::Comp(Comp&& other) noexcept : comp_(other.comp_), type_(std::move(other.type_)) {
    other.comp_ = nullptr;
}

Comp& Comp::operator=(const Comp& other) {
    comp_ = other.comp_;
    type_ = other.type_;
    return *this;
}

Comp& Comp::operator=(Comp&& other) noexcept {
    comp_ = other.comp_;
    other.comp_ = nullptr;
    type_ = other.type_;
    return *this;
}

std::shared_ptr<Object> Comp::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Comp>(Comp(type_));
}

std::string Comp::Serialize() {
    return type_;
}

std::shared_ptr<Object> Comp::Apply(const ObjectList& args) {
    if (args.empty()) {
        return std::make_shared<Bool>(true);
    }

    if (!Is<Number>(args.front())) {
        throw RuntimeError("Wrong type argument in compare operator");
    }

    for (size_t i = 1; i < args.size(); ++i) {
        if (!Is<Number>(args[i])) {
            throw RuntimeError("Wrong type argument in compare operator");
        }
        if (!comp_(As<Number>(args[i - 1])->GetValue(), As<Number>(args[i])->GetValue())) {
            return std::make_shared<Bool>(false);
        }
    }
    return std::make_shared<Bool>(true);
}

std::shared_ptr<Object> Not::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Not>();
}

std::string Not::Serialize() {
    return "not";
}

std::shared_ptr<Object> Not::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in 'not' operator");
    }
    bool arg_bool_val = !Is<Bool>(args.front()) || As<Bool>(args.front())->GetBool();
    return std::make_shared<Bool>(!arg_bool_val);
}

std::shared_ptr<Object> And::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<And>();
}

std::string And::Serialize() {
    return "and";
}

std::shared_ptr<Object> And::Apply(const ObjectList& args) {
    if (args.empty()) {
        return std::make_shared<Bool>(true);
    }

    bool expression_result = true;
    for (size_t i = 0; i < args.size() - 1; ++i) {
        bool arg_bool_val = !Is<Bool>(args.front()) || As<Bool>(args.front())->GetBool();
        expression_result &= arg_bool_val;
    }

    return expression_result ? args.back() : std::make_shared<Bool>(false);
}

std::shared_ptr<Object> Or::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Or>();
}

std::string Or::Serialize() {
    return "or";
}

std::shared_ptr<Object> Or::Apply(const ObjectList& args) {
    if (args.empty()) {
        return std::make_shared<Bool>(false);
    }

    bool expression_result = true;
    for (size_t i = 0; i < args.size() - 1; ++i) {
        bool arg_bool_val = !Is<Bool>(args.front()) || As<Bool>(args.front())->GetBool();
        expression_result |= arg_bool_val;
    }

    return expression_result ? args.back() : std::make_shared<Bool>(false);
}

void Cell::SetExterior(bool is_exterior) {
    is_exterior_ = is_exterior;
}

void Cell::SetFirst(const std::shared_ptr<Object>& first) {
    first_ = first;
}

void Cell::SetSecond(const std::shared_ptr<Object>& second) {
    second_ = second;
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_;
}

bool Cell::IsExterior() {
    return is_exterior_;
}

std::shared_ptr<Object> Cell::Eval(std::shared_ptr<Scope> scope) {
    auto cell = std::make_shared<Cell>();
    cell->SetExterior(is_exterior_);

    if (first_.get()) {
        cell->SetFirst(first_->Eval(scope));
    }
    if (second_.get()) {
        cell->SetSecond(second_->Eval(scope));
    }
    return cell;
}

std::string Cell::Serialize() {
    std::string serialization_result;
    std::string sep =
        (first_.get() && !Is<Cell>(first_) && second_.get() && !Is<Cell>(second_)) ? " . " : " ";

    if (first_.get()) {
        serialization_result = first_->Serialize();
    } else {
        serialization_result = "()";
    }

    if (second_.get()) {
        serialization_result += sep + second_->Serialize();
    }
    return is_exterior_ ? "(" + serialization_result + ")" : serialization_result;
}

std::shared_ptr<Object> IsPair::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<IsPair>();
}

std::string IsPair::Serialize() {
    return "pair?";
}

std::shared_ptr<Object> IsPair::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'pair?'");
    }

    if (!Is<Cell>(args.front())) {
        return std::make_shared<Bool>(false);
    }

    std::shared_ptr<Object> first = As<Cell>(args.front())->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(args.front())->GetSecond();

    // check for (1 . 2)
    bool is_dot_pair = first.get() && !Is<Cell>(first) && second.get() && !Is<Cell>(second);
    if (is_dot_pair) {
        return std::make_shared<Bool>(true);
    }

    // check for (1 2) aka (1 (2 ()))
    bool is_list_pair = first.get() && !Is<Cell>(first) && second.get() && Is<Cell>(second) &&
                        As<Cell>(second)->GetFirst().get() &&
                        !Is<Cell>(As<Cell>(second)->GetFirst()) &&
                        !As<Cell>(second)->GetSecond().get();
    return std::make_shared<Bool>(is_list_pair);
}

std::shared_ptr<Object> IsNull::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<IsNull>();
}

std::string IsNull::Serialize() {
    return "null?";
}

std::shared_ptr<Object> IsNull::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'null?'");
    }

    if (!Is<Cell>(args.front())) {
        return std::make_shared<Bool>(false);
    }

    std::shared_ptr<Object> first = As<Cell>(args.front())->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(args.front())->GetSecond();

    return std::make_shared<Bool>(!first.get() && !second.get());
}

std::shared_ptr<Object> IsList::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<IsList>();
}

std::string IsList::Serialize() {
    return "list?";
}

std::shared_ptr<Object> IsList::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'list?'");
    }
    if (!Is<Cell>(args.front())) {
        return std::make_shared<Bool>(false);
    }
    return std::make_shared<Bool>(RecursiveCheck(args.front()));
}

bool IsList::RecursiveCheck(std::shared_ptr<Object> ast) {
    if (!Is<Cell>(ast)) {
        return false;
    }

    std::shared_ptr<Object> first = As<Cell>(ast)->GetFirst();
    std::shared_ptr<Object> second = As<Cell>(ast)->GetSecond();

    if (!first.get()) {
        return !second.get();
    }
    if (Is<Cell>(first)) {
        return false;
    }
    if (!second.get()) {
        return true;
    }
    return Is<Cell>(second) && RecursiveCheck(second);
}

std::shared_ptr<Object> Cons::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Cons>();
}

std::string Cons::Serialize() {
    return "cons";
}

std::shared_ptr<Object> Cons::Apply(const ObjectList& args) {
    if (args.size() != 2) {
        throw RuntimeError("Wrong number of argument in function 'cons'");
    }
    auto cell = std::make_shared<Cell>();
    cell->SetExterior(true);
    cell->SetFirst(args.front());
    cell->SetSecond(args.back());
    return cell;
}

std::shared_ptr<Object> Car::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Car>();
}

std::string Car::Serialize() {
    return "car";
}

std::shared_ptr<Object> Car::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'car'");
    }
    if (!Is<Cell>(args.front())) {
        throw RuntimeError("Wrong type argument in function 'car'");
    }
    if (!As<Cell>(args.front())->GetFirst().get()) {
        throw RuntimeError("Empty list can't be used as function 'car' argument");
    }
    return As<Cell>(args.front())->GetFirst();
}

std::shared_ptr<Object> Cdr::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Cdr>();
}

std::string Cdr::Serialize() {
    return "cdr";
}

std::shared_ptr<Object> Cdr::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'cdr'");
    }

    if (!Is<Cell>(args.front())) {
        throw RuntimeError("Wrong type argument in function 'cdr'");
    }

    if (!As<Cell>(args.front())->GetFirst().get()) {
        throw RuntimeError("Empty list can't be used as function 'cdr' argument");
    }

    if (!As<Cell>(args.front())->GetSecond().get()) {
        return std::make_shared<Cell>();
    }

    std::shared_ptr<Object> second = As<Cell>(args.front())->GetSecond();
    if (Is<Cell>(second)) {
        As<Cell>(second)->SetExterior(true);
    }
    return second;
}

std::shared_ptr<Object> List::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<List>();
}

std::string List::Serialize() {
    return "list";
}

std::shared_ptr<Object> List::Apply(const ObjectList& args) {
    if (args.empty()) {
        return std::make_shared<Cell>();
    }

    auto resulting_list = std::make_shared<Cell>();
    resulting_list->SetExterior(true);

    resulting_list->SetFirst(args.front());
    RecursiveListContraction(args, 1, resulting_list);
    return resulting_list;
}

void List::RecursiveListContraction(const ObjectList& args, size_t ind,
                                    std::shared_ptr<Cell>& last_cell) {
    if (ind == args.size()) {
        return;
    }

    auto cur_cell = std::make_shared<Cell>();
    cur_cell->SetFirst(args[ind]);
    last_cell->SetSecond(cur_cell);

    RecursiveListContraction(args, ind + 1, cur_cell);
}

std::shared_ptr<Object> ListRef::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<ListRef>();
}

std::string ListRef::Serialize() {
    return "list-ref";
}

std::shared_ptr<Object> ListRef::Apply(const ObjectList& args) {
    if (args.size() != 2) {
        throw RuntimeError("Wrong number of argument in function 'list-ref'");
    }
    return RecursiveListSearch(args.front(), As<Number>(args.back())->GetValue());
}

std::shared_ptr<Object> ListRef::RecursiveListSearch(std::shared_ptr<Object> list, const size_t ind,
                                                     size_t cur_step) {
    if (!Is<Cell>(list)) {
        throw RuntimeError("Wrong type argument in function 'list-ref'");
    }
    if (ind == cur_step) {
        return As<Cell>(list)->GetFirst();
    }
    if (!As<Cell>(list)->GetSecond().get()) {
        throw RuntimeError("Index out of range");
    }
    return RecursiveListSearch(As<Cell>(list)->GetSecond(), ind, cur_step + 1);
}

std::shared_ptr<Object> ListTail::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<ListTail>();
}

std::string ListTail::Serialize() {
    return "list-tail";
}

std::shared_ptr<Object> ListTail::Apply(const ObjectList& args) {
    if (args.size() != 2) {
        throw RuntimeError("Wrong number of argument in function 'list-tail'");
    }
    return RecursiveListSearch(args.front(), As<Number>(args.back())->GetValue());
}

std::shared_ptr<Object> ListTail::RecursiveListSearch(std::shared_ptr<Object> list,
                                                      const size_t ind, size_t cur_step) {
    if (!list.get()) {
        if (ind == cur_step) {
            return std::make_shared<Cell>();
        }
        throw RuntimeError("Index out of range");
    }

    if (!Is<Cell>(list)) {
        throw RuntimeError("Wrong type argument in function 'list-ref'");
    }

    if (ind == cur_step) {
        As<Cell>(list)->SetExterior(true);
        return list;
    }
    return RecursiveListSearch(As<Cell>(list)->GetSecond(), ind, cur_step + 1);
}

Symbol::Symbol(const std::string& str) : str_(str) {
}

Symbol::Symbol(const Token& token) : str_(std::get<SymbolToken>(token).name) {
}

Symbol::Symbol(const Symbol& other) : str_(other.str_) {
}

Symbol::Symbol(Symbol&& other) noexcept : str_(move(other.str_)) {
}

Symbol& Symbol::operator=(const Symbol& other) {
    str_ = other.str_;
    return *this;
}

Symbol& Symbol::operator=(Symbol&& other) noexcept {
    str_ = move(other.str_);
    return *this;
}

std::shared_ptr<Object> Symbol::Eval(std::shared_ptr<Scope> scope) {
    if (!scope.get()) {
        return std::make_shared<Symbol>(*this);
    }
    return scope->ResolveSymbol(str_);
}

std::string Symbol::Serialize() {
    return str_;
}
const std::string& Symbol::GetName() const {
    return str_;
}

Scope::Scope(StringFuncMap&& inp, std::shared_ptr<Scope> par_scope) noexcept
    : parent_scope_(par_scope), scope_(inp) {
}

bool Scope::DefineSymbol(const std::string& symbol, std::shared_ptr<Object> ptr) {
    scope_[symbol] = ptr;
    return true;
}

std::shared_ptr<Object> Scope::ResolveSymbol(const std::string& symbol) {
    if (scope_.contains(symbol)) {
        return scope_[symbol];
    }
    if (!parent_scope_.get()) {
        throw NameError("SymbolsAreNotSelfEvaluating");
    }
    return parent_scope_->ResolveSymbol(symbol);
}

Scope::Scope(const Scope& other) : parent_scope_(other.parent_scope_), scope_(other.scope_) {
}

Scope::Scope(Scope&& other) noexcept
    : parent_scope_(move(other.parent_scope_)), scope_(move(other.scope_)) {
}

Scope& Scope::operator=(const Scope& other) {
    scope_ = other.scope_;
    parent_scope_ = other.parent_scope_;
    return *this;
}

Scope& Scope::operator=(Scope&& other) noexcept {
    scope_ = move(other.scope_);
    parent_scope_ = move(other.parent_scope_);
    return *this;
}

void Scope::SetSymbol(const std::string& symbol, std::shared_ptr<Object> ptr) {
    if (scope_.contains(symbol)) {
        scope_[symbol] = ptr;
        return;
    }
    if (parent_scope_.get()) {
        parent_scope_->SetSymbol(symbol, ptr);
        return;
    }
    throw NameError("No variable with such name in any scope");
}

bool Scope::HasSymbol(const std::string& symbol) {
    if (scope_.contains(symbol)) {
        return true;
    }
    if (parent_scope_.get()) {
        return parent_scope_->HasSymbol(symbol);
    }
    return false;
}

std::shared_ptr<Scope> Scope::MakeCopy() {
    return std::make_shared<Scope>(*this);
}

std::shared_ptr<Object> If::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<If>();
}

std::string If::Serialize() {
    return "if";
}

std::shared_ptr<Object> If::Apply(const ObjectList& args) {
    throw RuntimeError("Special form 'If' should never be applied");
}

std::shared_ptr<Object> Define::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Define>();
}

std::string Define::Serialize() {
    return "define";
}

std::shared_ptr<Object> Define::Apply(const ObjectList& args) {
    throw RuntimeError("Special form 'Define' should never be applied");
}

std::shared_ptr<Object> Set::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<Set>();
}

std::string Set::Serialize() {
    return "set!";
}

std::shared_ptr<Object> Set::Apply(const ObjectList& args) {
    throw RuntimeError("Special form 'Define' should never be applied");
}

std::shared_ptr<Object> SetCdr::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<SetCdr>();
}

std::string SetCdr::Serialize() {
    return "set-cdr!";
}

std::shared_ptr<Object> SetCdr::Apply(const ObjectList& args) {
    throw RuntimeError("Special form 'set-cdr!' should never be applied");
}

std::shared_ptr<Object> SetCar::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<SetCar>();
}

std::string SetCar::Serialize() {
    return "set-car!";
}

std::shared_ptr<Object> SetCar::Apply(const ObjectList& args) {
    throw RuntimeError("Special form 'set-car!' should never be applied");
}

std::shared_ptr<Object> IsSymbol::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<IsSymbol>();
}

std::string IsSymbol::Serialize() {
    return "symbol?";
}

std::shared_ptr<Object> IsSymbol::Apply(const ObjectList& args) {
    if (args.size() != 1) {
        throw RuntimeError("Wrong number of argument in function 'number?'");
    }
    return Bool(Is<Symbol>(args.front())).Eval(nullptr);
}

std::shared_ptr<Object> LambdaCall::Apply(const ObjectList& args) {
    if (args.size() != args_.size()) {
        throw RuntimeError("Wrong number of arguments in 'Lambda' function");
    }
    my_scope_ = my_scope_->MakeCopy();
    for (size_t i = 0; i < args.size(); ++i) {
        my_scope_->SetSymbol(args_[i]->GetName(), args[i]);
    }
    for (size_t i = 0; i < body_instructions_.size() - 1; ++i) {
        ParseTree(body_instructions_[i], my_scope_, false);
    }
    return ParseTree(body_instructions_.back(), my_scope_, false);
}

std::string LambdaCall::Serialize() {
    throw SyntaxError("LambdaCall should not be serialized");
}

std::shared_ptr<Object> LambdaCall::Eval(std::shared_ptr<Scope> scope) {
    return std::make_shared<LambdaCall>(my_scope_, body_instructions_, args_);
}

LambdaCall& LambdaCall::operator=(LambdaCall&& other) noexcept {
    my_scope_ = move(other.my_scope_);
    body_instructions_ = move(other.body_instructions_);
    args_ = move(other.args_);
    return *this;
}

LambdaCall& LambdaCall::operator=(const LambdaCall& other) {
    my_scope_ = other.my_scope_;
    body_instructions_ = other.body_instructions_;
    args_ = other.args_;
    return *this;
}

LambdaCall::LambdaCall(LambdaCall&& other) noexcept
    : my_scope_(move(other.my_scope_)),
      body_instructions_(move(other.body_instructions_)),
      args_(move(other.args_)) {
}

LambdaCall::LambdaCall(const LambdaCall& other)
    : my_scope_(other.my_scope_), body_instructions_(other.body_instructions_), args_(other.args_) {
}

LambdaCall::LambdaCall(std::shared_ptr<Scope> scope,
                       const std::vector<std::shared_ptr<Object>> body,
                       const std::vector<std::shared_ptr<Symbol>>& args)
    : my_scope_(scope), body_instructions_(body), args_(args) {
}

std::string Lambda::Serialize() {
    throw SyntaxError("Lambda should not be serialized");
}

std::shared_ptr<Object> Lambda::Eval(std::shared_ptr<Scope> scope) {
    throw SyntaxError("Lambda should not be copied");
}

#pragma clang diagnostic pop