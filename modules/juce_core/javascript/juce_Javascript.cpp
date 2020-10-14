/*
  ==============================================================================

   This file is part of the JUCE library.
   Copyright (c) 2020 - Raw Material Software Limited

   JUCE is an open source library subject to commercial or open-source
   licensing.

   The code included in this file is provided under the terms of the ISC license
   http://www.isc.org/downloads/software-support-policy/isc-license. Permission
   To use, copy, modify, and/or distribute this software for any purpose with or
   without fee is hereby granted provided that the above copyright notice and
   this permission notice appear in all copies.

   JUCE IS PROVIDED "AS IS" WITHOUT ANY WARRANTY, AND ALL WARRANTIES, WHETHER
   EXPRESSED OR IMPLIED, INCLUDING MERCHANTABILITY AND FITNESS FOR PURPOSE, ARE
   DISCLAIMED.

  ==============================================================================
*/


namespace juce
{

#define JUCE_JS_OPERATORS(X) \
    X(semicolon,            ";")   X(dot,               ".")   X(comma,        ",") \
    X(openParen,            "(")   X(closeParen,        ")")   X(openBrace,    "{")    X(closeBrace, "}") \
    X(openBracket,          "[")   X(closeBracket,      "]")   X(colon,        ":")    X(question,   "?") \
    X(typeEquals,           "===") X(equals,            "==")  X(assign,       "=") \
    X(typeNotEquals,        "!==") X(notEquals,         "!=")  X(logicalNot,   "!") \
    X(plusEquals,           "+=")  X(plusplus,          "++")  X(plus,         "+") \
    X(minusEquals,          "-=")  X(minusminus,        "--")  X(minus,        "-") \
    X(timesEquals,          "*=")  X(times,             "*")   X(divideEquals, "/=")   X(divide,     "/") \
    X(moduloEquals,         "%=")  X(modulo,            "%")   X(xorEquals,    "^=")   X(bitwiseXor, "^") \
    X(andEquals,            "&=")  X(logicalAnd,        "&&")  X(bitwiseAnd,   "&") \
    X(orEquals,             "|=")  X(logicalOr,         "||")  X(bitwiseOr,    "|") \
    X(leftShiftEquals,      "<<=") X(lessThanOrEqual,   "<=")  X(leftShift,    "<<")   X(lessThan,   "<") \
    X(rightShiftUnsigned,   ">>>") X(rightShiftEquals,  ">>=") X(rightShift,   ">>")   X(greaterThanOrEqual, ">=")  X(greaterThan,  ">")

#define JUCE_JS_KEYWORDS(X) \
    X(var,          "var")          X(if_,          "if")           X(else_,        "else")         X(do_,          "do")           X(null_,        "null") \
    X(while_,       "while")        X(for_,         "for")          X(break_,       "break")        X(continue_,    "continue")     X(undefined,    "undefined") \
    X(function,     "function")     X(return_,      "return")       X(true_,        "true")         X(false_,       "false")        X(new_,         "new") \
    X(typeof_,      "typeof")       X(instanceof_,  "instanceof")   X(infinity_,    "Infinity")     X(try_,         "try")          X(catch_,       "catch") \
    X(throw_,       "throw")        X(const_,       "const")        X(let_,         "let")          X(import_,      "import")       X(await_,       "await") \
    X(debugger_,    "debugger")     X(switch_,      "switch")       X(default_,     "default")      X(with_,        "with")         X(yield_,       "yield") \
    X(in_,          "in")           X(extends_,     "extends")      X(export_,      "export")       X(finally_,     "finally")      X(super_,       "super") \
    X(this_,        "this")         X(enum_,        "enum")         X(implements_,  "implements")   X(interface_,   "interface")    X(package_,     "package") \
    X(private_,     "private")      X(protected_,   "protected")    X(public_,      "public")       X(static_,      "static")       X(volatile_,    "volatile") \
    X(abstract_,    "abstract")     X(float_,       "float")        X(synchronized_,"synchronized") X(boolean_,     "boolean")      X(goto_,        "goto") \
    X(int_,         "int")          X(long_,        "long")         X(native_,      "native")       X(short_,       "short")        X(transient_,   "transient")

namespace TokenTypes
{
    #define JUCE_DECLARE_JS_TOKEN(name, str)  static const char* const name = str;
    JUCE_JS_KEYWORDS  (JUCE_DECLARE_JS_TOKEN)
    JUCE_JS_OPERATORS (JUCE_DECLARE_JS_TOKEN)
    JUCE_DECLARE_JS_TOKEN (eof,        "$eof")
    JUCE_DECLARE_JS_TOKEN (literal,    "$literal")
    JUCE_DECLARE_JS_TOKEN (identifier, "$identifier")
}

JUCE_BEGIN_IGNORE_WARNINGS_MSVC (4702)

//==============================================================================
struct JavascriptEngine::RootObject final  : public DynamicObject
{
    RootObject()
    {
        setMethod ("exec",                  exec);
        setMethod ("eval",                  eval);
        setMethod ("trace",                 trace);
        setMethod ("typeof",                typeof_internal);
        setMethod ("instanceof",            instanceof_internal);
        setMethod ("charToInt",             charToInt);                 //NB: Non-standard
        setMethod ("parseInt",              NumberClass::parseInt);
        setMethod ("parseFloat",            NumberClass::parseFloat);
        setMethod ("isNaN",                 NumberClass::isNaN);
        setMethod ("isFinite",              NumberClass::isFinite);
        setMethod ("encodeURI",             encodeURI);
        setMethod ("encodeURIComponent",    encodeURIComponent);
        setMethod ("decodeURI",             decodeURI);
        setMethod ("decodeURIComponent",    decodeURIComponent);

        setProperty ("Infinity",            std::numeric_limits<double>::infinity());
        setProperty ("NaN",                 std::numeric_limits<double>::quiet_NaN());
        setProperty ("undefined",           var::undefined());
        setProperty ("globalThis",          var::undefined()); //TODO: https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects

        registerNativeObject<ArrayBufferClass>();
        registerNativeObject<ArrayClass>();
        registerNativeObject<BooleanClass>();
        registerNativeObject<ConsoleClass>();                           //NB: Non-standard.
        registerNativeObject<DataViewClass>();
        registerNativeObject<DateClass>();
        registerNativeObject<JSONClass>();
        registerNativeObject<MapClass>();
        registerNativeObject<MathClass>();
        registerNativeObject<NumberClass>();
        registerNativeObject<ObjectClass>();
        registerNativeObject<RegExpClass>();
        registerNativeObject<SetClass>();
        registerNativeObject<StringClass>();
        registerNativeObject<SymbolClass>();
        registerNativeObject<WeakMapClass>();
        registerNativeObject<WeakSetClass>();
    }

    //==============================================================================
    #define JUCE_JS_CREATE_METHOD(methodName) \
        setMethod (JUCE_STRINGIFY (methodName), methodName);

    #define JUCE_JS_IDENTIFY_CLASS(className) \
        static Identifier getClassName() { static const Identifier i (className); return i; }

    template<typename RootClass>
    void registerNativeObject()
    {
        setProperty (RootClass::getClassName(), new RootClass());
    }

    Time timeout;

    using Args = const var::NativeFunctionArgs&;
    using TokenType = const char*;

    void execute (const String& code)
    {
        ExpressionTreeBuilder tb (code);
        std::unique_ptr<BlockStatement> (tb.parseStatementList())->perform (Scope ({}, *this, *this), nullptr);
    }

    var evaluate (const String& code)
    {
        ExpressionTreeBuilder tb (code);
        return ExpPtr (tb.parseExpression())->getResult (Scope ({}, *this, *this));
    }

    //==============================================================================
    static bool areTypeEqual (const var& a, const var& b)
    {
        return a.hasSameTypeAs (b) && isFunction (a) == isFunction (b)
                && (((a.isUndefined() || a.isVoid()) && (b.isUndefined() || b.isVoid())) || a == b);
    }

    static String getTokenName (TokenType t)                  { return t[0] == '$' ? String (t + 1) : ("'" + String (t) + "'"); }
    static bool isFunction (const var& v) noexcept            { return dynamic_cast<FunctionObject*> (v.getObject()) != nullptr; }
    static bool isNumeric (const var& v) noexcept             { return v.isInt() || v.isDouble() || v.isInt64() || v.isBool(); }
    static bool isNumericOrUndefined (const var& v) noexcept  { return isNumeric (v) || v.isUndefined(); }
    static int64 getOctalValue (const String& s)              { BigInteger b; b.parseString (s.initialSectionContainingOnly ("01234567"), 8); return b.toInt64(); }
    static Identifier getPrototypeIdentifier()                { static const Identifier i ("prototype"); return i; }
    static var* getPropertyPointer (DynamicObject& o, const Identifier& i) noexcept   { return o.getProperties().getVarPointer (i); }

    //==============================================================================
    struct CodeLocation final
    {
        CodeLocation (const String& code) noexcept        : program (code), location (program.getCharPointer()) {}
        CodeLocation (const CodeLocation& other) noexcept : program (other.program), location (other.location) {}

        void throwError (const String& message) const
        {
            int col = 1, line = 1;

            for (auto i = program.getCharPointer(); i < location && ! i.isEmpty(); ++i)
            {
                ++col;
                if (*i == '\n')
                {
                    col = 1;
                    ++line;
                }
            }

            throw "Line " + String (line) + ", column " + String (col) + " : " + message;
        }

        String program;
        String::CharPointerType location;
    };

    //==============================================================================
    struct Scope final
    {
        Scope (const Scope* p, ReferenceCountedObjectPtr<RootObject> rt, DynamicObject::Ptr scp) noexcept
            : parent (p), root (std::move (rt)),
              scope (std::move (scp)) {}

        const Scope* const parent;
        ReferenceCountedObjectPtr<RootObject> root;
        DynamicObject::Ptr scope;

        var findFunctionCall (const CodeLocation& location, const var& targetObject, const Identifier& functionName) const
        {
            if (auto* o = targetObject.getDynamicObject())
            {
                if (auto* prop = getPropertyPointer (*o, functionName))
                    return *prop;

                for (auto* p = o->getProperty (getPrototypeIdentifier()).getDynamicObject(); p != nullptr;
                     p = p->getProperty (getPrototypeIdentifier()).getDynamicObject())
                {
                    if (auto* prop = getPropertyPointer (*p, functionName))
                        return *prop;
                }

                // if there's a class with an overridden DynamicObject::hasMethod, this avoids an error
                if (o->hasMethod (functionName))
                    return {};
            }

            if (targetObject.isString())
                if (auto* m = findRootClassProperty (StringClass::getClassName(), functionName))
                    return *m;

            if (targetObject.isArray())
                if (auto* m = findRootClassProperty (ArrayClass::getClassName(), functionName))
                    return *m;

            if (targetObject.isInt() || targetObject.isInt64() || targetObject.isDouble())
                if (auto* m = findRootClassProperty (NumberClass::getClassName(), functionName))
                    return *m;

            if (auto* m = findRootClassProperty (ObjectClass::getClassName(), functionName))
                return *m;

            location.throwError ("Unknown function '" + functionName.toString() + "'");
            return {};
        }

        var* findRootClassProperty (const Identifier& className, const Identifier& propName) const
        {
            if (auto* cls = root->getProperty (className).getDynamicObject())
                return getPropertyPointer (*cls, propName);

            return nullptr;
        }

        var findSymbolInParentScopes (const Identifier& name) const
        {
            if (auto v = getPropertyPointer (*scope, name))
                return *v;

            return parent != nullptr ? parent->findSymbolInParentScopes (name)
                                     : var::undefined();
        }

