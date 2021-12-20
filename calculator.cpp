#include "calculator.h"

using namespace calc;

operationNode *operationNode::conj() const
{
    return new operationNode(left->conj(), right->conj(), operation);
}

expressionNode *operationNode::add(expressionNode *second) const
{
    return second->add(const_cast<operationNode *>(this));
}

expressionNode *operationNode::add(polyNode *secondPoly) const
{
    if (secondPoly->zeroEqualityCheck())
        return const_cast<operationNode *>(this);
    if (operation != operationType::ADDITION)
        return new operationNode(secondPoly, const_cast<operationNode *>(this), operationType::ADDITION);
    return (secondPoly->add(left))->add(right);
}

expressionNode *operationNode::add(operationNode *secondOp) const
{
    if (operation == operationType::ADDITION)
    {
        if (secondOp->operation == operationType::ADDITION)
        {
            return (add(secondOp->left))->add(secondOp->right);
        }
        return new operationNode(const_cast<operationNode *>(this), secondOp, operationType::ADDITION);
    }
    if (secondOp->operation == operationType::ADDITION)
    {
        return new operationNode(secondOp, const_cast<operationNode *>(this), operationType::ADDITION);
    }
    return new operationNode(const_cast<operationNode *>(this), secondOp, operationType::ADDITION);
}

expressionNode *operationNode::multiply(constTy coef) const
{
    if (operation == operationType::ADDITION)
        return (left->multiply(coef))->add(right->multiply(coef));
    if (operation == operationType::MULTIPLICATION)
        return (left->multiply(coef))->multiply(right);
    if (operation == operationType::DIVISION)
        return (left->multiply(coef))->divide(right);
}

expressionNode *operationNode::multiply(polyNode *secondPoly) const
{
    if (secondPoly->zeroEqualityCheck())
        return new polyNode();
    if (operation == operationType::MULTIPLICATION)
        return (secondPoly->multiply(left))->multiply(right);
    if (operation == operationType::DIVISION)
        return (secondPoly->multiply(left))->divide(right);
    return new operationNode(secondPoly, const_cast<operationNode *>(this), operationType::MULTIPLICATION);
}

expressionNode *operationNode::multiply(operationNode *secondOp) const
{
    if (operation == operationType::DIVISION)
    {
        if (secondOp->operation == operationType::DIVISION)
        {
            expressionNode *nominator = left->multiply(secondOp->left);
            expressionNode *denominator = right->multiply(secondOp->right);
            return nominator->divide(denominator);
        }
        expressionNode *nominator = left->multiply(secondOp);
        return nominator->divide(right);
    }
    if (secondOp->operation == operationType::DIVISION)
    {
        expressionNode *nominator = multiply(secondOp->left);
        return nominator->divide(secondOp->right);
    }
    if (operation == operationType::MULTIPLICATION)
    {
        if (secondOp->operation == operationType::MULTIPLICATION)
        {
            return (multiply(secondOp->left))->multiply(secondOp->right);
        }
        return new operationNode(const_cast<operationNode *>(this), secondOp, operationType::MULTIPLICATION);
    }
    if (secondOp->operation == operationType::MULTIPLICATION)
    {
        return new operationNode(secondOp, const_cast<operationNode *>(this), operationType::MULTIPLICATION);
    }
    return new operationNode(const_cast<operationNode *>(this), secondOp, operationType::MULTIPLICATION);
}

expressionNode *operationNode::divide(polyNode *secondPoly, const bool isDivident) const
{
    if (isDivident)
    {
        if (operation == operationType::DIVISION)
        {
            expressionNode *denominator = right->multiply(secondPoly);
            return left->divide(denominator);
        }
        return new operationNode(const_cast<operationNode *>(this), secondPoly, operationType::DIVISION);
    }
    else
    {
        if (operation == operationType::DIVISION)
        {
            expressionNode *nominator = secondPoly->multiply(right);
            return nominator->divide(left);
        }
        else
        {
            return new operationNode(secondPoly, const_cast<operationNode *>(this), operationType::DIVISION);
        }
    }
}

