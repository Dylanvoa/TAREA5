#pragma once
#include <cmath>
#include <memory>
#include <sstream>
#include <string>

class Expression;
using ExprPtr = std::shared_ptr<Expression>;

// ---------- Componente ----------
class Expression {
public:
    virtual ~Expression() = default;
    virtual double evaluate(double x) const = 0;
    virtual ExprPtr derivative() const = 0;
    virtual std::string toString() const = 0;
    // Evita dynamic_cast: permite saber si la expresion es un valor constante.
    virtual bool isConstant() const { return false; }
};

// Fabricas con simplificacion (declaradas aqui, definidas al final)
ExprPtr makeConst(double v);
ExprPtr makeAdd(ExprPtr a, ExprPtr b);
ExprPtr makeSub(ExprPtr a, ExprPtr b);
ExprPtr makeMul(ExprPtr a, ExprPtr b);
ExprPtr makeDiv(ExprPtr a, ExprPtr b);
ExprPtr makePow(ExprPtr a, ExprPtr b);
ExprPtr makeSin(ExprPtr a);
ExprPtr makeCos(ExprPtr a);
ExprPtr makeLn(ExprPtr a);

// ---------- Hojas ----------
class Constant : public Expression {
public:
    explicit Constant(double v) : value_(v) {}
    double evaluate(double) const override { return value_; }
    ExprPtr derivative() const override { return makeConst(0); }
    std::string toString() const override {
        std::ostringstream os;
        os << value_;
        return os.str();
    }
    bool isConstant() const override { return true; }
private:
    double value_;
};

class Variable : public Expression {
public:
    double evaluate(double x) const override { return x; }
    ExprPtr derivative() const override { return makeConst(1); }
    std::string toString() const override { return "x"; }
};

// ---------- Compuestos: operaciones binarias ----------
class BinaryOperation : public Expression {
public:
    BinaryOperation(ExprPtr l, ExprPtr r) : left_(std::move(l)), right_(std::move(r)) {}
    std::string toString() const override {
        return "(" + left_->toString() + " " + symbol() + " " + right_->toString() + ")";
    }
protected:
    virtual std::string symbol() const = 0;
    ExprPtr left_, right_;
};

class Add : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) const override { return left_->evaluate(x) + right_->evaluate(x); }
    ExprPtr derivative() const override { return makeAdd(left_->derivative(), right_->derivative()); }
protected:
    std::string symbol() const override { return "+"; }
};

class Subtract : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) const override { return left_->evaluate(x) - right_->evaluate(x); }
    ExprPtr derivative() const override { return makeSub(left_->derivative(), right_->derivative()); }
protected:
    std::string symbol() const override { return "-"; }
};

// Regla del producto: (uv)' = u'v + uv'
class Multiply : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) const override { return left_->evaluate(x) * right_->evaluate(x); }
    ExprPtr derivative() const override {
        return makeAdd(makeMul(left_->derivative(), right_),
                       makeMul(left_, right_->derivative()));
    }
protected:
    std::string symbol() const override { return "*"; }
};

// Regla del cociente: (u/v)' = (u'v - uv') / v^2
class Divide : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) const override { return left_->evaluate(x) / right_->evaluate(x); }
    ExprPtr derivative() const override {
        return makeDiv(makeSub(makeMul(left_->derivative(), right_),
                               makeMul(left_, right_->derivative())),
                       makePow(right_, makeConst(2)));
    }
protected:
    std::string symbol() const override { return "/"; }
};

// Potencia:
//  - exponente constante n:  (u^n)' = n * u^(n-1) * u'
//  - caso general:           (u^v)' = u^v * (v' ln u + v u'/u)
class Power : public BinaryOperation {
public:
    using BinaryOperation::BinaryOperation;
    double evaluate(double x) const override { return std::pow(left_->evaluate(x), right_->evaluate(x)); }
    ExprPtr derivative() const override {
        if (right_->isConstant()) {
            double n = right_->evaluate(0);
            return makeMul(makeMul(makeConst(n), makePow(left_, makeConst(n - 1))),
                           left_->derivative());
        }
        return makeMul(makePow(left_, right_),
                       makeAdd(makeMul(right_->derivative(), makeLn(left_)),
                               makeDiv(makeMul(right_, left_->derivative()), left_)));
    }
protected:
    std::string symbol() const override { return "^"; }
};