        bool findAndInvokeMethod (const Identifier& function, const var::NativeFunctionArgs& args, var& result) const
        {
            auto* target = args.thisObject.getDynamicObject();

            if (target == nullptr || target == scope.get())
            {
                if (auto* m = getPropertyPointer (*scope, function))
                {
                    if (auto fo = dynamic_cast<FunctionObject*> (m->getObject()))
                    {
                        result = fo->invoke (*this, args);
                        return true;
                    }
                }
            }

            const auto& props = scope->getProperties();

            for (int i = 0; i < props.size(); ++i)
                if (auto* o = props.getValueAt (i).getDynamicObject())
                    if (Scope (this, *root, *o).findAndInvokeMethod (function, args, result))
                        return true;

            return false;
        }

        bool invokeMethod (const var& m, const var::NativeFunctionArgs& args, var& result) const
        {
            if (isFunction (m))
            {
                auto* target = args.thisObject.getDynamicObject();

                if (target == nullptr || target == scope.get())
                {
                    if (auto fo = dynamic_cast<FunctionObject*> (m.getObject()))
                    {
                        result = fo->invoke (*this, args);
                        return true;
                    }
                }
            }

            return false;
        }

        void checkTimeOut (const CodeLocation& location) const
        {
            if (Time::getCurrentTime() > root->timeout)
                location.throwError (root->timeout == Time() ? "Interrupted" : "Execution timed-out");
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Scope)
    };

    //==============================================================================
    struct Statement
    {
        Statement (const CodeLocation& l) noexcept : location (l) {}
        virtual ~Statement() {}

        enum class ResultCode { ok = 0, returnWasHit, breakWasHit, continueWasHit };
        virtual ResultCode perform (const Scope&, var*) const  { return ResultCode::ok; }

        CodeLocation location;
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (Statement)
    };

    struct Expression  : public Statement
    {
        Expression (const CodeLocation& l) noexcept : Statement (l) {}

        virtual var getResult (const Scope&) const            { return var::undefined(); }
        virtual void assign (const Scope&, const var&) const  { location.throwError ("Cannot assign to this expression!"); }

        ResultCode perform (const Scope& s, var*) const override  { getResult (s); return ResultCode::ok; }
    };

    using ExpPtr = std::unique_ptr<Expression>;

    struct BlockStatement final  : public Statement
    {
        BlockStatement (const CodeLocation& l) noexcept : Statement (l) {}

        ResultCode perform (const Scope& s, var* returnedValue) const override
        {
            for (auto* statement : statements)
            {
                const auto r = statement->perform (s, returnedValue);
                if (r != ResultCode::ok)
                    return r;
            }

            return ResultCode::ok;
        }

        OwnedArray<Statement> statements;
    };

    struct IfStatement final  : public Statement
    {
        IfStatement (const CodeLocation& l) noexcept : Statement (l) {}

        ResultCode perform (const Scope& s, var* returnedValue) const override
        {
            return (condition->getResult(s) ? trueBranch : falseBranch)->perform (s, returnedValue);
        }

        ExpPtr condition;
        std::unique_ptr<Statement> trueBranch, falseBranch;
    };

    struct VarStatement final  : public Statement
    {
        VarStatement (const CodeLocation& l) noexcept : Statement (l) {}

        ResultCode perform (const Scope& s, var*) const override
        {
            s.scope->setProperty (name, initialiser->getResult (s));
            return ResultCode::ok;
        }

        Identifier name;
        ExpPtr initialiser;
    };

    struct LoopStatement final  : public Statement
    {
        LoopStatement (const CodeLocation& l, bool isDo) noexcept : Statement (l), isDoLoop (isDo) {}

        ResultCode perform (const Scope& s, var* returnedValue) const override
        {
            initialiser->perform (s, nullptr);

            while (isDoLoop || condition->getResult (s))
            {
                s.checkTimeOut (location);
                auto r = body->perform (s, returnedValue);

                if (r == ResultCode::returnWasHit)   return r;
                if (r == ResultCode::breakWasHit)    break;

                iterator->perform (s, nullptr);

                if (isDoLoop && r != ResultCode::continueWasHit && ! condition->getResult (s))
                    break;
            }

            return ResultCode::ok;
        }

        std::unique_ptr<Statement> initialiser, iterator, body;
        ExpPtr condition;
        bool isDoLoop;
    };

    struct ReturnStatement final  : public Statement
    {
        ReturnStatement (const CodeLocation& l, ExpPtr v) noexcept : Statement (l), returnValue (std::move (v)) {}

        ResultCode perform (const Scope& s, var* ret) const override
        {
            if (ret != nullptr)
                *ret = returnValue->getResult (s);

            return ResultCode::returnWasHit;
        }

        ExpPtr returnValue;
    };

    struct BreakStatement final  : public Statement
    {
        BreakStatement (const CodeLocation& l) noexcept : Statement (l) {}
        ResultCode perform (const Scope&, var*) const override  { return ResultCode::breakWasHit; }
    };

    struct ContinueStatement final  : public Statement
    {
        ContinueStatement (const CodeLocation& l) noexcept : Statement (l) {}
        ResultCode perform (const Scope&, var*) const override  { return ResultCode::continueWasHit; }
    };

    struct LiteralValue final  : public Expression
    {
        LiteralValue (const CodeLocation& l, const var& v = {}) noexcept : Expression (l), value (v) {}
        var getResult (const Scope&) const override   { return value; }
        var value;
    };

    struct UnqualifiedName final  : public Expression
    {
        UnqualifiedName (const CodeLocation& l, const Identifier& n) noexcept : Expression (l), name (n) {}

        var getResult (const Scope& s) const override  { return s.findSymbolInParentScopes (name); }

        void assign (const Scope& s, const var& newValue) const override
        {
            if (auto* v = getPropertyPointer (*s.scope, name))
                *v = newValue;
            else
                s.root->setProperty (name, newValue);
        }

        Identifier name;
    };

    struct DotOperator final  : public Expression
    {
        DotOperator (const CodeLocation& l, ExpPtr& p, const Identifier& c) noexcept : Expression (l), parent (p.release()), child (c) {}

        var getResult (const Scope& s) const override
        {
            auto p = parent->getResult (s);
            static const Identifier lengthID ("length");

            if (child == lengthID)
            {
                if (auto* array = p.getArray())   return array->size();
                if (p.isString())                 return p.toString().length();
            }

            if (auto* o = p.getDynamicObject())
                if (auto* v = getPropertyPointer (*o, child))
                    return *v;

            return var::undefined();
        }

        void assign (const Scope& s, const var& newValue) const override
        {
            if (auto* o = parent->getResult (s).getDynamicObject())
                o->setProperty (child, newValue);
            else
                Expression::assign (s, newValue);
        }

        ExpPtr parent;
        Identifier child;
    };

    struct ArraySubscript final  : public Expression
    {
        ArraySubscript (const CodeLocation& l) noexcept : Expression (l) {}

        var getResult (const Scope& s) const override
        {
            auto arrayVar = object->getResult (s); // must stay alive for the scope of this method
            auto key = index->getResult (s);

            if (const auto* array = arrayVar.getArray())
                if (key.isInt() || key.isInt64() || key.isDouble())
                    return (*array) [static_cast<int> (key)];

            if (auto* o = arrayVar.getDynamicObject())
                if (key.isString())
                    if (auto* v = getPropertyPointer (*o, Identifier (key)))
                        return *v;

            return var::undefined();
        }

        void assign (const Scope& s, const var& newValue) const override
        {
            auto arrayVar = object->getResult (s); // must stay alive for the scope of this method
            auto key = index->getResult (s);

            if (auto* array = arrayVar.getArray())
            {
                if (key.isInt() || key.isInt64() || key.isDouble())
                {
                    const int i = key;
                    while (array->size() < i)
                        array->add (var::undefined());

                    array->set (i, newValue);
                    return;
                }
            }

            if (auto* o = arrayVar.getDynamicObject())
            {
                if (key.isString())
                {
                    o->setProperty (Identifier (key), newValue);
                    return;
                }
            }

            Expression::assign (s, newValue);
        }

        ExpPtr object, index;
    };

    struct BinaryOperatorBase  : public Expression
    {
        BinaryOperatorBase (const CodeLocation& l, ExpPtr& a, ExpPtr& b, TokenType op) noexcept
            : Expression (l), lhs (a.release()), rhs (b.release()), operation (op) {}

        ExpPtr lhs, rhs;
        TokenType operation;
    };

    struct BinaryOperator  : public BinaryOperatorBase
    {
        BinaryOperator (const CodeLocation& l, ExpPtr& a, ExpPtr& b, TokenType op) noexcept
            : BinaryOperatorBase (l, a, b, op) {}

        virtual var getWithUndefinedArg() const                           { return var::undefined(); }
        virtual var getWithDoubles (double, double) const                 { return throwError ("Double"); }
        virtual var getWithInts (int64, int64) const                      { return throwError ("Integer"); }
        virtual var getWithArrayOrObject (const var& a, const var&) const { return throwError (a.isArray() ? "Array" : "Object"); }
        virtual var getWithStrings (const String&, const String&) const   { return throwError ("String"); }

        var getResult (const Scope& s) const override
        {
            var a (lhs->getResult (s)), b (rhs->getResult (s));

            if ((a.isUndefined() || a.isVoid()) && (b.isUndefined() || b.isVoid()))
                return getWithUndefinedArg();

            if (isNumericOrUndefined (a) && isNumericOrUndefined (b))
                return (a.isDouble() || b.isDouble()) ? getWithDoubles (a, b) : getWithInts (a, b);

            if (a.isArray() || a.isObject())
                return getWithArrayOrObject (a, b);

            return getWithStrings (a.toString(), b.toString());
        }

        var throwError (const char* typeName) const
        {
            location.throwError (getTokenName (operation) + " is not allowed on the " + typeName + " type");
            return {};
        }
    };

    struct EqualsOp final  : public BinaryOperator
    {
        EqualsOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::equals) {}
        var getWithUndefinedArg() const override                               { return true; }
        var getWithDoubles (double a, double b) const override                 { return a == b; }
        var getWithInts (int64 a, int64 b) const override                      { return a == b; }
        var getWithStrings (const String& a, const String& b) const override   { return a == b; }
        var getWithArrayOrObject (const var& a, const var& b) const override   { return a == b; }
    };

