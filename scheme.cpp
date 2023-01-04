#include "object.h"
#include "parser.h"
#include "scheme.h"
#include <sstream>
#include "tokenizer.h"

std::shared_ptr<Object> ParseTree(std::shared_ptr<Object> ast, std::shared_ptr<Scope> scope,
                                  bool and_or_eval_optimization = false) {
    if (!Is<Cell>(ast)) {
        if (ast.get()) {
            return ast->Eval(scope);
        }
        throw RuntimeError("Invalid expression");
    }

    if (!As<Cell>(ast)->GetFirst().get()) {
        throw RuntimeError("Expression should contain operator");
    }

    // and/or eval optimization
    if (Is<Symbol>(As<Cell>(ast)->GetFirst()) &&
        !scope->HasSymbol(As<Symbol>(As<Cell>(ast)->GetFirst())->GetName()) &&
        and_or_eval_optimization) {
        return ast;
    }

    std::shared_ptr<Object> function;
    // if lambda -- lambda should be constructed before calling
    if (Is<Cell>(As<Cell>(ast)->GetFirst()) && As<Cell>(As<Cell>(ast)->GetFirst()).get()) {
        function = ParseTree(As<Cell>(ast)->GetFirst(), scope, and_or_eval_optimization);
    } else {
        function = As<Cell>(ast)->GetFirst()->Eval(scope);
    }

    if (!Is<Function>(function)) {
        // lambda case
        if (Is<Lambda>(function)) {
            if (!As<Cell>(ast)->GetSecond().get()) {
                throw SyntaxError("Lambda should have args");
            }
            auto tail = As<Cell>(ast)->GetSecond();
            auto list_arg = As<Cell>(tail)->GetFirst();
            // handling args
            std::vector<std::shared_ptr<Symbol>> cur_scope_variables;
            while (list_arg.get()) {
                if (!As<Cell>(list_arg).get()) {
                    throw SyntaxError("Wrong lambda args syntax");
                }
                auto current = As<Cell>(list_arg)->GetFirst();
                if (Is<Symbol>(current)) {
                    cur_scope_variables.push_back(As<Symbol>(current));
                    list_arg = As<Cell>(list_arg)->GetSecond();
                } else {
                    throw NameError("Lambda args should be Symbol type");
                }
            }
            // creating scope variables
            StringFuncMap cur_scope_alias;
            for (auto ptr : cur_scope_variables) {
                cur_scope_alias[ptr->GetName()];
            }
            // creating current scope
            auto cur_scope = std::make_shared<Scope>(move(cur_scope_alias), scope);

            // working with function body
            auto func_body = As<Cell>(tail)->GetSecond();
            std::vector<std::shared_ptr<Object>> instructions;

            if (!func_body.get()) {
                throw SyntaxError("Lambda should have body");
            }

            while (func_body.get()) {
                auto current = As<Cell>(func_body)->GetFirst();
                instructions.push_back(current);
                func_body = As<Cell>(func_body)->GetSecond();
            }
            return std::make_shared<LambdaCall>(cur_scope, instructions, cur_scope_variables);
        }
        if (and_or_eval_optimization) {
            return ast;
        }
        throw RuntimeError("Expression should contain operator or lambda");
    }

    // quote case
    if (Is<Quote>(function)) {
        if (As<Cell>(ast)->GetSecond().get()) {
            return As<Cell>(ast)->GetSecond();
        }
        // empty list
        return std::make_shared<Cell>();
    }

    // if case
    if (Is<If>(function)) {
        if (!As<Cell>(ast)->GetSecond().get()) {
            throw SyntaxError("Incorrect 'if' syntax");
        }
        // parse condition
        auto tail = As<Cell>(ast)->GetSecond();
        if (!Is<Cell>(tail)) {
            throw SyntaxError("Incorrect 'if' syntax");
        }
        auto expression = As<Cell>(tail)->GetFirst();

        // calc condition
        if (!expression.get()) {
            throw SyntaxError("Incorrect 'if' syntax");
        }
        auto calc_result = ParseTree(expression, scope, and_or_eval_optimization);

        // interpret result
        bool condition_state = true;
        if (Is<Bool>(calc_result)) {
            condition_state = As<Bool>(calc_result)->GetBool();
        }

        // has true-branch
        expression = As<Cell>(tail)->GetSecond();
        if (!Is<Cell>(expression)) {
            throw SyntaxError("Incorrect 'if' syntax");
        }
        if (!As<Cell>(expression).get()) {
            return nullptr;
        }

        if (condition_state) {  // true branch
            calc_result =
                ParseTree(As<Cell>(expression)->GetFirst(), scope, and_or_eval_optimization);

            // check is 'if' syntax correct
            expression = As<Cell>(expression)->GetSecond();
            if (!Is<Cell>(expression) || !As<Cell>(expression).get()) {
                return calc_result;
            }
            if (As<Cell>(expression)->GetSecond().get()) {
                throw SyntaxError("Incorrect 'if' syntax");
            }
            return calc_result;
        }

        // false branch
        expression = As<Cell>(expression)->GetSecond();
        if (!Is<Cell>(expression) || !As<Cell>(expression).get()) {
            return std::make_shared<Cell>();
        }
        if (As<Cell>(expression)->GetSecond().get()) {
            throw SyntaxError("Incorrect 'if' syntax");
        }
        if (!As<Cell>(expression)->GetFirst().get()) {
            return std::make_shared<Cell>();
        }
        return ParseTree(As<Cell>(expression)->GetFirst(), scope, and_or_eval_optimization);
    }

    //     define/set/set-car/set-cdr case
    if (Is<Define>(function) || Is<Set>(function) || Is<SetCar>(function) || Is<SetCdr>(function)) {
        if (!As<Cell>(ast)->GetSecond().get()) {
            throw SyntaxError("Incorrect 'define/set' syntax");
        }
        auto tail = As<Cell>(ast)->GetSecond();

        if (!Is<Cell>(tail) || !As<Cell>(tail).get()) {
            throw SyntaxError("Incorrect 'define/set' syntax");
        }

        // handling lambda syntax sugar
        if (Is<Define>(function) && Is<Cell>(As<Cell>(tail)->GetFirst())) {
            // rebuilding ast to make it match usual lambda syntax
            auto left_cell = As<Cell>(As<Cell>(tail)->GetFirst());
            As<Cell>(tail)->SetFirst(left_cell->GetFirst());
            left_cell->SetFirst(std::make_shared<Symbol>("lambda"));

            auto body = As<Cell>(As<Cell>(tail)->GetSecond());
            auto right_cell = std::make_shared<Cell>();
            right_cell->SetFirst(left_cell);
            As<Cell>(tail)->SetSecond(right_cell);

            auto cur_node = As<Cell>(As<Cell>(As<Cell>(tail)->GetSecond())->GetFirst());
            auto arg_list = cur_node->GetSecond();

            auto new_right_cell = std::make_shared<Cell>();
            new_right_cell->SetFirst(arg_list);
            new_right_cell->SetSecond(body);

            As<Cell>(As<Cell>(As<Cell>(tail)->GetSecond())->GetFirst())->SetSecond(new_right_cell);
        }
        auto first_arg = As<Cell>(tail)->GetFirst();
        if (!Is<Symbol>(first_arg) || !As<Symbol>(first_arg).get()) {
            first_arg = ParseTree(first_arg, scope, and_or_eval_optimization);
        }

        auto second_arg = As<Cell>(tail)->GetSecond();
        if (!Is<Cell>(second_arg) || !As<Cell>(second_arg).get()) {
            throw SyntaxError("Incorrect 'define/set' syntax");
        }
        // check for case : define x y z
        if (As<Cell>(second_arg)->GetSecond().get()) {
            throw SyntaxError("Incorrect 'define/set' syntax");
        }
        second_arg = ParseTree(As<Cell>(second_arg)->GetFirst(), scope, and_or_eval_optimization);

        // define case
        if (Is<Define>(function)) {
            std::string first_arg_name;
            if (Is<Symbol>(first_arg)) {
                first_arg_name = As<Symbol>(first_arg)->GetName();
            } else {

                throw NameError("Name of variable should be a string");
            }
            // why we need eval here?
            if (scope->DefineSymbol(first_arg_name, second_arg->Eval(scope))) {
                return std::make_shared<Cell>();
            }
            throw RuntimeError("Variable can't be defined");
        }

        if (Is<Symbol>(first_arg) && As<Symbol>(first_arg).get()) {
            first_arg = scope->ResolveSymbol(As<Symbol>(first_arg)->GetName());
        }
        // set case
        if (Is<Set>(function)) {
            if (Is<Number>(first_arg) && Is<Number>(second_arg)) {
                *As<Number>(first_arg).get() = *As<Number>(second_arg).get();
            } else if (Is<Cell>(first_arg) && Is<Cell>(second_arg)) {
                *As<Number>(first_arg).get() = *As<Number>(second_arg).get();
            } else {
                throw NameError("Wrong operand types in 'set!'");
            }
        } else if (Is<SetCar>(function)) {  // set-car case
            As<Cell>(first_arg)->SetFirst(second_arg);
        } else if (Is<SetCdr>(function)) {  // set-cdr case
            As<Cell>(first_arg)->SetSecond(second_arg);
        }
        return std::make_shared<Cell>();
    }

    if (Is<And>(function) || Is<Or>(function)) {
        and_or_eval_optimization = true;
    }

    ObjectList object_list;
    ConvertArgsToVector(As<Cell>(ast)->GetSecond(), object_list, scope, and_or_eval_optimization);
    return As<Function>(function)->Apply(object_list);
}