// ---------- Compuestos: funciones unarias ----------
class UnaryFunction : public Expression {
public:
    explicit UnaryFunction(ExprPtr arg) : arg_(std::move(arg)) {}
    std::string toString() const override { return name() + "(" + arg_->toString() + ")"; }
protected:
    virtual std::string name() const = 0;
    ExprPtr arg_;
};

// Regla de la cadena en todas: (f(u))' = f'(u) * u'
class Sin : public UnaryFunction {
public:
    using UnaryFunction::UnaryFunction;
    double evaluate(double x) const override { return std::sin(arg_->evaluate(x)); }
    ExprPtr derivative() const override { return makeMul(makeCos(arg_), arg_->derivative()); }
protected:
    std::string name() const override { return "sin"; }
};

class Cos : public UnaryFunction {
public:
    using UnaryFunction::UnaryFunction;
    double evaluate(double x) const override { return std::cos(arg_->evaluate(x)); }
    ExprPtr derivative() const override {
        return makeMul(makeMul(makeConst(-1), makeSin(arg_)), arg_->derivative());
    }
protected:
    std::string name() const override { return "cos"; }
};

class Exp : public UnaryFunction {
public:
    using UnaryFunction::UnaryFunction;
    double evaluate(double x) const override { return std::exp(arg_->evaluate(x)); }
    ExprPtr derivative() const override {
        return makeMul(std::make_shared<Exp>(arg_), arg_->derivative());
    }
protected:
    std::string name() const override { return "exp"; }
};

class Ln : public UnaryFunction {
public:
    using UnaryFunction::UnaryFunction;
    double evaluate(double x) const override { return std::log(arg_->evaluate(x)); }
    ExprPtr derivative() const override { return makeDiv(arg_->derivative(), arg_); }
protected:
    std::string name() const override { return "ln"; }
};

// ---------- Fabricas con simplificacion algebraica ----------
inline bool isZero(const ExprPtr& e) { return e->isConstant() && e->evaluate(0) == 0; }
inline bool isOne(const ExprPtr& e)  { return e->isConstant() && e->evaluate(0) == 1; }

inline ExprPtr makeConst(double v) { return std::make_shared<Constant>(v); }

inline ExprPtr makeAdd(ExprPtr a, ExprPtr b) {
    if (isZero(a)) return b;
    if (isZero(b)) return a;
    if (a->isConstant() && b->isConstant()) return makeConst(a->evaluate(0) + b->evaluate(0));
    return std::make_shared<Add>(a, b);
}
inline ExprPtr makeSub(ExprPtr a, ExprPtr b) {
    if (isZero(b)) return a;
    if (isZero(a)) return makeMul(makeConst(-1), b);
    if (a->isConstant() && b->isConstant()) return makeConst(a->evaluate(0) - b->evaluate(0));
    return std::make_shared<Subtract>(a, b);
}
inline ExprPtr makeMul(ExprPtr a, ExprPtr b) {
    if (isZero(a) || isZero(b)) return makeConst(0);
    if (isOne(a)) return b;
    if (isOne(b)) return a;
    if (a->isConstant() && b->isConstant()) return makeConst(a->evaluate(0) * b->evaluate(0));
    return std::make_shared<Multiply>(a, b);
}
inline ExprPtr makeDiv(ExprPtr a, ExprPtr b) {
    if (isZero(a)) return makeConst(0);
    if (isOne(b)) return a;
    return std::make_shared<Divide>(a, b);
}
inline ExprPtr makePow(ExprPtr a, ExprPtr b) {
    if (isOne(b)) return a;
    if (isZero(b)) return makeConst(1);
    return std::make_shared<Power>(a, b);
}
inline ExprPtr makeSin(ExprPtr a) { return std::make_shared<Sin>(a); }
inline ExprPtr makeCos(ExprPtr a) { return std::make_shared<Cos>(a); }
inline ExprPtr makeLn(ExprPtr a)  { return std::make_shared<Ln>(a); }
