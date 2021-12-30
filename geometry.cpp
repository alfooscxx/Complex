#include "calculator.h"

using namespace calc;

// *

using expr = expressionNode * ;

expr barycenter(expr A, expr B, expr C)
{
	return (A->add(B)->add(C))->divide(make_scalar(3));
}

expr sum(expr A, expr B, expr C)
{
	return (A->add(B)->add(C));
};

expr middlepoint(expr A, expr B)
{
	return (A->add(B))->divide(make_scalar(2));
};

expr linear_comb(expr A, expr kA, expr B, expr kB)
{
	return ((A->multiply(kA))->add(B->multiply(kB)));
};

// param A - center
expr symm(expr A, expr B)
{
	return linear_comb(A, make_scalar(2), B, make_scalar(-1));
};

expr rotate90(expr A, expr B)
{
	return linear_comb(A, make_scalar(constTy(1, -1)), B, make_scalar(constTy(0, 1)));
};

expr rot_homothety(expr A, expr B, expr coef)
{
	return linear_comb(A, make_scalar(1)->substract(coef), B, coef);
}

/*
X       Y
conj(X) conj(Y)
*/
expr det(expr X, expr Y)
{
	return (X->multiply(Y->conj()))->substract(Y->multiply(X->conj()));
};

bool collinear(expr A, expr B, expr C)
{
	expr detABC = det(A, B)->add(det(B, C))->add(det(C, A));
	detABC->print();
	std::cout << std::endl;
	return detABC->expand()->checkZeroEquality();
};

struct line
{
	expr A, B, C;
};

line chord(expr X, expr Y)
{
	return { make_scalar(1), X->multiply(Y), X->add(Y) };
}

line tangent(expr X)
{
	return { make_scalar(1), X->multiply(X), X->multiply(make_scalar(2)) };
}

line by_two(expr X, expr Y)
{
	expr A = (X->conj())->substract(Y->conj());
	expr B = Y->substract(X);
	expr C = det(Y, X);
	return { A, B, C };
}

expr intersect(line l1, line l2)
{
	expr nom = (l1.C->multiply(l2.B))->substract(l1.B->multiply(l2.C));
	expr denom = (l1.A->multiply(l2.B))->substract(l1.B->multiply(l2.A));
	return nom->divide(denom);
}

bool concurrent(line l1, line l2, line l3)
{
	expr det1 = (l2.B->multiply(l3.C))->substract(l2.C->multiply(l3.B));
	expr det2 = (l2.C->multiply(l3.A))->substract(l2.A->multiply(l3.C));
	expr det3 = (l2.A->multiply(l3.B))->substract(l2.B->multiply(l3.A));
	expr det123 = (l1.A->multiply(det1))->add(l1.B->multiply(det2))->add(l1.C->multiply(det3));
	det123->print();
	std::cout << std::endl;
	return det123->expand()->checkZeroEquality();
}

expressionNode *parseString(const std::string &line, size_t l, size_t r);

int main()
{
	// TESTS
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	auto d = make_unit_term("d");
	auto X = rot_homothety(a, b, make_scalar(constTy(1, 1))->divide(make_scalar(2)));
	auto Y = rot_homothety(b, c, make_scalar(constTy(1, 1))->divide(make_scalar(2)));
	auto Z = rot_homothety(c, d, make_scalar(constTy(1, 1))->divide(make_scalar(2)));
	auto T = rot_homothety(d, a, make_scalar(constTy(1, 1))->divide(make_scalar(2)));
	(X->substract(Z)->multiply(make_scalar(constTy(0, 1))))->substract(T->substract(Y))->print();
	/*
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	line A = chord(a->multiply(a), b->multiply(c)->negate());
	line B = chord(b->multiply(b), c->multiply(a)->negate());
	line C = chord(c->multiply(c), a->multiply(b)->negate());
	std::cout << concurrent(A, B, C) << std::endl;
	*/
	/*
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	line A = tangent(a);
	line B = tangent(b);
	line C = tangent(c);
	auto a1 = intersect(B, C);
	auto b1 = intersect(A, C);
	auto c1 = intersect(A, B);
	std::cout << concurrent(by_two(a1, a), by_two(b1, b), by_two(c1, c)) << std::endl;
	*/
	/*
	//brianchon partial case
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	auto d = make_unit_term("d");
	line A = tangent(a);
	line B = tangent(b);
	line C = tangent(c);
	line D = tangent(d);
	auto X = intersect(A, B);
	auto Y = intersect(B, C);
	auto Z = intersect(C, D);
	auto T = intersect(D, A);
	std::cout << concurrent(by_two(X, Z), by_two(Y, T), chord(a, c));
	*/
	/*
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	auto d = make_unit_term("d");
	auto e = make_unit_term("e");
	auto f = make_unit_term("f");
	line AE = chord(a, e);
	line CE = chord(c, e);
	line BD = chord(b, d);
	line BF = chord(b, f);
	line AD = chord(a, d);
	line CF = chord(c, f);
	expr first = intersect(AE, BF);
	expr second = intersect(CE, BD);
	expr third = intersect(CF, AD);
	std::cout << collinear(first, second, third) << std::endl;
	/*
	
	*/
	/*
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	line AHa = by_two(a, b->multiply(c)->divide(a)->negate());
	line BHb = by_two(b, c->multiply(a)->divide(b)->negate());
	line CHc = by_two(c, a->multiply(b)->divide(c)->negate());
	std::cout << concurrent(AHa, BHb, CHc) << std::endl;
	*/
	/*
	auto a = make_term("a");
	auto b = make_term("b");
	auto c = make_term("c");
	line AMa = by_two(a, middlepoint(b, c));
	line BMb = by_two(b, middlepoint(a, c));
	line CMc = by_two(c, middlepoint(a, b));
	std::cout << concurrent(AMa, BMb, CMc) << std::endl;
	*/
	/*
	auto a = make_unit_term("a");
	auto b = make_unit_term("b");
	auto c = make_unit_term("c");
	auto d = make_unit_term("d");
	auto e = make_unit_term("e");
	auto f = make_unit_term("f");
	line A = tangent(a);
	line B = tangent(b);
	line C = tangent(c);
	line D = tangent(d);
	line E = tangent(e);
	line F = tangent(f);
	line first = by_two(intersect(A, B), intersect(E, D));
	line second = by_two(intersect(B, C), intersect(E, F));
	line third = by_two(intersect(C, D), intersect(A, F));
	std::cout << concurrent(first, second, third) << std::endl;
	*/
	// g++ geometry.cpp calculator.h calculator.cpp -o brianchon
	/*
	std::string s;
	std::cin >> s;
	auto res = parseString(s, 0, s.size());
	res->expand()->print();
	*/
	return 0;
}