    struct NotEqualsOp final  : public BinaryOperator
    {
        NotEqualsOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::notEquals) {}
        var getWithUndefinedArg() const override                               { return false; }
        var getWithDoubles (double a, double b) const override                 { return a != b; }
        var getWithInts (int64 a, int64 b) const override                      { return a != b; }
        var getWithStrings (const String& a, const String& b) const override   { return a != b; }
        var getWithArrayOrObject (const var& a, const var& b) const override   { return a != b; }
    };

    #undef JUCE_JAVASCRIPT_DEFINE_OP
    #define JUCE_JAVASCRIPT_DEFINE_OP(name, operationToPerform, tokenTypeToUse) \
        struct name final  : public BinaryOperator \
        { \
            name (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::tokenTypeToUse) {} \
            var getWithDoubles (double a, double b) const override                 { return a operationToPerform b; } \
            var getWithInts (int64 a, int64 b) const override                      { return a operationToPerform b; } \
            var getWithStrings (const String& a, const String& b) const override   { return a operationToPerform b; } \
        };

    JUCE_JAVASCRIPT_DEFINE_OP (LessThanOp, <, lessThan)
    JUCE_JAVASCRIPT_DEFINE_OP (LessThanOrEqualOp, <=, lessThanOrEqual)
    JUCE_JAVASCRIPT_DEFINE_OP (GreaterThanOp, >, greaterThan)
    JUCE_JAVASCRIPT_DEFINE_OP (GreaterThanOrEqualOp, >=, greaterThanOrEqual)
    JUCE_JAVASCRIPT_DEFINE_OP (AdditionOp, +, plus)

    #undef JUCE_JAVASCRIPT_DEFINE_OP

    struct SubtractionOp final  : public BinaryOperator
    {
        SubtractionOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::minus) {}
        var getWithDoubles (double a, double b) const override { return a - b; }
        var getWithInts (int64 a, int64 b) const override      { return a - b; }
    };

    struct MultiplyOp final  : public BinaryOperator
    {
        MultiplyOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::times) {}
        var getWithDoubles (double a, double b) const override { return a * b; }
        var getWithInts (int64 a, int64 b) const override      { return a * b; }
    };

    struct DivideOp final  : public BinaryOperator
    {
        DivideOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::divide) {}
        var getWithDoubles (double a, double b) const override  { return b != 0 ? a / b : std::numeric_limits<double>::infinity(); }
        var getWithInts (int64 a, int64 b) const override       { return b != 0 ? var ((double) a / (double) b) : var (std::numeric_limits<double>::infinity()); }
    };

    struct ModuloOp final  : public BinaryOperator
    {
        ModuloOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::modulo) {}
        var getWithDoubles (double a, double b) const override  { return b != 0 ? fmod (a, b) : std::numeric_limits<double>::infinity(); }
        var getWithInts (int64 a, int64 b) const override       { return b != 0 ? var (a % b) : var (std::numeric_limits<double>::infinity()); }
    };

    struct BitwiseOrOp final  : public BinaryOperator
    {
        BitwiseOrOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::bitwiseOr) {}
        var getWithInts (int64 a, int64 b) const override   { return a | b; }
    };

    struct BitwiseAndOp final  : public BinaryOperator
    {
        BitwiseAndOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::bitwiseAnd) {}
        var getWithInts (int64 a, int64 b) const override   { return a & b; }
    };

    struct BitwiseXorOp final  : public BinaryOperator
    {
        BitwiseXorOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::bitwiseXor) {}
        var getWithInts (int64 a, int64 b) const override   { return a ^ b; }
    };

    struct LeftShiftOp final  : public BinaryOperator
    {
        LeftShiftOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::leftShift) {}
        var getWithInts (int64 a, int64 b) const override   { return ((int) a) << (int) b; }
    };

    struct RightShiftOp final  : public BinaryOperator
    {
        RightShiftOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::rightShift) {}
        var getWithInts (int64 a, int64 b) const override   { return ((int) a) >> (int) b; }
    };

    struct RightShiftUnsignedOp final  : public BinaryOperator
    {
        RightShiftUnsignedOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperator (l, a, b, TokenTypes::rightShiftUnsigned) {}
        var getWithInts (int64 a, int64 b) const override   { return (int) (((uint32) a) >> (int) b); }
    };

    struct LogicalAndOp final  : public BinaryOperatorBase
    {
        LogicalAndOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperatorBase (l, a, b, TokenTypes::logicalAnd) {}
        var getResult (const Scope& s) const override       { return lhs->getResult (s) && rhs->getResult (s); }
    };

    struct LogicalOrOp final  : public BinaryOperatorBase
    {
        LogicalOrOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperatorBase (l, a, b, TokenTypes::logicalOr) {}
        var getResult (const Scope& s) const override       { return lhs->getResult (s) || rhs->getResult (s); }
    };

    struct TypeEqualsOp final  : public BinaryOperatorBase
    {
        TypeEqualsOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperatorBase (l, a, b, TokenTypes::typeEquals) {}
        var getResult (const Scope& s) const override       { return areTypeEqual (lhs->getResult (s), rhs->getResult (s)); }
    };

    struct TypeNotEqualsOp final  : public BinaryOperatorBase
    {
        TypeNotEqualsOp (const CodeLocation& l, ExpPtr& a, ExpPtr& b) noexcept : BinaryOperatorBase (l, a, b, TokenTypes::typeNotEquals) {}
        var getResult (const Scope& s) const override       { return ! areTypeEqual (lhs->getResult (s), rhs->getResult (s)); }
    };

    struct ConditionalOp final  : public Expression
    {
        ConditionalOp (const CodeLocation& l) noexcept : Expression (l) {}

        var getResult (const Scope& s) const override              { return (condition->getResult (s) ? trueBranch : falseBranch)->getResult (s); }
        void assign (const Scope& s, const var& v) const override  { (condition->getResult (s) ? trueBranch : falseBranch)->assign (s, v); }

        ExpPtr condition, trueBranch, falseBranch;
    };

    struct Assignment final  : public Expression
    {
        Assignment (const CodeLocation& l, ExpPtr& dest, ExpPtr& source) noexcept : Expression (l), target (dest.release()), newValue (source.release()) {}

        var getResult (const Scope& s) const override
        {
            auto value = newValue->getResult (s);
            target->assign (s, value);
            return value;
        }

        ExpPtr target, newValue;
    };

    struct SelfAssignment  : public Expression
    {
        SelfAssignment (const CodeLocation& l, Expression* dest, Expression* source) noexcept
            : Expression (l), target (dest), newValue (source) {}

        var getResult (const Scope& s) const override
        {
            auto value = newValue->getResult (s);
            target->assign (s, value);
            return value;
        }

        Expression* target; // Careful! this pointer aliases a sub-term of newValue!
        ExpPtr newValue;
        TokenType op;
    };

    struct PostAssignment final  : public SelfAssignment
    {
        PostAssignment (const CodeLocation& l, Expression* dest, Expression* source) noexcept : SelfAssignment (l, dest, source) {}

        var getResult (const Scope& s) const override
        {
            auto oldValue = target->getResult (s);
            target->assign (s, newValue->getResult (s));
            return oldValue;
        }
    };

    struct FunctionCall  : public Expression
    {
        FunctionCall (const CodeLocation& l) noexcept : Expression (l) {}

        var getResult (const Scope& s) const override
        {
            if (auto* dot = dynamic_cast<DotOperator*> (object.get()))
            {
                auto thisObject = dot->parent->getResult (s);
                return invokeFunction (s, s.findFunctionCall (location, thisObject, dot->child), thisObject);
            }

            auto function = object->getResult (s);
            return invokeFunction (s, function, var (s.scope.get()));
        }

        var invokeFunction (const Scope& s, const var& function, const var& thisObject) const
        {
            s.checkTimeOut (location);
            Array<var> argVars;

            for (auto* a : arguments)
                argVars.add (a->getResult (s));

            const var::NativeFunctionArgs args (thisObject, argVars.begin(), argVars.size());

            if (auto nativeFunction = function.getNativeFunction())
                return nativeFunction (args);

            if (auto* fo = dynamic_cast<FunctionObject*> (function.getObject()))
                return fo->invoke (s, args);

            if (auto* dot = dynamic_cast<DotOperator*> (object.get()))
                if (auto* o = thisObject.getDynamicObject())
                    if (o->hasMethod (dot->child)) // allow an overridden DynamicObject::invokeMethod to accept a method call.
                        return o->invokeMethod (dot->child, args);

            location.throwError ("This expression is not a function!"); return {};
        }

        ExpPtr object;
        OwnedArray<Expression> arguments;
    };

    struct NewOperator final  : public FunctionCall
    {
        NewOperator (const CodeLocation& l) noexcept : FunctionCall (l) {}

        var getResult (const Scope& s) const override
        {
            var classOrFunc = object->getResult (s);
            const bool isFunc = isFunction (classOrFunc);

            if (! (isFunc || classOrFunc.getDynamicObject() != nullptr))
                return var::undefined();

            DynamicObject::Ptr newObject (new DynamicObject());

            if (isFunc)
                invokeFunction (s, classOrFunc, newObject.get());
            else
                newObject->setProperty (getPrototypeIdentifier(), classOrFunc);

            return newObject.get();
        }
    };

    struct ObjectDeclaration final  : public Expression
    {
        ObjectDeclaration (const CodeLocation& l) noexcept : Expression (l) {}

        var getResult (const Scope& s) const override
        {
            DynamicObject::Ptr newObject (new DynamicObject());
            jassert (names.size() == initialisers.size());

            for (int i = 0; i < names.size(); ++i)
                newObject->setProperty (names.getUnchecked (i), initialisers.getUnchecked (i)->getResult (s));

            return newObject.get();
        }

        Array<Identifier> names;
        OwnedArray<Expression> initialisers;
    };

    struct ArrayDeclaration final  : public Expression
    {
        ArrayDeclaration (const CodeLocation& l) noexcept : Expression (l) {}

        var getResult (const Scope& s) const override
        {
            Array<var> a;

            for (const auto& v : values)
                a.add (v->getResult (s));

            // std::move() needed here for older compilers
            JUCE_BEGIN_IGNORE_WARNINGS_GCC_LIKE ("-Wredundant-move")
            return std::move (a);
            JUCE_END_IGNORE_WARNINGS_GCC_LIKE
        }

        OwnedArray<Expression> values;
    };

    //==============================================================================
    struct FunctionObject final  : public DynamicObject
    {
        FunctionObject() noexcept {}

        FunctionObject (const FunctionObject& other)  : functionCode (other.functionCode)
        {
            ExpressionTreeBuilder tb (functionCode);
            tb.parseFunctionParamsAndBody (*this);
        }

        DynamicObject::Ptr clone() override    { return *new FunctionObject (*this); }

        void writeAsJSON (OutputStream& out, int /*indentLevel*/, bool /*allOnOneLine*/, int /*maximumDecimalPlaces*/) override
        {
            out << "function " << functionCode;
        }

        var invoke (const Scope& s, const var::NativeFunctionArgs& args) const
        {
            DynamicObject::Ptr functionRoot (new DynamicObject());

            static const Identifier thisIdent ("this");
            functionRoot->setProperty (thisIdent, args.thisObject);

            for (int i = 0; i < parameters.size(); ++i)
                functionRoot->setProperty (parameters.getReference(i),
                                           i < args.numArguments ? args.arguments[i] : var::undefined());

            var result;
            body->perform (Scope (&s, s.root, functionRoot), &result);
            return result;
        }

        String functionCode;
        Array<Identifier> parameters;
        std::unique_ptr<Statement> body;
    };

    //==============================================================================
    struct TokenIterator
    {
        TokenIterator (const String& code) : location (code), p (code.getCharPointer()) { skip(); }
        virtual ~TokenIterator() {}

        void skip()
        {
            skipWhitespaceAndComments();
            location.location = p;
            currentType = matchNextToken();
        }

        void match (TokenType expected)
        {
            if (currentType != expected)
                location.throwError ("Found " + getTokenName (currentType) + " when expecting " + getTokenName (expected));

            skip();
        }

        bool matchIf (TokenType expected)                                 { if (currentType == expected)  { skip(); return true; } return false; }
        bool matchesAny (TokenType t1, TokenType t2) const                { return currentType == t1 || currentType == t2; }
        bool matchesAny (TokenType t1, TokenType t2, TokenType t3) const  { return matchesAny (t1, t2) || currentType == t3; }

        CodeLocation location;
        TokenType currentType;
        var currentValue;

    private:
        String::CharPointerType p;

        static bool isIdentifierStart (juce_wchar c) noexcept   { return CharacterFunctions::isLetter (c)        || c == '_'; }
        static bool isIdentifierBody  (juce_wchar c) noexcept   { return CharacterFunctions::isLetterOrDigit (c) || c == '_'; }

        TokenType matchNextToken()
        {
            if (isIdentifierStart (*p))
            {
                auto end = p;
                while (isIdentifierBody (*++end)) {}

                auto len = (size_t) (end - p);
                #define JUCE_JS_COMPARE_KEYWORD(name, str) if (len == sizeof (str) - 1 && matchToken (TokenTypes::name, len)) return TokenTypes::name;
                JUCE_JS_KEYWORDS (JUCE_JS_COMPARE_KEYWORD)

                currentValue = String (p, end); p = end;
                return TokenTypes::identifier;
            }

            if (p.isDigit())
            {
                if (parseHexLiteral() || parseFloatLiteral() || parseOctalLiteral() || parseDecimalLiteral())
                    return TokenTypes::literal;

                location.throwError ("Syntax error in numeric constant");
            }

            if (parseStringLiteral (*p) || (*p == '.' && parseFloatLiteral()))
                return TokenTypes::literal;

            #define JUCE_JS_COMPARE_OPERATOR(name, str) if (matchToken (TokenTypes::name, sizeof (str) - 1)) return TokenTypes::name;
            JUCE_JS_OPERATORS (JUCE_JS_COMPARE_OPERATOR)

            if (! p.isEmpty())
                location.throwError ("Unexpected character '" + String::charToString (*p) + "' in source");

            return TokenTypes::eof;
        }

        bool matchToken (TokenType name, size_t len) noexcept
        {
            if (p.compareUpTo (CharPointer_ASCII (name), (int) len) != 0)
                return false;

            p += (int) len;
            return true;
        }

        void skipWhitespaceAndComments()
        {
            for (;;)
            {
                p = p.findEndOfWhitespace();

                if (*p == '/')
                {
                    auto c2 = p[1];

                    if (c2 == '/')
                    {
                        p = CharacterFunctions::find (p, (juce_wchar) '\n');
                        continue;
                    }

                    if (c2 == '*')
                    {
                        location.location = p;
                        p = CharacterFunctions::find (p + 2, CharPointer_ASCII ("*/"));
                        if (p.isEmpty()) location.throwError ("Unterminated '/*' comment");
                        p += 2; continue;
                    }
                }

                break;
            }
        }

        bool parseStringLiteral (juce_wchar quoteType)
        {
            if (quoteType != '"' && quoteType != '\'')
                return false;

            const auto r = JSON::parseQuotedString (p, currentValue);
            if (r.failed())
                location.throwError (r.getErrorMessage());

            return true;
        }

        bool parseHexLiteral()
        {
            if (*p != '0' || (p[1] != 'x' && p[1] != 'X'))
                return false;

            auto t = ++p;
            auto v = static_cast<int64> (CharacterFunctions::getHexDigitValue (*++t));
            if (v < 0)
                return false;

            for (;;)
            {
                auto digit = CharacterFunctions::getHexDigitValue (*++t);
                if (digit < 0)
                    break;

                v = v * 16 + digit;
            }

            currentValue = v;
            p = t;
            return true;
        }

        bool parseFloatLiteral()
        {
            int numDigits = 0;
            auto t = p;
            while (t.isDigit())
            {
                ++t;
                ++numDigits;
            }

            const bool hasPoint = *t == '.';

            if (hasPoint)
                while ((++t).isDigit())  ++numDigits;

            if (numDigits == 0)
                return false;

            auto c = *t;
            const bool hasExponent = (c == 'e' || c == 'E');

            if (hasExponent)
            {
                c = *++t;
                if (c == '+' || c == '-')
                    ++t;

                if (! t.isDigit())
                    return false;

                while ((++t).isDigit())
                {
                }
            }

            if (! (hasExponent || hasPoint))
                return false;

            currentValue = CharacterFunctions::getDoubleValue (p);  p = t;
            return true;
        }

        bool parseOctalLiteral()
        {
            auto t = p;
            int64 v = *t - '0';
            if (v != 0)
                return false;  // first digit of octal must be 0

            for (;;)
            {
                auto digit = (int) (*++t - '0');
                if (isPositiveAndBelow (digit, 8))        v = v * 8 + digit;
                else if (isPositiveAndBelow (digit, 10))  location.throwError ("Decimal digit in octal constant");
                else break;
            }

            currentValue = v;
            p = t;
            return true;
        }

        bool parseDecimalLiteral()
        {
            int64 v = 0;

            for (;; ++p)
            {
                auto digit = (int) (*p - '0');
                if (isPositiveAndBelow (digit, 10))
                    v = v * 10 + digit;
                else
                    break;
            }

            currentValue = v;
            return true;
        }
    };

    //==============================================================================
    struct ExpressionTreeBuilder final  : private TokenIterator
    {
        ExpressionTreeBuilder (const String code)  : TokenIterator (code) {}

        BlockStatement* parseStatementList()
        {
            auto b = std::make_unique<BlockStatement> (location);

            while (currentType != TokenTypes::closeBrace && currentType != TokenTypes::eof)
                b->statements.add (parseStatement());

            return b.release();
        }

        void parseFunctionParamsAndBody (FunctionObject& fo)
        {
            match (TokenTypes::openParen);

            while (currentType != TokenTypes::closeParen)
            {
                auto paramName = currentValue.toString();
                match (TokenTypes::identifier);
                fo.parameters.add (paramName);

                if (currentType != TokenTypes::closeParen)
                    match (TokenTypes::comma);
            }

            match (TokenTypes::closeParen);
            fo.body.reset (parseBlock());
        }

        ExpPtr parseExpression()
        {
            ExpPtr lhs (parseLogicOperator());

            if (matchIf (TokenTypes::question))          return parseTernaryOperator (lhs);
            if (matchIf (TokenTypes::assign))            { ExpPtr rhs (parseExpression()); ExpPtr a (new Assignment (location, lhs, rhs)); return std::move (a); }
            if (matchIf (TokenTypes::plusEquals))        return parseInPlaceOpExpression<AdditionOp> (lhs);
            if (matchIf (TokenTypes::minusEquals))       return parseInPlaceOpExpression<SubtractionOp> (lhs);
            if (matchIf (TokenTypes::timesEquals))       return parseInPlaceOpExpression<MultiplyOp> (lhs);
            if (matchIf (TokenTypes::divideEquals))      return parseInPlaceOpExpression<DivideOp> (lhs);
            if (matchIf (TokenTypes::moduloEquals))      return parseInPlaceOpExpression<ModuloOp> (lhs);
            if (matchIf (TokenTypes::leftShiftEquals))   return parseInPlaceOpExpression<LeftShiftOp> (lhs);
            if (matchIf (TokenTypes::rightShiftEquals))  return parseInPlaceOpExpression<RightShiftOp> (lhs);

            return std::move (lhs);
        }

    private:
        void throwError (const String& err) const { location.throwError (err); }

        template<typename OpType>
        ExpPtr parseInPlaceOpExpression (ExpPtr& lhs)
        {
            auto rhs = parseExpression();
            auto* bareLHS = lhs.get(); // careful - bare pointer is deliberately aliased
            return std::make_unique<SelfAssignment> (location, bareLHS, new OpType (location, lhs, rhs));
        }

        BlockStatement* parseBlock()
        {
            match (TokenTypes::openBrace);
            auto* b = parseStatementList();
            match (TokenTypes::closeBrace);
            return b;
        }

        Statement* parseStatement()
        {
            if (currentType == TokenTypes::openBrace)   return parseBlock();
            if (matchIf (TokenTypes::var))              return parseVar();
            if (matchIf (TokenTypes::if_))              return parseIf();
            if (matchIf (TokenTypes::while_))           return parseDoOrWhileLoop (false);
            if (matchIf (TokenTypes::do_))              return parseDoOrWhileLoop (true);
            if (matchIf (TokenTypes::for_))             return parseForLoop();
            if (matchIf (TokenTypes::return_))          return parseReturn();
            if (matchIf (TokenTypes::break_))           return new BreakStatement (location);
            if (matchIf (TokenTypes::continue_))        return new ContinueStatement (location);
            if (matchIf (TokenTypes::function))         return parseFunction();
            if (matchIf (TokenTypes::semicolon))        return new Statement (location);
            if (matchIf (TokenTypes::plusplus))         return parsePreIncDec<AdditionOp>().release();
            if (matchIf (TokenTypes::minusminus))       return parsePreIncDec<SubtractionOp>().release();

            if (matchesAny (TokenTypes::openParen, TokenTypes::openBracket))
                return matchEndOfStatement (parseFactor()).release();

            if (matchesAny (TokenTypes::identifier, TokenTypes::literal, TokenTypes::minus))
                return matchEndOfStatement (parseExpression()).release();

            throwError ("Found " + getTokenName (currentType) + " when expecting a statement");
            return nullptr;
        }

        ExpPtr matchEndOfStatement (ExpPtr e)   { if (currentType != TokenTypes::eof) match (TokenTypes::semicolon); return std::move (e); }
        ExpPtr matchCloseParen (ExpPtr e)       { match (TokenTypes::closeParen); return std::move (e); }

        Statement* parseIf()
        {
            auto s = std::make_unique<IfStatement> (location);
            match (TokenTypes::openParen);
            s->condition = parseExpression();
            match (TokenTypes::closeParen);
            s->trueBranch.reset (parseStatement());
            s->falseBranch.reset (matchIf (TokenTypes::else_) ? parseStatement() : new Statement (location));
            return s.release();
        }

        Statement* parseReturn()
        {
            if (matchIf (TokenTypes::semicolon))
                return new ReturnStatement (location, std::make_unique<Expression> (location));

            auto* r = new ReturnStatement (location, parseExpression());
            matchIf (TokenTypes::semicolon);
            return r;
        }

        Statement* parseVar()
        {
            auto s = std::make_unique<VarStatement> (location);
            s->name = parseIdentifier();
            s->initialiser.reset (matchIf (TokenTypes::assign) ? parseExpression().release() : new Expression (location));

            if (matchIf (TokenTypes::comma))
            {
                auto b = std::make_unique<BlockStatement> (location);
                b->statements.add (std::move (s));
                b->statements.add (parseVar());
                return b.release();
            }

            match (TokenTypes::semicolon);
            return s.release();
        }

        Statement* parseFunction()
        {
            Identifier name;
            auto fn = parseFunctionDefinition (name);

            if (name.isNull())
                throwError ("Functions defined at statement-level must have a name");

            ExpPtr nm (new UnqualifiedName (location, name)), value (new LiteralValue (location, fn));
            return new Assignment (location, nm, value);
        }

        Statement* parseForLoop()
        {
            auto s = std::make_unique<LoopStatement> (location, false);
            match (TokenTypes::openParen);
            s->initialiser.reset (parseStatement());

            if (matchIf (TokenTypes::semicolon))
            {
                s->condition.reset (new LiteralValue (location, true));
            }
            else
            {
                s->condition = parseExpression();
                match (TokenTypes::semicolon);
            }

            if (matchIf (TokenTypes::closeParen))
            {
                s->iterator.reset (new Statement (location));
            }
            else
            {
                s->iterator = parseExpression();
                match (TokenTypes::closeParen);
            }

            s->body.reset (parseStatement());
            return s.release();
        }

        Statement* parseDoOrWhileLoop (bool isDoLoop)
        {
            auto s = std::make_unique<LoopStatement> (location, isDoLoop);
            s->initialiser.reset (new Statement (location));
            s->iterator.reset (new Statement (location));

            if (isDoLoop)
            {
                s->body.reset (parseBlock());
                match (TokenTypes::while_);
            }

            match (TokenTypes::openParen);
            s->condition = parseExpression();
            match (TokenTypes::closeParen);

            if (! isDoLoop)
                s->body.reset (parseStatement());

            return s.release();
        }

        Identifier parseIdentifier()
        {
            Identifier i;
            if (currentType == TokenTypes::identifier)
                i = currentValue.toString();

            match (TokenTypes::identifier);
            return i;
        }

        var parseFunctionDefinition (Identifier& functionName)
        {
            auto functionStart = location.location;

            if (currentType == TokenTypes::identifier)
                functionName = parseIdentifier();

            auto fo = std::make_unique<FunctionObject>();
            parseFunctionParamsAndBody (*fo);
            fo->functionCode = String (functionStart, location.location);
            return var (fo.release());
        }

        ExpPtr parseFunctionCall (FunctionCall* call, ExpPtr& function)
        {
            jassert (call != nullptr);

            call->object.reset (function.release());
            match (TokenTypes::openParen);

            while (currentType != TokenTypes::closeParen)
            {
                call->arguments.add (parseExpression());
                if (currentType != TokenTypes::closeParen)
                    match (TokenTypes::comma);
            }

            return matchCloseParen (ExpPtr (call));
        }

        ExpPtr parseSuffixes (ExpPtr&& input)
        {
            if (matchIf (TokenTypes::dot))
                return parseSuffixes (ExpPtr (new DotOperator (location, input, parseIdentifier())));

            if (currentType == TokenTypes::openParen)
                return parseSuffixes (parseFunctionCall (new FunctionCall (location), input));

            if (matchIf (TokenTypes::openBracket))
            {
                auto s = std::make_unique<ArraySubscript> (location);
                s->object = std::move (input);
                s->index = parseExpression();
                match (TokenTypes::closeBracket);
                return parseSuffixes (std::move (s));
            }

            if (matchIf (TokenTypes::plusplus))   return parsePostIncDec<AdditionOp> (input);
            if (matchIf (TokenTypes::minusminus)) return parsePostIncDec<SubtractionOp> (input);

            return std::move (input);
        }

        ExpPtr parseFactor()
        {
            if (currentType == TokenTypes::identifier)  return parseSuffixes (ExpPtr (new UnqualifiedName (location, parseIdentifier())));
            if (matchIf (TokenTypes::openParen))        return parseSuffixes (matchCloseParen (parseExpression()));
            if (matchIf (TokenTypes::true_))            return parseSuffixes (ExpPtr (new LiteralValue (location, (int) 1)));
            if (matchIf (TokenTypes::false_))           return parseSuffixes (ExpPtr (new LiteralValue (location, (int) 0)));
            if (matchIf (TokenTypes::null_))            return parseSuffixes (ExpPtr (new LiteralValue (location)));
            if (matchIf (TokenTypes::infinity_))        return parseSuffixes (ExpPtr (new LiteralValue (location, std::numeric_limits<double>::infinity())));
            if (matchIf (TokenTypes::undefined))        return parseSuffixes (ExpPtr (new Expression (location)));

            if (currentType == TokenTypes::literal)
            {
                var v (currentValue);
                skip();
                return parseSuffixes (ExpPtr (new LiteralValue (location, v)));
            }

            if (matchIf (TokenTypes::openBrace))
            {
                auto e = std::make_unique<ObjectDeclaration> (location);

                while (currentType != TokenTypes::closeBrace)
                {
                    auto memberName = currentValue.toString();
                    match ((currentType == TokenTypes::literal && currentValue.isString())
                             ? TokenTypes::literal : TokenTypes::identifier);
                    match (TokenTypes::colon);

                    e->names.add (memberName);
                    e->initialisers.add (parseExpression());

                    if (currentType != TokenTypes::closeBrace)
                        match (TokenTypes::comma);
                }

                match (TokenTypes::closeBrace);
                return parseSuffixes (std::move (e));
            }

            if (matchIf (TokenTypes::openBracket))
            {
                auto e = std::make_unique<ArrayDeclaration> (location);

                while (currentType != TokenTypes::closeBracket)
                {
                    e->values.add (parseExpression());

                    if (currentType != TokenTypes::closeBracket)
                        match (TokenTypes::comma);
                }

                match (TokenTypes::closeBracket);
                return parseSuffixes (std::move (e));
            }

            if (matchIf (TokenTypes::function))
            {
                Identifier name;
                auto fn = parseFunctionDefinition (name);

                if (name.isValid())
                    throwError ("Inline functions definitions cannot have a name");

                return ExpPtr (new LiteralValue (location, fn));
            }

            if (matchIf (TokenTypes::new_))
            {
                ExpPtr name (new UnqualifiedName (location, parseIdentifier()));

                while (matchIf (TokenTypes::dot))
                    name.reset (new DotOperator (location, name, parseIdentifier()));

                return parseFunctionCall (new NewOperator (location), name);
            }

            throwError ("Found " + getTokenName (currentType) + " when expecting an expression");
            return nullptr;
        }

        template<typename OpType>
        ExpPtr parsePreIncDec()
        {
            auto* e = parseFactor().release(); // careful - bare pointer is deliberately aliased
            ExpPtr lhs (e),
                   one (new LiteralValue (location, (int) 1)),
                   sa (new SelfAssignment (location, e, new OpType (location, lhs, one)));
            return std::move (sa);
        }

        template<typename OpType>
        ExpPtr parsePostIncDec (ExpPtr& lhs)
        {
            auto e = lhs.release(); // careful - bare pointer is deliberately aliased
            ExpPtr lhs2 (e),
                   one (new LiteralValue (location, (int) 1)),
                   sa (new PostAssignment (location, e, new OpType (location, lhs2, one)));
            return std::move (sa);
        }

        ExpPtr parseTypeof()
        {
            auto f = std::make_unique<FunctionCall> (location);
            f->object.reset (new UnqualifiedName (location, "typeof"));
            f->arguments.add (parseUnary());
            return std::move (f);
        }

        ExpPtr parseInstanceof()
        {
            auto f = std::make_unique<FunctionCall> (location);
            f->object.reset (new UnqualifiedName (location, "instanceof"));
            f->arguments.add (parseUnary());
            return std::move (f);
        }

        ExpPtr parseUnary()
        {
            if (matchIf (TokenTypes::minus))       { ExpPtr a (new LiteralValue (location, (int) 0)), b (parseUnary()); ExpPtr c (new SubtractionOp (location, a, b)); return c; }
            if (matchIf (TokenTypes::logicalNot))  { ExpPtr a (new LiteralValue (location, (int) 0)), b (parseUnary()); ExpPtr c (new EqualsOp (location, a, b)); return c; }
            if (matchIf (TokenTypes::plusplus))    return parsePreIncDec<AdditionOp>();
            if (matchIf (TokenTypes::minusminus))  return parsePreIncDec<SubtractionOp>();
            if (matchIf (TokenTypes::typeof_))     return parseTypeof();
            if (matchIf (TokenTypes::instanceof_)) return parseInstanceof();

            return parseFactor();
        }

        ExpPtr parseMultiplyDivide()
        {
            auto a = parseUnary();

            for (;;)
            {
                if (matchIf (TokenTypes::times))        { ExpPtr b (parseUnary()); a.reset (new MultiplyOp (location, a, b)); }
                else if (matchIf (TokenTypes::divide))  { ExpPtr b (parseUnary()); a.reset (new DivideOp   (location, a, b)); }
                else if (matchIf (TokenTypes::modulo))  { ExpPtr b (parseUnary()); a.reset (new ModuloOp   (location, a, b)); }
                else break;
            }

            return std::move (a);
        }

        ExpPtr parseAdditionSubtraction()
        {
            auto a = parseMultiplyDivide();

            for (;;)
            {
                if (matchIf (TokenTypes::plus))            { ExpPtr b (parseMultiplyDivide()); a.reset (new AdditionOp    (location, a, b)); }
                else if (matchIf (TokenTypes::minus))      { ExpPtr b (parseMultiplyDivide()); a.reset (new SubtractionOp (location, a, b)); }
                else break;
            }

            return std::move (a);
        }

        ExpPtr parseShiftOperator()
        {
            auto a = parseAdditionSubtraction();

            for (;;)
            {
                if (matchIf (TokenTypes::leftShift))                { ExpPtr b (parseExpression()); a.reset (new LeftShiftOp          (location, a, b)); }
                else if (matchIf (TokenTypes::rightShift))          { ExpPtr b (parseExpression()); a.reset (new RightShiftOp         (location, a, b)); }
                else if (matchIf (TokenTypes::rightShiftUnsigned))  { ExpPtr b (parseExpression()); a.reset (new RightShiftUnsignedOp (location, a, b)); }
                else break;
            }

            return std::move (a);
        }

        ExpPtr parseComparator()
        {
            auto a = parseShiftOperator();

            for (;;)
            {
                if (matchIf (TokenTypes::equals))                  { ExpPtr b (parseShiftOperator()); a.reset (new EqualsOp             (location, a, b)); }
                else if (matchIf (TokenTypes::notEquals))          { ExpPtr b (parseShiftOperator()); a.reset (new NotEqualsOp          (location, a, b)); }
                else if (matchIf (TokenTypes::typeEquals))         { ExpPtr b (parseShiftOperator()); a.reset (new TypeEqualsOp         (location, a, b)); }
                else if (matchIf (TokenTypes::typeNotEquals))      { ExpPtr b (parseShiftOperator()); a.reset (new TypeNotEqualsOp      (location, a, b)); }
                else if (matchIf (TokenTypes::lessThan))           { ExpPtr b (parseShiftOperator()); a.reset (new LessThanOp           (location, a, b)); }
                else if (matchIf (TokenTypes::lessThanOrEqual))    { ExpPtr b (parseShiftOperator()); a.reset (new LessThanOrEqualOp    (location, a, b)); }
                else if (matchIf (TokenTypes::greaterThan))        { ExpPtr b (parseShiftOperator()); a.reset (new GreaterThanOp        (location, a, b)); }
                else if (matchIf (TokenTypes::greaterThanOrEqual)) { ExpPtr b (parseShiftOperator()); a.reset (new GreaterThanOrEqualOp (location, a, b)); }
                else break;
            }

            return std::move (a);
        }

        ExpPtr parseLogicOperator()
        {
            auto a = parseComparator();

            for (;;)
            {
                if (matchIf (TokenTypes::logicalAnd))       { ExpPtr b (parseComparator()); a.reset (new LogicalAndOp (location, a, b)); }
                else if (matchIf (TokenTypes::logicalOr))   { ExpPtr b (parseComparator()); a.reset (new LogicalOrOp  (location, a, b)); }
                else if (matchIf (TokenTypes::bitwiseAnd))  { ExpPtr b (parseComparator()); a.reset (new BitwiseAndOp (location, a, b)); }
                else if (matchIf (TokenTypes::bitwiseOr))   { ExpPtr b (parseComparator()); a.reset (new BitwiseOrOp  (location, a, b)); }
                else if (matchIf (TokenTypes::bitwiseXor))  { ExpPtr b (parseComparator()); a.reset (new BitwiseXorOp (location, a, b)); }
                else break;
            }

            return std::move (a);
        }

        ExpPtr parseTernaryOperator (ExpPtr& condition)
        {
            auto e = std::make_unique<ConditionalOp> (location);
            e->condition = std::move (condition);
            e->trueBranch = parseExpression();
            match (TokenTypes::colon);
            e->falseBranch = parseExpression();
            return std::move (e);
        }

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (ExpressionTreeBuilder)
    };

    //==============================================================================
    static var get (Args a, int index) noexcept                 { return isPositiveAndBelow (index, a.numArguments) ? a.arguments[index] : var(); }
    static bool isInt (Args a, int index) noexcept              { return get (a, index).isInt() || get (a, index).isInt64(); }
    static int getInt (Args a, int index) noexcept              { return static_cast<int> (get (a, index)); }
    static bool isDouble (Args a, int index) noexcept           { return get (a, index).isDouble(); }
    static double getDouble (Args a, int index) noexcept        { return static_cast<double> (get (a, index)); }
    static bool isString (Args a, int index) noexcept           { return get (a, index).isString(); }
    static String getString (Args a, int index) noexcept        { return get (a, index).toString(); }
    static bool isArray (Args a, int index) noexcept            { return get (a, index).isArray(); }
    static Array<var>* getArray (Args a, int index) noexcept    { return get (a, index).getArray(); }

    static var trace (Args a)                                   { Logger::outputDebugString (JSON::toString (a.thisObject)); return var::undefined(); }
    static var charToInt (Args a)                               { return (int) getString (a, 0)[0]; }

    static var encodeURI (Args a)
    {
        if (! isString (a, 0) || a.numArguments > 1)
            return var::undefined();

        return URL::addEscapeChars (getString (a, 0), false);
    }

    static var encodeURIComponent (Args a)
    {
        if (! isString (a, 0) || a.numArguments > 1)
            return var::undefined();

        return URL::addEscapeChars (getString (a, 0), true);
    }

    static var decodeURI (Args a)
    {
        if (! isString (a, 0) || a.numArguments > 1)
            return var::undefined();

        return URL::removeEscapeChars (getString (a, 0));
    }

    static var decodeURIComponent (Args a)
    {
        return decodeURI (a);
    }

    //==============================================================================
    static var typeof_internal (Args a)
    {
        const auto v = get (a, 0);

        if (v.isVoid())                      return "void";
        if (v.isString())                    return "string";
        if (v.isBool())                      return "boolean";
        if (isNumeric (v))                   return "number";
        if (isFunction (v) || v.isMethod())  return "function";
        if (v.isObject())                    return "object";

        return "undefined";
    }

    static var instanceof_internal (Args a)
    {
        const auto v = get (a, 0);

        if (v.isVoid())                      return "void";
        if (v.isString())                    return "string";
        if (v.isBool())                      return "boolean";
        if (isNumeric (v))                   return "number";
        if (isFunction (v) || v.isMethod())  return "function";
        if (v.isObject())                    return "object";

        return "undefined";
    }

    static var exec (Args a)
    {
        if (auto* root = dynamic_cast<RootObject*> (a.thisObject.getObject()))
            root->execute (getString (a, 0));

        return var::undefined();
    }

    static var eval (Args a)
    {
        if (auto* root = dynamic_cast<RootObject*> (a.thisObject.getObject()))
            return root->evaluate (getString (a, 0));

        return var::undefined();
    }

    #define JUCE_JS_CREATE_METHOD(methodName) \
        setMethod (JUCE_STRINGIFY (methodName), methodName);

    //==============================================================================
    /** A simple helper class to allow debugging and testing using copy/paste Javascript code from official examples online. */
    struct ConsoleClass final  : public DynamicObject
    {
        ConsoleClass()
        {
            setMethod ("log", logMethod);
            setMethod ("print", logMethod);
        }

        JUCE_JS_IDENTIFY_CLASS ("console")

        static var logMethod (Args a)
        {
            ignoreUnused (a);

            MemoryOutputStream mo (1024);

            for (int i = 0; i < a.numArguments; ++i)
                mo << a.arguments[i].toString() << newLine;

            Logger::writeToLog (mo.toString());

            return var::undefined();
        }
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Object
    */
    struct ObjectClass final  : public DynamicObject
    {
        ObjectClass()
        {
            #define OBJECT_CLASS_METHODS(X) \
                X (assign)                      X (create)                      X (defineProperties)    X (defineProperty) \
                X (dump)                        X (entries)                     X (freeze)              X (fromEntires) \
                X (getOwnPropertyDescriptor)    X (getOwnPropertyDescriptors)   X (getOwnPropertyNames) X (getOwnPropertySymbols) \
                X (getPrototypeOf)              X (is)                          X (isExtensible)        X (isFrozen) \
                X (isSealed)                    X (keys)                        X (preventExtensions)   X (seal) \
                X (setProtypeOf)                X (values)

            OBJECT_CLASS_METHODS (JUCE_JS_CREATE_METHOD)

            #undef OBJECT_CLASS_METHODS

            setMethod ("clone",  cloneFn);
        }

        JUCE_JS_IDENTIFY_CLASS ("Object")

        //NB: These are non-standard methods - just helpers...
        static var dump (Args a)                        { DBG (JSON::toString (a.thisObject)); ignoreUnused (a); return var::undefined(); }
        static var cloneFn (Args a)                     { return a.thisObject.clone(); }

        static var assign (Args a)                      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var create (Args a)                      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var defineProperties (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var defineProperty (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var entries (Args a)                     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var freeze (Args a)                      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var fromEntires (Args a)                 { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getOwnPropertyDescriptor (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getOwnPropertyDescriptors (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getOwnPropertyNames (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getOwnPropertySymbols (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getPrototypeOf (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var is (Args a)                          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var isExtensible (Args a)                { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var isFrozen (Args a)                    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var isSealed (Args a)                    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var keys (Args a)                        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var preventExtensions (Args a)           { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var seal (Args a)                        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setProtypeOf (Args a)                { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var values (Args a)                      { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    /*
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array
    */
    struct ArrayClass final  : public DynamicObject
    {
        ArrayClass()
        {
            #define ARRAY_CLASS_METHODS(X) \
                X (concat)      X (contains)    X (copyWithin)      X (entries) \
                X (every)       X (fill)        X (filter)          X (find) \
                X (findIndex)   X (forEach)     X (from)            X (includes) \
                X (indexOf)     X (isArray)     X (join)            X (keys) \
                X (lastIndexOf) X (map)         X (observe)         X (of) \
                X (pop)         X (push)        X (reduce)          X (reduceRight) \
                X (reverse)     X (shift)       X (slice)           X (some) \
                X (sort)        X (splice)      X (toLocaleString)  X (toSource) \
                X (toString)    X (unshift)     X (values)

            ARRAY_CLASS_METHODS (JUCE_JS_CREATE_METHOD)

            #undef ARRAY_CLASS_METHODS
        }

        JUCE_JS_IDENTIFY_CLASS ("Array")
        static Array<var>* getThisArray (Args a) { return a.thisObject.getArray(); }

        static var entries (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var from (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var isArray (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var keys (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var map (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var observe (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var of (Args a)              { return concat (a); }
        static var slice (Args a)           { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var some (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toLocaleString (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toSource (Args a)        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toString (Args a)        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var unshift (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var values (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var reduceRight (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO

        static var reduce (Args a)
        {
            if (auto* sourceArray = getThisArray (a))
            {
                if (a.numArguments > 0)
                {
                    auto sortMethod = get (a, 0);
                    if (sortMethod.isMethod())
                    {
                        jassertfalse; //TODO!
                        //sourceArray->sort();
                    }
                    else
                    {
                        jassertfalse; //Bogus sort function!
                    }
                }
                else
                {
                    sourceArray->sort();
                }
            }

            return var::undefined();
        }

        static var sort (Args a)
        {
            if (auto* sourceArray = getThisArray (a))
            {
                if (a.numArguments > 0)
                {
                    auto sortMethod = get (a, 0);
                    if (sortMethod.isMethod())
                    {
                        jassertfalse; //TODO!
                        //sourceArray->sort();
                    }
                    else
                    {
                        jassertfalse; //Bogus sort function!
                    }
                }
                else
                {
                    sourceArray->sort();
                }
            }

            return var::undefined();
        }

        static var concat (Args a)
        {
            Array<var> result;

            if (auto* sourceArray = getThisArray (a))
            {
                result = *sourceArray;

                for (int i = 0; i < a.numArguments; ++i)
                {
                    auto target = get (a, 0);

                    if (auto* targetArray = target.getArray())
                    {
                        result.addArray (*targetArray);
                    }
                    else if (target.isInt() || target.isInt64() || target.isBool()
                          || target.isDouble() || target.isString())
                    {
                        result.add (target);
                    }
                }
            }

            result.minimiseStorageOverheads();
            return result;
        }

        static var contains (Args a)
        {
            if (auto* array = getThisArray (a))
                return array->contains (get (a, 0));

            return false;
        }

        static const var& copyWithin (Args a)
        {
            if (auto* sourceArray = getThisArray (a))
            {
                const int length = sourceArray->size();
                const int startIndex = getInt (a, 0);

                if (startIndex < 0 && std::abs (startIndex) > length)
                    return a.thisObject;

                jassertfalse; //TODO
                return a.thisObject;
            }

            static auto ref = var::undefined();
            return ref;
        }

        static var every (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);

                if (auto func = target.getNativeFunction())
                {
                    for (int i = 0; i < array->size(); ++i)
                    {
                        const var arguments[] =
                        {
                            array->getUnchecked (i),
                            i,
                            a.thisObject
                        };

                        if (static_cast<bool> (func ({ target, arguments, 3 })))
                            return true;
                    }
                }
                else
                {
                    if (auto* p = dynamic_cast<FunctionObject*> (target.getDynamicObject()))
                    {
                        for (int i = 0; i < array->size(); ++i)
                        {
                            const var arguments[] =
                            {
                                array->getUnchecked (i),
                                i,
                                a.thisObject
                            };

                            if (static_cast<bool> (p->invoke ({ nullptr, nullptr, p }, { target, arguments, 3 })))
                                return true;
                        }
                    }
                }
            }

            return false;
        }

        static var fill (Args a)
        {
            if (auto* sourceArray = getThisArray (a))
            {
                const auto value = get (a, 0);
                const auto length = sourceArray->size();

                auto start  = a.numArguments >= 1 ? getInt (a, 1) : 0;
                auto end    = a.numArguments >= 2 ? getInt (a, 2) : length;

                if (start < 0)
                    start = length + start;

                if (end < 0)
                    end = length + end;

                if (start > end)
                    std::swap (start, end);

                for (int i = start; i < end; ++i)
                    sourceArray->getReference (i) = value;

                return *sourceArray;
            }

            return Array<var>(); //NB: Purposely constructing an empty array of vars here
        }

        static var filter (Args a)
        {
            Array<var> resultArray;

            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);

                if (auto func = target.getNativeFunction())
                {
                    for (int i = 0; i < array->size(); ++i)
                    {
                        const var arguments[] =
                        {
                            array->getUnchecked (i),
                            i,
                            a.thisObject
                        };

                        auto result = func ({ target, arguments, 3 });
                        if (! result.isBool())
                        {
                            jassertfalse; //Hackerman strikes again.
                            return var::undefined();
                        }

                        if (static_cast<bool> (result))
                            resultArray.add (array->getUnchecked (i));
                    }
                }
            }

            resultArray.minimiseStorageOverheads();
            return resultArray;
        }

        static var find (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);

                if (auto func = target.getNativeFunction())
                {
                    for (int i = 0; i < array->size(); ++i)
                    {
                        const var arguments[] =
                        {
                            array->getUnchecked (i),
                            i,
                            a.thisObject
                        };

                        auto result = func ({ target, arguments, 3 });
                        if (result.isBool())
                        {
                            if (static_cast<bool> (result))
                                return array->getUnchecked (i);
                        }
                        else
                        {
                            jassertfalse; //Hackerman strikes again.
                            break;
                        }

                    }
                }
            }

            return var::undefined();
        }

        static var findIndex (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);

                if (auto func = target.getNativeFunction())
                {
                    for (int i = 0; i < array->size(); ++i)
                    {
                        const var arguments[] =
                        {
                            array->getUnchecked (i),
                            i,
                            a.thisObject
                        };

                        auto result = func ({ target, arguments, 3 });
                        if (! result.isBool())
                        {
                            jassertfalse; //Hackerman strikes again.
                            return var::undefined();
                        }

                        if (static_cast<bool> (result))
                            return i;
                    }
                }
            }

            return -1;
        }

        static var forEach (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);

                if (auto func = target.getNativeFunction())
                {
                    for (int i = 0; i < array->size(); ++i)
                    {
                        const var arguments[] =
                        {
                            array->getUnchecked (i),
                            i,
                            a.thisObject
                        };

                        func ({ target, arguments, 3 });
                    }
                }
            }

            return var::undefined();
        }

        static var includes (Args a)
        {
            if (a.numArguments <= 0)
            {
                jassertfalse;
                return false;
            }

            if (auto* array = getThisArray (a))
                return array->contains (get (a, 0));

            return false;
        }

        static var indexOf (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);

                for (int i = (a.numArguments > 1 ? getInt (a, 1) : 0); i < array->size(); ++i)
                    if (array->getReference (i) == target)
                        return i;
            }

            return -1;
        }

        static var lastIndexOf (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto target = get (a, 0);
                const auto startIndex = (a.numArguments > 1 ? getInt (a, 1) : 0);

                for (int i = array->size(); --i >= startIndex;)
                    if (array->getReference (i) == target)
                        return i;
            }

            return -1;
        }

        /** This method is a special case where all elements of the array are joined
            into a string and returns this string, regardless of the element's type.
            @see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Array/join
            @see https://www.w3schools.com/jsref/jsref_join.asp
        */
        static var join (Args a)
        {
            StringArray strings;

            if (auto* array = getThisArray (a))
                for (const auto& v : *array)
                    strings.add (v.toString());

            return strings.joinIntoString (getString (a, 0));
        }

        static var pop (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                for (int i = 0; i < a.numArguments; ++i)
                    array->add (a.arguments[i]);

                array->removeLast();
                return array->size();
            }

            return var::undefined();
        }

        static var push (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                for (int i = 0; i < a.numArguments; ++i)
                    array->add (a.arguments[i]);

                return array->size();
            }

            return var::undefined();
        }

        static var remove (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                array->removeAllInstancesOf (get (a, 0));
                return *array;
            }

            return var::undefined();
        }

        static var reverse (Args a)
        {
            if (auto* sourceArray = getThisArray (a))
                std::reverse (sourceArray->begin(), sourceArray->end());

            return var::undefined();
        }

        static var shift (Args a)
        {
            if (auto* array = getThisArray (a))
                if (! array->isEmpty())
                    return array->removeAndReturn (0);

            return var::undefined();
        }

        static var splice (Args a)
        {
            if (auto* array = getThisArray (a))
            {
                auto arraySize = array->size();
                int start = get (a, 0);

                if (start < 0)
                    start = jmax (0, arraySize + start);
                else if (start > arraySize)
                    start = arraySize;

                const int num = a.numArguments > 1 ? jlimit (0, arraySize - start, getInt (a, 1))
                                                   : arraySize - start;

                Array<var> itemsRemoved;
                itemsRemoved.ensureStorageAllocated (num);

                for (int i = 0; i < num; ++i)
                    itemsRemoved.add (array->getReference (start + i));

                array->removeRange (start, num);

                for (int i = 2; i < a.numArguments; ++i)
                    array->insert (start++, get (a, i));

                return itemsRemoved;
            }

            return var::undefined();
        }
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/String
    */
    struct StringClass final  : public DynamicObject
    {
        StringClass()
        {
            #define STRING_CLASS_METHODS(X) \
                X (charAt)      X (charCodeAt)      X (codePointAt)     X (concat) \
                X (endsWith)    X (fromCharCode)    X (fromCodePoint)   X (includes) \
                X (indexOf)     X (lastIndexOf)     X (localeCompare)   X (match) \
                X (normalize)   X (padEnd)          X (padStart)        X (quote) \
                X (raw)         X (repeat)          X (replace)         X (search) \
                X (slice)       X (split)           X (startsWith)      X (substr) \
                X (substring)   X (toLocaleLowerCase) X (toLocaleUpperCase) X (toLowerCase) \
                X (toString)    X (toUpperCase)     X (trim)            X (trimLeft) \
                X (trimRight)   X (valueOf)

            STRING_CLASS_METHODS (JUCE_JS_CREATE_METHOD)

            #undef STRING_CLASS_METHODS
        }

        JUCE_JS_IDENTIFY_CLASS ("String")

        static String getThisString (Args a)    { return a.thisObject.toString(); }

        static var codePointAt (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var fromCodePoint (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var normalize (Args a)           { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var padEnd (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var padStart (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var raw (Args a)                 { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var search (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var slice (Args a)               { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var substr (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var valueOf (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO

        static var charAt (Args a)              { int p = getInt (a, 0); return getThisString (a).substring (p, p + 1); }
        static var charCodeAt (Args a)          { return (int) getThisString (a)[getInt (a, 0)]; }
        static var concat (Args a)              { return getThisString (a) + getString (a, 0); }
        static var endsWith (Args a)            { return getThisString (a).endsWith (getString (a, 0)); }
        static var fromCharCode (Args a)        { return String::charToString (static_cast<juce_wchar> (getInt (a, 0))); }
        static var includes (Args a)            { return getThisString (a).substring (getInt (a, 1)).contains (getString (a, 0)); }
        static var indexOf (Args a)             { return getThisString (a).indexOf (getString (a, 0)); }
        static var lastIndexOf (Args a)         { return getThisString (a).lastIndexOf (getString (a, 0)); }
        static var localeCompare (Args a)       { return getThisString (a).compare (getString (a, 0)); }
        static var match (Args a)               { std::regex r (getString (a, 0).trim().toStdString()); return std::regex_match (getThisString (a).toStdString(), r); }
        static var quote (Args a)               { return getThisString (a).quoted(); }
        static var repeat (Args a)              { return String::repeatedString (getString (a, 0), getInt (a, 1)); }
        static var replace (Args a)             { return getThisString (a).replace (getString (a, 0), getString (a, 1)); }
        static var startsWith (Args a)          { return getThisString (a).startsWith (getString (a, 0)); }
        static var substring (Args a)           { return getThisString (a).substring (getInt (a, 0), getInt (a, 1)); }
        static var toLocaleLowerCase (Args a)   { return getThisString (a).toLowerCase(); }
        static var toLocaleUpperCase (Args a)   { return getThisString (a).toUpperCase(); }
        static var toLowerCase (Args a)         { return getThisString (a).toLowerCase(); }
        static var toString (Args a)            { return getThisString (a); }
        static var toUpperCase (Args a)         { return getThisString (a).toUpperCase(); }
        static var trim (Args a)                { return getThisString (a).trim(); }
        static var trimLeft (Args a)            { return getThisString (a).trimStart(); }
        static var trimRight (Args a)           { return getThisString (a).trimEnd(); }

        static var split (Args a)
        {
            auto str = a.thisObject.toString();
            auto sep = getString (a, 0);
            StringArray strings;

            if (sep.isNotEmpty())
                strings.addTokens (str, sep.substring (0, 1), {});
            else // special-case for empty separator: split all chars separately
                for (auto pos = str.getCharPointer(); ! pos.isEmpty(); ++pos)
                    strings.add (String::charToString (*pos));

            var array;

            for (const auto& s : strings)
                array.append (s);

            return array;
        }
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Date
    */
    struct DateClass final  : public DynamicObject
    {
        DateClass()
        {
        }

        JUCE_JS_IDENTIFY_CLASS ("Date")

        //statics...
        static var UTC (Args a)                 { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var now (Args a)                 { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var parse (Args a)               { ignoreUnused (a); jassertfalse; return var(); } //TODO

        //methods...
        static var getDate (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getFullYear (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getHours (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getMilliseconds (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getMinutes (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getMonth (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getSeconds (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getTime (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getTimezoneOffset (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCDate (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCDay (Args a)           { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCFullYear (Args a)      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCHours (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCMilliseconds (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCMinutes (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCMonth (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUTCSeconds (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getYear (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setDate (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setFullYear (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setHours (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setMilliseconds (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setMinutes (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setMonth (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setSeconds (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setTime (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCDate (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCFullYear (Args a)      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCHours (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCMilliseconds (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCMinutes (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCMonth (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUTCSeconds (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setYear (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toDateString (Args a)        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toGMTString (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO

        static var toISOString (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toJSON (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toLocaleDateString (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toLocaleFormat (Args a)      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toLocaleString (Args a)      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toLocaleTimeString (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toSource (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toString (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toTimeString (Args a)        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toUTCString (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var valueOf (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    struct MathClass final  : public DynamicObject
    {
        MathClass()
        {
            #define MATH_CLASS_METHODS(X) \
                X (abs) \
                X (acos) \
                X (acosh) \
                X (asin) \
                X (asinh) \
                X (atan) \
                X (atanh) \
                X (atan2) \
                X (cbrt) \
                X (ceil) \
                X (cos) \
                X (cosh) \
                X (exp) \
                X (expm1) \
                X (floor) \
                X (fround) \
                X (hypot) \
                X (imul) \
                X (log) \
                X (log1p) \
                X (log2) \
                X (log10) \
                X (max) \
                X (min) \
                X (pow) \
                X (randInt) \
                X (random) \
                X (round) \
                X (sign) \
                X (sin) \
                X (sinh) \
                X (sqrt) \
                X (tan) \
                X (tanh) \
                X (trunc) \
                X (range) \
                X (sqr) \
                X (toDegrees) \
                X (toRadians)

            #define CREATE_MATH_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), Math_ ## methodName);

            MATH_CLASS_METHODS (CREATE_MATH_METHOD)

            #undef CREATE_MATH_METHOD
            #undef MATH_CLASS_METHODS

            const auto log2 = std::log (2.0);
            const auto log10 = std::log (2.0);

            setProperty ("PI",      MathConstants<double>::pi);
            setProperty ("E",       MathConstants<double>::euler);
            setProperty ("SQRT2",   MathConstants<double>::sqrt2);
            setProperty ("SQRT1_2", std::sqrt (0.5));
            setProperty ("LN2",     log2);
            setProperty ("LN10",    log10);
            setProperty ("LOG2E",   std::log (MathConstants<double>::euler) / log2);
            setProperty ("LOG10E",  std::log (MathConstants<double>::euler) / log10);
        }

        JUCE_JS_IDENTIFY_CLASS ("Math")

        static var Math_abs       (Args a) { return isInt (a, 0) ? var (std::abs   (getInt (a, 0))) : var (std::abs   (getDouble (a, 0))); }
        static var Math_acos      (Args a) { return std::acos  (getDouble (a, 0)); }
        static var Math_asin      (Args a) { return std::asin  (getDouble (a, 0)); }
        static var Math_atan      (Args a) { return std::atan  (getDouble (a, 0)); }
        static var Math_atan2     (Args a) { return std::atan2 (getDouble (a, 0), getDouble (a, 1)); }
        static var Math_cbrt      (Args a) { return std::cbrt  (getDouble (a, 0)); }
        static var Math_ceil      (Args a) { return std::ceil  (getDouble (a, 0)); }
        static var Math_cos       (Args a) { return std::cos   (getDouble (a, 0)); }
        static var Math_cosh      (Args a) { return std::cosh  (getDouble (a, 0)); }
        static var Math_exp       (Args a) { return std::exp   (getDouble (a, 0)); }
        static var Math_expm1     (Args a) { return std::expm1 (getDouble (a, 0)); }
        static var Math_floor     (Args a) { return std::floor (getDouble (a, 0)); }
        static var Math_fround    (Args a) { return Math_round (a); }
        static var Math_hypot     (Args a) { return std::hypot (getDouble (a, 0), getDouble (a, 1)); }
        static var Math_imul      (Args a) { return getInt (a, 0) * getInt (a, 1); }
        static var Math_log       (Args a) { return std::log   (getDouble (a, 0)); }
        static var Math_log1p     (Args a) { return std::log1p (getDouble (a, 0)); }
        static var Math_log2      (Args a) { return std::log2  (getDouble (a, 0)); }
        static var Math_log10     (Args a) { return std::log10 (getDouble (a, 0)); }
        static var Math_max       (Args a) { return (isInt (a, 0) && isInt (a, 1)) ? var (jmax (getInt (a, 0), getInt (a, 1))) : var (jmax (getDouble (a, 0), getDouble (a, 1))); }
        static var Math_min       (Args a) { return (isInt (a, 0) && isInt (a, 1)) ? var (jmin (getInt (a, 0), getInt (a, 1))) : var (jmin (getDouble (a, 0), getDouble (a, 1))); }
        static var Math_pow       (Args a) { return std::pow   (getDouble (a, 0), getDouble (a, 1)); }
        static var Math_randInt   (Args a) { return a.numArguments < 2 ? var::undefined() : Random::getSystemRandom().nextInt (Range<int> (getInt (a, 0), getInt (a, 1))); }
        static var Math_random    (Args)   { return Random::getSystemRandom().nextDouble(); }
        static var Math_round     (Args a) { return isInt (a, 0) ? var (roundToInt (getInt (a, 0))) : var (roundToInt (getDouble (a, 0))); }
        static var Math_sign      (Args a) { return isInt (a, 0) ? var (sign (getInt (a, 0))) : var (sign (getDouble (a, 0))); }
        static var Math_sin       (Args a) { return std::sin   (getDouble (a, 0)); }
        static var Math_sinh      (Args a) { return std::sinh  (getDouble (a, 0)); }
        static var Math_sqrt      (Args a) { return std::sqrt  (getDouble (a, 0)); }
        static var Math_tan       (Args a) { return std::tan   (getDouble (a, 0)); }
        static var Math_tanh      (Args a) { return std::tanh  (getDouble (a, 0)); }
        static var Math_trunc     (Args a) { return std::trunc (getDouble (a, 0)); }

        //NB: These are non-standard.
        static var Math_range     (Args a) { return isInt (a, 0) ? var (jlimit (getInt (a, 1), getInt (a, 2), getInt (a, 0))) : var (jlimit (getDouble (a, 1), getDouble (a, 2), getDouble (a, 0))); }
        static var Math_sqr       (Args a) { return square           (getDouble (a, 0)); }
        static var Math_toDegrees (Args a) { return radiansToDegrees (getDouble (a, 0)); }
        static var Math_toRadians (Args a) { return degreesToRadians (getDouble (a, 0)); }

        static var Math_clz32 (Args a)
        {
            if (isInt (a, 0))
                return 32 - BigInteger (std::abs (getInt (a, 0))).countNumberOfSetBits();

            return 0;
        }

        // We can't use the std namespace equivalents of these functions without breaking
        // compatibility with older versions of OS X.
        static var Math_asinh     (Args a) { return asinh (getDouble (a, 0)); }
        static var Math_acosh     (Args a) { return acosh (getDouble (a, 0)); }
        static var Math_atanh     (Args a) { return atanh (getDouble (a, 0)); }

        template<typename Type>
        static constexpr Type sign (Type n) noexcept 
        {
            return n > 0 ? (Type) 1 : (n < 0 ? (Type) -1 : 0);
        }
    };

    //==============================================================================
    struct JSONClass final  : public DynamicObject
    {
        JSONClass()
        {
            setMethod ("parse", parse);
            setMethod ("stringify", stringify);
        }

        JUCE_JS_IDENTIFY_CLASS ("JSON")

        static var parse (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var stringify (Args a)   { return JSON::toString (get (a, 0)); }
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Symbol
    */
    struct SymbolClass final  : public DynamicObject
    {
        SymbolClass()
        {
        }

        JUCE_JS_IDENTIFY_CLASS ("SymbolClass")

        static var Symbol_for (Args a)          { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Symbol_keyFor (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO

        //TODO Hm... should be properties and not methods.
        static var asyncIterator (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var hasInstance (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var isConcatSpreadable (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var match (Args a)               { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var matchAll (Args a)            { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var replace (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var search (Args a)              { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var split (Args a)               { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var species (Args a)             { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toPrimitive (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toStringTag (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var unscopables (Args a)         { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/RegExp
    */
    struct RegExpClass final  : public DynamicObject
    {
        RegExpClass()
        {
        }

        JUCE_JS_IDENTIFY_CLASS ("RegExp")

        static var compile (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var exec (Args a)        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var test (Args a)        { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toSource (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var toString (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    /**

        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Guide/Numbers_and_dates
    */
    struct NumberClass final  : public DynamicObject
    {
        NumberClass()
        {
            setMethod ("parseInt",              parseInt);
            setMethod ("parseFloat",            parseFloat);
            setMethod ("isNaN",                 isNaN);
            setMethod ("isFinite",              isFinite);
            setMethod ("isInteger",             [] (Args a) { return isInt (a, 0); });
            setMethod ("isSafeInteger",         [] (Args a) { return ! std::isnan (getDouble (a, 0)); });

            setProperty ("EPSILON",             std::numeric_limits<double>::epsilon());
            setProperty ("NaN",                 std::numeric_limits<double>::quiet_NaN());
            setProperty ("NEGATIVE_INFINITY",   -std::numeric_limits<double>::infinity());
            setProperty ("POSITIVE_INFINITY",   std::numeric_limits<double>::infinity());
            setProperty ("MIN_SAFE_INTEGER",    std::numeric_limits<int64>::min());
            setProperty ("MAX_SAFE_INTEGER",    std::numeric_limits<int64>::max());
            setProperty ("MIN_VALUE",           std::numeric_limits<double>::min());
            setProperty ("MAX_VALUE",           std::numeric_limits<double>::max());
        }

        JUCE_JS_IDENTIFY_CLASS ("Number")

        static var parseInt (Args a)
        {
            auto s = getString (a, 0).trim();

            return s[0] == '0' ? (s[1] == 'x' ? s.substring (2).getHexValue64() : getOctalValue (s))
                               : s.getLargeIntValue();
        }

        static var parseFloat (Args a)      { return getDouble (a, 0); }
        static var isNaN (Args a)           { return std::isnan (getDouble (a, 0)); }
        static var isFinite (Args a)        { return std::isfinite (getDouble (a, 0)); }
    };

    //==============================================================================
    /**

        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Boolean
    */
    struct BooleanClass final  : public DynamicObject
    {
        BooleanClass()
        {
        }

        JUCE_JS_IDENTIFY_CLASS ("Boolean")
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Boolean
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Map
    */
    struct MapClass final  : public DynamicObject
    {
        MapClass()
        {
            #define MAP_CLASS_METHODS(X) \
                X (delete)  X (clear)   X (entries) X (forEach) X (get) \
                X (has)     X (keys)    X (set)     X (values)

            #define CREATE_MAP_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), Map_ ## methodName);

            MAP_CLASS_METHODS (CREATE_MAP_METHOD)

            #undef MAP_CLASS_METHODS
            #undef CREATE_MAP_METHOD
        }

        JUCE_JS_IDENTIFY_CLASS ("Map")

        static var Map_delete (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_clear (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_entries (Args a) { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_forEach (Args a) { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_get (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_has (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_keys (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_set (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Map_values (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO

        //operator[]
        static var Map_operatorBrackets (Args a)
        {
            ignoreUnused (a); jassertfalse; return var();
        }
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/Set
    */
    struct SetClass final  : public DynamicObject
    {
        SetClass()
        {
            #define SET_CLASS_METHODS(X) \
                X (delete) X (add) X (clear) X (entries) X (forEach) X (has) X (values)

            #define CREATE_SET_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), Set_ ## methodName);

            SET_CLASS_METHODS (CREATE_SET_METHOD)

            #undef SET_CLASS_METHODS
            #undef CREATE_SET_METHOD
        }

        JUCE_JS_IDENTIFY_CLASS ("Set")

        static var Set_delete (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Set_add (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Set_clear (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Set_entries (Args a) { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Set_forEach (Args a) { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Set_has (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var Set_values (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO

        //operator[]
        static var Set_operatorBrackets (Args a)
        {
            ignoreUnused (a); jassertfalse; return var();
        }
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/WeakMap
    */
    struct WeakMapClass final  : public DynamicObject
    {
        WeakMapClass()
        {
            #define WEAKMAP_CLASS_METHODS(X) \
                X (delete) X (clear) X (get) X (has) X (set)

            #define CREATE_WEAKMAP_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), WeakMap_ ## methodName);

            WEAKMAP_CLASS_METHODS (CREATE_WEAKMAP_METHOD)

            #undef WEAKMAP_CLASS_METHODS
            #undef CREATE_WEAKMAP_METHOD
        }

        JUCE_JS_IDENTIFY_CLASS ("WeakMap")

        static var WeakMap_delete (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakMap_clear (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakMap_get (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakMap_has (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakMap_set (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/WeakSet
    */
    struct WeakSetClass final  : public DynamicObject
    {
        WeakSetClass()
        {
            #define WEAKSET_CLASS_METHODS(X) \
                X (add) X (clear) X (delete) X (has)

            #define CREATE_WEAKSET_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), WeakSet_ ## methodName);

            WEAKSET_CLASS_METHODS (CREATE_WEAKSET_METHOD)

            #undef WEAKSET_CLASS_METHODS
            #undef CREATE_WEAKSET_METHOD
        }

        JUCE_JS_IDENTIFY_CLASS ("WeakSet")

        static var WeakSet_add (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakSet_clear (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakSet_delete (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var WeakSet_has (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/ArrayBuffer
    */
    struct ArrayBufferClass final  : public DynamicObject
    {
        ArrayBufferClass()
        {
            #define ARRAYBUFFER_CLASS_METHODS(X) \
                X (isView) X (slice) X (transfer)

            #define CREATE_ARRAYBUFFER_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), methodName);

            ARRAYBUFFER_CLASS_METHODS (CREATE_ARRAYBUFFER_METHOD)

            #undef ARRAYBUFFER_CLASS_METHODS
            #undef CREATE_ARRAYBUFFER_METHOD
        }

        JUCE_JS_IDENTIFY_CLASS ("ArrayBuffer")

        static var isView (Args a)      { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var slice (Args a)       { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var transfer (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };

    //==============================================================================
    /**
        https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Global_Objects/DataView
    */
    struct DataViewClass final  : public DynamicObject
    {
        DataViewClass()
        {
            #define DATAVIEW_CLASS_METHODS(X) \
                X (getFloat32)  X (getFloat64)  X (getInt8)     X (getInt16)    X (getInt32) \
                X (getUint8)    X (getUint16)   X (getUint32)   X (setFloat32)  X (setFloat64) \
                X (setInt8)     X (setInt16)    X (setInt32)    X (setUint8)    X (setUint16) \
                X (setUint32)

            #define CREATE_DAVAVIEW_METHOD(methodName) \
                    setMethod (JUCE_STRINGIFY (methodName), methodName);

            DATAVIEW_CLASS_METHODS (CREATE_DAVAVIEW_METHOD)

            #undef DATAVIEW_CLASS_METHODS
            #undef CREATE_DAVAVIEW_METHOD
        }

        JUCE_JS_IDENTIFY_CLASS ("DataView")

        static var getFloat32 (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getFloat64 (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getInt8 (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getInt16 (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getInt32 (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUint8 (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUint16 (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var getUint32 (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setFloat32 (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setFloat64 (Args a)  { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setInt8 (Args a)     { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setInt16 (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setInt32 (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUint8 (Args a)    { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUint16 (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
        static var setUint32 (Args a)   { ignoreUnused (a); jassertfalse; return var(); } //TODO
    };
};

//==============================================================================
JavascriptEngine::JavascriptEngine()  : maximumExecutionTime (15.0), root (new RootObject())
{
}

JavascriptEngine::~JavascriptEngine() {}

void JavascriptEngine::prepareTimeout() const noexcept   { root->timeout = Time::getCurrentTime() + maximumExecutionTime; }
void JavascriptEngine::stop() noexcept                   { root->timeout = {}; }

void JavascriptEngine::registerNativeObject (const Identifier& name, DynamicObject* object)
{
    root->setProperty (name, object);
}

Result JavascriptEngine::execute (const String& code)
{
    try
    {
        prepareTimeout();
        root->execute (code);
    }
    catch (String& error)
    {
        return Result::fail (error);
    }

    return Result::ok();
}

var JavascriptEngine::evaluate (const String& code, Result* result)
{
    try
    {
        prepareTimeout();
        if (result != nullptr)
            *result = Result::ok();

        return root->evaluate (code);
    }
    catch (String& error)
    {
        if (result != nullptr)
            *result = Result::fail (error);
    }

    return var::undefined();
}

var JavascriptEngine::callFunction (const Identifier& function, const var::NativeFunctionArgs& args, Result* result)
{
    auto returnVal = var::undefined();

    try
    {
        prepareTimeout();
        if (result != nullptr)
            *result = Result::ok();

        RootObject::Scope ({}, *root, *root).findAndInvokeMethod (function, args, returnVal);
    }
    catch (String& error)
    {
        if (result != nullptr)
            *result = Result::fail (error);
    }

    return returnVal;
}

var JavascriptEngine::callFunctionObject (DynamicObject* objectScope, const var& functionObject,
                                          const var::NativeFunctionArgs& args, Result* result)
{
    auto returnVal = var::undefined();

    try
    {
        prepareTimeout();
        if (result != nullptr)
            *result = Result::ok();

        RootObject::Scope rootScope ({}, *root, *root);
        RootObject::Scope (&rootScope, *root, DynamicObject::Ptr (objectScope))
            .invokeMethod (functionObject, args, returnVal);
    }
    catch (String& error)
    {
        if (result != nullptr)
            *result = Result::fail (error);
    }

    return returnVal;
}

const NamedValueSet& JavascriptEngine::getRootObjectProperties() const noexcept
{
    return root->getProperties();
}

#if JUCE_MSVC
 #pragma warning (pop)
#endif

} // namespace juce
