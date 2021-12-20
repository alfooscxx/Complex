#pragma once

// * needed structures
#include <vector>
#include <string>
#include <set>
#include <stack>
#include <complex>
#include <numeric>
#include <algorithm>

// * debugging
#include <iostream>

namespace calc
{
    using constTy = std::complex<int>;

    class operationNode;
    class polyNode;

    // main abstract class describing rational complex-valued function
    // cannot be const to be able to modify expression tree
    // always allocate dynamically
    class expressionNode
    {
    public:
        virtual expressionNode *conj() const = 0;

        // double dispatching of binary operations
        virtual expressionNode *add(expressionNode *second) const = 0;
        virtual expressionNode *add(polyNode *secondPoly) const = 0;
        virtual expressionNode *add(operationNode *secondOp) const = 0;

        virtual expressionNode *multiply(constTy coef) const = 0;
        expressionNode *negate() const { return multiply(-1); }
        expressionNode *substract(expressionNode *second) const
        {
            return add(second->negate());
        }

        virtual expressionNode *multiply(expressionNode *second) const = 0;
        virtual expressionNode *multiply(polyNode *secondPoly) const = 0;
        virtual expressionNode *multiply(operationNode *secondOp) const = 0;

        virtual expressionNode *divide(expressionNode *second, const bool isDivident = true) const = 0;
        virtual expressionNode *divide(polyNode *secondPoly, const bool isDivident = true) const = 0;
        virtual expressionNode *divide(operationNode *secondOp, const bool isDivident = true) const = 0;

        // double dispatching of bracket expansion
        virtual expressionNode *expandAddition(expressionNode *second) const = 0;
        virtual expressionNode *expandAddition(polyNode *secondPoly) const = 0;
        virtual expressionNode *expandAddition(operationNode *secondOp) const = 0;
        virtual expressionNode *expandMultiplication(expressionNode *second) const = 0;
        virtual expressionNode *expandMultiplication(polyNode *secondPoly) const = 0;
        virtual expressionNode *expandMultiplication(operationNode *secondOp) const = 0;

        virtual expressionNode *expand() = 0;

        // printing data, utility predicates
        virtual const bool zeroEqualityCheck() const = 0; // TODO: add optional printing // TODO: make pure virtual

        virtual void print() const = 0;
        virtual const bool requiresBracketsPrinting() const = 0;
    };

    class operationNode : public expressionNode // a sum, product, or fraction of two expressions
    {
    public:
        enum class operationType
        {
            ADDITION,
            MULTIPLICATION,
            DIVISION
        };

    private:
        operationType operation;
        expressionNode *left;
        expressionNode *right;

    public:
        operationNode(expressionNode *_left, expressionNode *_second, operationType _operation)
            : left(_left), right(_second), operation(_operation)
        {
        }

        operationNode *conj() const;

        virtual expressionNode *add(expressionNode *second) const override;
        virtual expressionNode *add(polyNode *second) const override;
        virtual expressionNode *add(operationNode *second) const override;

        virtual expressionNode *multiply(constTy coef) const;

        virtual expressionNode *multiply(expressionNode *second) const
        {
            return second->multiply(const_cast<operationNode *>(this));
        }
        virtual expressionNode *multiply(polyNode *secondPoly) const;
        virtual expressionNode *multiply(operationNode *secondOp) const;

        virtual expressionNode *divide(expressionNode *second, const bool isDivident = true) const
        {
            return second->divide(const_cast<operationNode *>(this), !isDivident);
        };
        virtual expressionNode *divide(polyNode *secondPoly, const bool isDivident = true) const;
        virtual expressionNode *divide(operationNode *secondOp, const bool isDivident = true) const;

        virtual expressionNode *expandAddition(expressionNode *second) const
        {
            return second->expandAddition(const_cast<operationNode *>(this));
        }
        virtual expressionNode *expandAddition(polyNode *secondPoly) const;
        virtual expressionNode *expandAddition(operationNode *secondOp) const;

        virtual expressionNode *expandMultiplication(expressionNode *second) const
        {
            return second->expandMultiplication(const_cast<operationNode *>(this));
        }
        virtual expressionNode *expandMultiplication(polyNode *secondPoly) const;
        virtual expressionNode *expandMultiplication(operationNode *secondOp) const;

        virtual expressionNode *expand() override;

        virtual const bool zeroEqualityCheck() const { return false; } // FIXME

        virtual void print() const override;
        virtual const bool requiresBracketsPrinting() const;
    };

    class term
    {
    public:
        term() {}
        term(const std::string _name)
            : name(_name) {}

