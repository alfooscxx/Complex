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
    if (secondPoly->checkZeroEquality())
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
    if (secondPoly->checkZeroEquality())
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
        if (secondPoly->checkZeroEquality())
            return new polyNode();
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
        return new polyNode(monomial(1, new basicTerm(*this)));
    if (props.isUnit)
        return make_scalar(1)->divide(new polyNode(monomial(1, new basicTerm(*this))));
    basicTerm *conjugated = new basicTerm(*this);
    conjugated->invertMark();
    return new polyNode(monomial(1, conjugated));
}

expressionNode *quasiTerm::conj() const
{
    return new polyNode(monomial(1, new quasiTerm(name, hiddenExpression->conj())));
}

expressionNode *monomial::conj() const
{
    expressionNode *result = make_scalar(std::conj(coef));
    for (auto const &[name, degree] : product)
    {
        expressionNode *conj_term = degree.first->conj();
        for (int i = 0; i < degree.second; ++i)
            result = result->multiply(conj_term);
    }
    return result;
}

const bool monomial::operator<(const monomial &other) const
{
    auto jt = other.product.begin();
    for (auto it = product.begin(); it != product.end(); ++it)
    {
        if (jt == other.product.end())
            return false;
        if (it->first != jt->first)
            return it->first < jt->first;
        if (it->second.second < jt->second.second)
            return (it == std::prev(product.end()));
        if (jt->second.second < it->second.second)
            return (jt != std::prev(other.product.end()));
        ++jt;
    }
    if (jt != other.product.end())
        return true;
    return false;
}

const bool monomial::operator==(const monomial &other) const
{
    return std::equal(product.begin(), product.end(), other.product.begin(), other.product.end(),
                      [](const auto &a, const auto &b)
                      { return std::make_pair(a.first, a.second.second) == std::make_pair(b.first, b.second.second); });
}

monomial monomial::operator*(const monomial &other) const
{
    monomial result = *this;
    result.coef = this->coef * other.coef;
    for (const auto &[name, degree] : other.product)
    {
        auto it = result.product.find(name);
        if (it != result.product.end())
            it->second.second += degree.second;
        else
            result.product.insert({name, {degree.first, degree.second}});
    }
    return result;
}

void monomial::operator*=(const constTy k) const
{
    resetCoef(coef * k);
}

const bool monomial::dividedBy(const monomial &divider) const
{
    for (auto const &[name, degree] : divider.product)
    {
        auto it = product.find(name);
        if (it == product.end() || it->second.second < degree.second)
            return false;
    }
    return true;
}

monomial monomial::divide(const monomial &divider) const
{
    if (divider.product.empty())
        return *this;
    monomial result = *this;
    for (auto const &[name, degree] : divider.product)
    {
        auto it = result.product.find(name);
        it->second.second -= degree.second;
        if (it->second.second == 0)
            result.product.erase(it);
    }
    return result;
}

expressionNode *polyNode::conj() const
{
    expressionNode *result = new polyNode();
    for (monomial mono : sum)
        result = result->add(mono.conj());
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
        result += mono * other;
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
    if (isDivident)
    {
        if (sum.empty())
            return new polyNode();
        if (secondPoly->sum.size() == 1)
        {
            monomial divider = *secondPoly->sum.begin();
            if (!divider.product.empty() && dividedBy(divider))
                return divide(divider);
            if (divider.product.empty() && (divider.coef == 1))
                return const_cast<polyNode *>(this);
            if (divider.product.empty() && (divider.coef == -1))
                return negate();
        }
        if (sum.size() == 1)
        {
            monomial divider = *sum.begin();
            if (divider.product.size() && secondPoly->dividedBy(divider))
                return make_scalar(1)->divide(secondPoly->divide(divider));
        }
        return new operationNode(const_cast<polyNode *>(this), secondPoly, operationNode::operationType::DIVISION);
    }
    else
        return secondPoly->divide(const_cast<polyNode *>(this));
}