expressionNode *operationNode::divide(operationNode *secondOp, const bool isDivident) const
{
    if (isDivident)
    {
        if (operation == operationType::DIVISION)
        {
            if (secondOp->operation == operationType::DIVISION)
            {
                expressionNode *nominator = left->multiply(secondOp->right);
                expressionNode *denominator = right->multiply(secondOp->left);
                return nominator->divide(denominator);
            }
            expressionNode *denominator = right->multiply(secondOp);
            return left->divide(denominator);
        }
        if (secondOp->operation == operationType::DIVISION)
        {
            expressionNode *nominator = multiply(secondOp->right);
            return nominator->divide(secondOp->left);
        }
        return new operationNode(const_cast<operationNode *>(this), secondOp, operationType::DIVISION);
    }
    else
    {
        return secondOp->divide(const_cast<operationNode *>(this));
    }
}

expressionNode *operationNode::expandAddition(polyNode *secondPoly) const
{
    if (operation != operationType::DIVISION)
        return add(secondPoly);
    return (left->add(right->multiply(secondPoly)))->divide(right);
}

expressionNode *operationNode::expandAddition(operationNode *secondOp) const
{
    if (operation != operationType::DIVISION)
    {
        if (secondOp->operation != operationType::DIVISION)
            return add(secondOp);
        return secondOp->expandAddition(const_cast<operationNode *>(this));
    }
    if (secondOp->operation != operationType::DIVISION)
        return (left->add(right->multiply(secondOp)))->divide(right);
    expressionNode *nominator = (left->multiply(secondOp->right))->add(right->multiply(secondOp->left));
    return nominator->divide(right->multiply(secondOp->right));
}

expressionNode *operationNode::expandMultiplication(polyNode *secondPoly) const
{
    if (operation != operationType::ADDITION)
    {
        return multiply(secondPoly);
    }
    return (left->multiply(secondPoly))->add(right->multiply(secondPoly));
}

expressionNode *operationNode::expandMultiplication(operationNode *secondOp) const
{
    if (operation != operationType::ADDITION)
    {
        return multiply(secondOp);
    }
    return (left->multiply(secondOp))->add(right->multiply(secondOp));
}

expressionNode *operationNode::expand()
{
    left = left->expand();
    right = right->expand();
    if (operation == operationType::ADDITION)
    {
        return left->expandAddition(right);
    }
    if (operation == operationType::MULTIPLICATION)
    {
        return left->expandMultiplication(right);
    }
    if (operation == operationType::DIVISION)
    {
        return left->divide(right);
    }
}

const bool operationNode::requiresBracketsPrinting() const
{
    return !(operation == operationType::MULTIPLICATION);
}

const bool term::operator<(const term &other) const
{
    if (name != other.name)
        return name < other.name;
    if (hasConjugationMark != other.hasConjugationMark)
        return !hasConjugationMark;
    return false;
}

const bool term::operator==(const term &other) const
{
    return (name == other.name) && (hasConjugationMark == other.hasConjugationMark);
}

expressionNode *basicTerm::conj() const
{
    if (props.isReal)
    {
        return new polyNode(monomial(1, *this));
    }
    if (props.isUnit)
    {
        return make_scalar(1)->divide(new polyNode(monomial(1, *this)));
    }
    basicTerm conjugated(*this);
    conjugated.invertMark();
    return new polyNode(monomial(1, conjugated));
}

expressionNode *quasiTerm::conj() const
{
    return new polyNode(monomial(1, quasiTerm(name, hiddenExpression->conj())));
}

expressionNode *monomial::conj() const
{
    expressionNode *result = make_scalar(coef);
    for (term _term : product)
        result->multiply(_term.conj());
    return result;
}

monomial monomial::operator*(const monomial &other) const
{
    monomial result = *this;
    result.coef = this->coef * other.coef;
    for (const auto &multiply : other.product)
        result.product.insert(multiply);
    return result;
}

void monomial::operator*=(const constTy k) const
{
    resetCoef(coef * k);
}

expressionNode *polyNode::conj() const
{
    expressionNode *result = new polyNode();
    for (monomial mono : sum)
        result->add(mono.conj());
    return result;
}