        bool hasConjugationMark = false;
        void invertMark() { hasConjugationMark = !hasConjugationMark; }
        std::string name;
        const bool operator<(const term &other) const;
        const bool operator==(const term &other) const;

        expressionNode *conj() const;
    };

    class basicTerm : public term
    {
    public:
        expressionNode *conj() const;

    private:
        struct termProperties
        {
            bool isReal;
            bool isUnit;
        };
        const termProperties props = {false, false};
    };

    class quasiTerm : public term
    {
    public:
        expressionNode *conj() const;
        quasiTerm(const std::string _name, expressionNode *_hiddenExpression)
            : term(_name), hiddenExpression(_hiddenExpression)
        {
        }

    private:
        expressionNode *hiddenExpression;
    };

    struct monomial
    {
        mutable constTy coef = 1; // changing value of coef doesn't invalidate the order of monomials
        void resetCoef(constTy _coef) const
        {
            coef = _coef;
        }
        std::multiset<term> product = std::multiset<term>();
        monomial() {}
        monomial(constTy _coef, const term &_term)
            : coef(_coef)
        {
            product.insert(_term);
        }
        monomial(const constTy _coef, std::multiset<term> _product)
            : coef(_coef), product(_product)
        {
        }

        bool operator<(const monomial &other) const { return product < other.product; }
        bool operator==(const monomial &other) const { return product == other.product; }

        expressionNode *conj() const;

        monomial operator*(const monomial &other) const;
        void operator*=(const constTy k) const;
    };

    class polyNode : public expressionNode
    {
    public:
        expressionNode *conj() const override;

        polyNode() {}
        polyNode(const monomial &mono)
        {
            sum.insert(mono);
        }

        void operator+=(const monomial &mono);
        polyNode operator+(const polyNode &other) const;
        polyNode operator*(const monomial &mono) const;
        polyNode operator*(const polyNode &other) const;

        polyNode operator*(const constTy k) const;

        polyNode operator-(const polyNode &other) const;

        virtual expressionNode *add(expressionNode *second) const { return second->add(const_cast<polyNode *>(this)); }
        virtual expressionNode *add(polyNode *secondPoly) const { return new polyNode(*this + *secondPoly); }
        virtual expressionNode *add(operationNode *secondOp) const
        {
            return secondOp->add(const_cast<polyNode *>(this));
        }

        virtual expressionNode *multiply(constTy coef) const { return new polyNode(*this * coef); }

        virtual expressionNode *multiply(expressionNode *second) const
        {
            return second->multiply(const_cast<polyNode *>(this));
        }
        virtual expressionNode *multiply(polyNode *secondPoly) const
        {
            return new polyNode((*this) * (*secondPoly));
        }
        virtual expressionNode *multiply(operationNode *secondOp) const
        {
            return secondOp->multiply(const_cast<polyNode *>(this));
        }

        virtual expressionNode *divide(expressionNode *second, const bool isDivident = true) const
        {
            return second->divide(const_cast<polyNode *>(this), !isDivident);
        }
        virtual expressionNode *divide(polyNode *secondPoly, const bool isDivident = true) const;
        virtual expressionNode *divide(operationNode *secondOp, const bool isDivident = true) const;

        virtual expressionNode *expandAddition(expressionNode *second) const
        {
            return second->expandAddition(const_cast<polyNode *>(this));
        }
        virtual expressionNode *expandAddition(polyNode *secondPoly) const
        {
            return add(secondPoly);
        }
        virtual expressionNode *expandAddition(operationNode *secondOp) const
        {
            return secondOp->expandAddition(const_cast<polyNode *>(this));
        }

        virtual expressionNode *expandMultiplication(expressionNode *second) const
        {
            return second->expandMultiplication(const_cast<polyNode *>(this));
        }
        virtual expressionNode *expandMultiplication(polyNode *secondPoly) const
        {
            return multiply(secondPoly);
        }
        virtual expressionNode *expandMultiplication(operationNode *secondOp) const
        {
            return secondOp->expandMultiplication(const_cast<polyNode *>(this));
        }

        virtual expressionNode *expand() { return const_cast<polyNode *>(this); }

        virtual void print() const;

    private:
        std::set<monomial> sum = std::set<monomial>();

    public:
        virtual const bool requiresBracketsPrinting() const { return sum.size() > 1; }
        const bool zeroEqualityCheck() const { return sum.empty(); }
    };

    polyNode *make_term(std::string name);
    polyNode *make_scalar(constTy scalar);
}