expressionNode *polyNode::divide(operationNode *secondOp, const bool isDivident) const
{
    return secondOp->divide(const_cast<polyNode *>(this), !isDivident);
}

const bool polyNode::dividedBy(const monomial &divider) const
{
    for (auto mono : sum)
    {
        if (!mono.dividedBy(divider))
            return false;
    }
    return true;
}

expressionNode *polyNode::divide(const monomial &divider) const
{
    expressionNode *result = new polyNode();
    for (auto mono : sum)
        result = result->add(new polyNode(mono.divide(divider)));
    if (divider.coef != 1 && divider.coef != -1)
        return result->divide(make_scalar(divider.coef));
    else if (divider.coef == -1)
        return result->negate();
    else
        return result;
}

polyNode *calc::make_term(std::string name)
{
    return new polyNode(monomial(1, new basicTerm(name)));
}

polyNode *calc::make_unit_term(std::string name)
{
    return new polyNode(monomial(1, new basicTerm(name, {false, true})));
}

polyNode *calc::make_real_term(std::string name)
{
    return new polyNode(monomial(1, new basicTerm(name, {true, false})));
}

polyNode *calc::make_scalar(constTy scalar)
{
    if (scalar == 0)
        return new polyNode();
    return new polyNode(monomial(scalar, std::map<std::string, std::pair<term *, int>>()));
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
            {
                if (it != sum.begin())
                    std::cout << " ";
                std::cout << "- ";
            }
            if ((mono.product.empty()) || (coef.real() != 1) && (coef.real() != -1))
                std::cout << std::abs(coef.real());
        }
        for (auto [name, degree] : mono.product)
            std::cout << name << (degree.second > 1 ? "^" + std::to_string(degree.second) : "");
    }
}

expressionNode *parseString(const std::string &line, size_t l, size_t r)
{
    while (l < r && line[l] == ' ')
        ++l;
    while (r > l && line[r - 1] == ' ')
        --r;
    if (r <= l)
        return make_scalar(1);
    unsigned counter = 0;
    size_t curr = l;
    size_t m, d, s;
    m = r;
    d = r;
    s = r;
    while (curr != r)
    {
        if (line[curr] == '(')
        {
            ++counter;
            ++curr;
            continue;
        }
        if (line[curr] == ')')
        {
            --counter;
            ++curr;
            continue;
        }
        if (counter == 0)
        {
            if (line[curr] == '+')
                return parseString(line, l, curr)->add(parseString(line, curr + 1, r));
            if (line[curr] == '-')
                s = curr;
            if (line[curr] == '*')
            {
                m = curr++;
                continue;
            }
            if (line[curr] == '/')
            {
                d = curr++;
                continue;
            }
        }
        ++curr;
    }
    if (s != r && s != l)
        return parseString(line, l, s)->substract(parseString(line, s + 1, r));
    if (m != r)
        return parseString(line, l, m)->multiply(parseString(line, m + 1, r));
    if (d != r)
        return parseString(line, l, d)->divide(parseString(line, d + 1, r));
    if (line[l] == '(')
    {
        counter = 1;
        size_t closing = l + 1;
        while (closing < r)
        {
            if (line[closing] == '(')
                ++counter;
            if (line[closing] == ')')
                --counter;
            if (counter == 0)
                return parseString(line, l + 1, closing)->multiply(parseString(line, closing + 1, r));
            ++closing;
        }
    }
    if (l == r)
        return make_scalar(1);
    if (line[l] == '-')
        return parseString(line, l + 1, r)->negate();
    if (line[l] >= 48 && line[l] <= 57)
    {
        size_t numEnd = l + 1;
        while (numEnd < r && (line[numEnd] >= 48) && (line[numEnd] <= 57))
            ++numEnd;
        return make_scalar(std::stoi(line.substr(l, numEnd - l)))->multiply(parseString(line, numEnd, r));
    }
    return make_unit_term(line.substr(l, 1))->multiply(parseString(line, l + 1, r));
}