void polyNode::operator+=(const monomial &mono)
{
    auto it = sum.find(mono);
    if (it != sum.end())
    {
        if (it->coef + mono.coef != 0)
            it->resetCoef(it->coef + mono.coef);
        else
            sum.erase(it);
    }
    else
        sum.insert(mono);
    return;
}

polyNode polyNode::operator+(const polyNode &other) const
{
    polyNode result = *this;
    for (const auto &mono : other.sum)
        result += (mono);
    return result;
}

polyNode polyNode::operator*(const monomial &other) const
{
    polyNode result;
    for (const auto &mono : sum)
    {
        result += mono * other;
    }
    return result;
}

polyNode polyNode::operator*(const polyNode &other) const
{
    polyNode result;
    for (const auto &mono : other.sum)
        result = result + (*this * mono);
    return result;
}

polyNode polyNode::operator*(const constTy k) const
{
    polyNode result(*this);
    for (auto &mono : result.sum)
        mono *= k;
    return result;
}

polyNode polyNode::operator-(const polyNode &other) const
{
    return (*this) + other * (-1);
}

expressionNode *polyNode::divide(polyNode *secondPoly, const bool isDivident) const
{
    // TODO: gcd cancelling (unreal) (maybe very slow)
    if (isDivident)
    {
        if (zeroEqualityCheck())
            return new polyNode();
        return new operationNode(const_cast<polyNode *>(this), secondPoly, operationNode::operationType::DIVISION);
    }
    else
    {
        return secondPoly->divide(const_cast<polyNode *>(this));
    }
}

expressionNode *polyNode::divide(operationNode *secondOp, const bool isDivident) const
{
    return secondOp->divide(const_cast<polyNode *>(this), !isDivident);
}

polyNode *calc::make_term(std::string name)
{
    return new polyNode(monomial(1, term(name)));
}

polyNode *calc::make_scalar(constTy scalar)
{
    if (scalar == 0)
        return new polyNode();
    return new polyNode(monomial(scalar, std::multiset<term>()));
}

void operationNode::print() const
{
    std::string opChar;
    switch (operation)
    {
    case (operationType::ADDITION):
        opChar = " + ";
        break;
    case (operationType::MULTIPLICATION):
        opChar = "";
        break;
    case (operationType::DIVISION):
        opChar = " / ";
        break;
    }
    const bool printLeftBrackets = left->requiresBracketsPrinting();
    if (opChar != " + " && printLeftBrackets)
        std::cout << "(";
    left->print();
    if (opChar != " + " && printLeftBrackets)
        std::cout << ")";
    std::cout << opChar;
    const bool printRightBrackets = right->requiresBracketsPrinting();
    if (opChar != " + " && printRightBrackets)
        std::cout << "(";
    right->print();
    if (opChar != " + " && printRightBrackets)
        std::cout << ")";
}

void polyNode::print() const
{
    if (sum.empty())
    {
        std::cout << 0;
        return;
    }
    for (auto it = sum.begin(); it != sum.end(); ++it)
    {
        auto mono = *it;
        constTy coef = mono.coef;
        if (coef.imag() != 0)
        {
            std::cout << "(";
            std::cout << coef.real();
            std::cout << " + ";
            std::cout << coef.imag();
            std::cout << "i)";
        }
        else
        {
            if (coef.real() > 0)
            {
                if (it != sum.begin())
                    std::cout << " + ";
            }
            else
                std::cout << " - ";
            if ((mono.product.empty()) || (coef.real() != 1) && (coef.real() != -1))
                std::cout << std::abs(coef.real());
        }
        for (auto it = mono.product.begin(); it != mono.product.end();)
        {
            term current = *it;
            int deg = mono.product.count(current);
            std::cout << current.name;
            if (current.hasConjugationMark)
                std::cout << '~';
            if (deg != 1)
            {
                std::cout << "^";
                std::cout << deg;
            }
            std::advance(it, deg);
        }
    }
}

int main()
{
    polyNode *a = make_term("a");
    polyNode *b = make_term("b");
    polyNode *c = make_term("c");
    polyNode *two = make_scalar(2);
    expressionNode *result = two->multiply(a)->multiply(b)->divide(a->add(b));
    result->print();
    std::getchar();
    return 0;
}