void ConvertArgsToVector(std::shared_ptr<Object> ast, ObjectList& obj_list,
                         std::shared_ptr<Scope> scope, bool and_or_eval_optimization) {
    if (!ast.get()) {
        return;
    }

    if (!Is<Cell>(ast)) {
        obj_list.push_back(ast);
        return;
    }

    std::shared_ptr<Object> first_obj = As<Cell>(ast)->GetFirst();
    if (!first_obj.get()) {
        obj_list.push_back(first_obj);
        return;
    }

    if (Is<Cell>(first_obj)) {
        obj_list.push_back(ParseTree(first_obj, scope, and_or_eval_optimization));
    } else {
        obj_list.push_back(first_obj->Eval(scope));
    }

    std::shared_ptr<Object> second_obj = As<Cell>(ast)->GetSecond();
    if (second_obj.get()) {
        ConvertArgsToVector(second_obj, obj_list, scope, and_or_eval_optimization);
    }
}

std::string Interpreter::Run(const std::string& input) {
    std::stringstream inp_stream(input);
    Tokenizer tokenizer(&inp_stream);

    auto input_ast = Read(&tokenizer);

    auto output_ast = ParseTree(input_ast, global_);

    return output_ast->Serialize();
}

Interpreter::Interpreter() {
    StringFuncMap alias{
        {"+", std::make_shared<Sum>()},
        {"-", std::make_shared<Dif>()},
        {"/", std::make_shared<Div>()},
        {"or", std::make_shared<Or>()},
        {"if", std::make_shared<If>()},
        {"*", std::make_shared<Prod>()},
        {"=", std::make_shared<Comp>("=")},
        {">", std::make_shared<Comp>(">")},
        {"<", std::make_shared<Comp>("<")},
        {">=", std::make_shared<Comp>(">=")},
        {"<=", std::make_shared<Comp>("<=")},
        {"min", std::make_shared<Min>()},
        {"max", std::make_shared<Max>()},
        {"not", std::make_shared<Not>()},
        {"and", std::make_shared<And>()},
        {"abs", std::make_shared<Abs>()},
        {"car", std::make_shared<Car>()},
        {"cdr", std::make_shared<Cdr>()},
        {"set!", std::make_shared<Set>()},
        {"cons", std::make_shared<Cons>()},
        {"list", std::make_shared<List>()},
        {"quote", std::make_shared<Quote>()},
        {"pair?", std::make_shared<IsPair>()},
        {"null?", std::make_shared<IsNull>()},
        {"list?", std::make_shared<IsList>()},
        {"define", std::make_shared<Define>()},
        {"lambda", std::make_shared<Lambda>()},
        {"number?", std::make_shared<IsNum>()},
        {"symbol?", std::make_shared<IsSymbol>()},
        {"set-cdr!", std::make_shared<SetCdr>()},
        {"set-car!", std::make_shared<SetCar>()},
        {"boolean?", std::make_shared<IsBool>()},
        {"list-ref", std::make_shared<ListRef>()},
        {"list-tail", std::make_shared<ListTail>()},
    };
    global_ = std::make_shared<Scope>(std::move(alias));
}
