#pragma once

#include <memory>
#include <string>
#include "tokenizer.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>

class Object;

using ObjectList = std::vector<std::shared_ptr<Object>>;
using StringFuncMap = std::unordered_map<std::string, std::shared_ptr<Object>>;

// Runtime type checking and conversion.
template <class T>
std::shared_ptr<T> As(const std::shared_ptr<Object>& obj) {
    return std::dynamic_pointer_cast<T>(obj);
}

template <class T>
bool Is(const std::shared_ptr<Object>& obj) {
    return As<T>(obj).get();
}

class Scope {
public:
    Scope() = default;

    Scope(StringFuncMap&& inp, std::shared_ptr<Scope> par_scope = nullptr) noexcept;

    Scope(const Scope& other);

    Scope(Scope&& other) noexcept;

    Scope& operator=(const Scope& other);

    Scope& operator=(Scope&& other) noexcept;

    ~Scope() = default;

    std::shared_ptr<Object> ResolveSymbol(const std::string& symbol);

    bool DefineSymbol(const std::string& symbol, std::shared_ptr<Object> ptr);

    void SetSymbol(const std::string& symbol, std::shared_ptr<Object> ptr);

    bool HasSymbol(const std::string& symbol);

    std::shared_ptr<Scope> MakeCopy();

private:
    std::shared_ptr<Scope> parent_scope_ = nullptr;
    StringFuncMap scope_;
};

///////////////////////////////////////////////////////////////////////////////

void ConvertArgsToVector(std::shared_ptr<Object> ast, ObjectList& obj_list,
                         std::shared_ptr<Scope> scope, bool and_or_eval_optimization);

std::shared_ptr<Object> ParseTree(std::shared_ptr<Object> ast, std::shared_ptr<Scope> scope,
                                  bool and_or_eval_optimization);

///////////////////////////////////////////////////////////////////////////////

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) = 0;

    virtual std::string Serialize() = 0;

    virtual ~Object() = default;
};

class Dot : public Object {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;
};

class Number : public Object {
public:
    Number(int64_t value);

    Number(const Token& token);

    Number(const Number& other);

    Number(Number&& other) noexcept;

    Number& operator=(const Number& other);

    Number& operator=(Number&& other) noexcept;

    ~Number() override = default;

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    int64_t GetValue() const;

private:
    int64_t value_;
};

class Bool : public Object {
public:
    Bool(bool inp);

    Bool(const Token& token);

    Bool(const Bool& other);

    Bool(Bool&& other) noexcept;

    Bool& operator=(const Bool& other);

    Bool& operator=(Bool&& other) noexcept;

    ~Bool() override = default;

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    bool GetBool() const;

private:
    bool value_;
};

class Symbol : public Object {
public:
    Symbol(const std::string& str);

    Symbol(const Token& token);

    Symbol(const Symbol& other);

    Symbol(Symbol&& other) noexcept;

    Symbol& operator=(const Symbol& other);

    Symbol& operator=(Symbol&& other) noexcept;

    ~Symbol() override = default;

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    const std::string& GetName() const;

private:
    std::string str_;
};

class Function : public Object {
public:
    virtual std::shared_ptr<Object> Apply(const ObjectList& args) = 0;
};

class LambdaCall : public Function {
public:
    LambdaCall(std::shared_ptr<Scope> scope, const std::vector<std::shared_ptr<Object>> body,
               const std::vector<std::shared_ptr<Symbol>>& args);

    LambdaCall(const LambdaCall& other);

    LambdaCall(LambdaCall&& other) noexcept;

    LambdaCall& operator=(const LambdaCall& other);

    LambdaCall& operator=(LambdaCall&& other) noexcept;

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;

private:
    std::shared_ptr<Scope> my_scope_;
    std::vector<std::shared_ptr<Object>> body_instructions_;
    std::vector<std::shared_ptr<Symbol>> args_;
};

class Lambda : public Object {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;
};

class If : public Function {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Define : public Function {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Set : public Function {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class SetCdr : public Function {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class SetCar : public Function {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Quote : public Function {
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class IsNum : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class IsSymbol : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class IsBool : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Abs : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Sum : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Dif : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Prod : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Div : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Min : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Max : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Comp : public Function {
public:
    Comp(std::string inp);

    Comp(const Comp& other);

    Comp(Comp&& other) noexcept;

    Comp& operator=(const Comp& other);

    Comp& operator=(Comp&& other) noexcept;

    ~Comp() override = default;

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;

private:
    bool (*comp_)(int64_t, int64_t);
    std::string type_;
    inline static std::unordered_map<std::string, bool (*)(int64_t, int64_t)> get_lambda = {
        {"=", +[](int64_t x, int64_t y) { return x == y; }},
        {">", +[](int64_t x, int64_t y) { return x > y; }},
        {"<", +[](int64_t x, int64_t y) { return x < y; }},
        {">=", +[](int64_t x, int64_t y) { return x >= y; }},
        {"<=", +[](int64_t x, int64_t y) { return x <= y; }}};
};

class Not : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class And : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Or : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Cell : public Object {
public:
    void SetExterior(bool is_exterior);

    void SetFirst(const std::shared_ptr<Object>& first);

    void SetSecond(const std::shared_ptr<Object>& second);

    std::shared_ptr<Object> GetFirst() const;

    std::shared_ptr<Object> GetSecond() const;

    bool IsExterior();

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

private:
    bool is_exterior_ = false;
    std::shared_ptr<Object> first_;
    std::shared_ptr<Object> second_;
};

class IsPair : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class IsNull : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class IsList : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;

private:
    bool RecursiveCheck(std::shared_ptr<Object> ast);
};

class Cons : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Car : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class Cdr : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;
};

class List : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;

private:
    void RecursiveListContraction(const ObjectList& args, size_t ind,
                                  std::shared_ptr<Cell>& last_cell);
};

class ListRef : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;

private:
    std::shared_ptr<Object> RecursiveListSearch(std::shared_ptr<Object> list, const size_t ind,
                                                size_t cur_step = 0);
};

class ListTail : public Function {
public:
    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope) override;

    std::string Serialize() override;

    std::shared_ptr<Object> Apply(const ObjectList& args) override;

private:
    std::shared_ptr<Object> RecursiveListSearch(std::shared_ptr<Object> list, const size_t ind,
                                                size_t cur_step = 0);
};
