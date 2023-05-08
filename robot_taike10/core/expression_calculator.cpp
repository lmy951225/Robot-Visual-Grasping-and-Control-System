﻿#include <sstream>
#include <iostream>
#include <exception>
#include <stdexcept>
#include <cstring>
#include <algorithm>
#include <regex>
#include <list>
#include <cmath>

#include "expression_calculator.hpp"

namespace amc::core
{
	auto s_mm(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			Size row_idx = i * lda;
			for (Size j = 0; j < n; ++j)
			{
				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[row_idx + u] * B[j + u * ldb];
				}
			}
		}
	}
	auto s_mmTN(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			for (Size j = 0; j < n; ++j)
			{
				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[i + u * lda] * B[j + u * ldb];
				}
			}
		}
	}
	auto s_mmNT(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			Size row_idx = i * lda;
			for (Size j = 0; j < n; ++j)
			{
				Size col_idx = j * ldb;

				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[row_idx + u] * B[col_idx + u];
				}
			}
		}
	}
	auto s_mmTT(Size m, Size n, Size k, const double* A, Size lda, const double* B, Size ldb, double *C, Size ldc) noexcept->void
	{
		for (Size i = 0; i < m; ++i)
		{
			for (Size j = 0; j < n; ++j)
			{
				Size col_idx = j * ldb;
				Size idx = i * ldc + j;

				C[idx] = 0;
				for (Size u = 0; u < k; ++u)
				{
					C[idx] += A[i + u * lda] * B[col_idx + u];
				}
			}
		}
	}

	Matrix::Matrix(Size m, Size n, double value) : m_(m), n_(n), is_row_major_(true), data_vec_(m*n, value) { }
	Matrix::Matrix(Size m, Size n, const double *data) : Matrix(m, n) { std::copy(data, data + m * n, this->data()); }
	Matrix::Matrix(double value) : m_(1), n_(1), is_row_major_(true), data_vec_(1, value) {}
	Matrix::Matrix(const std::initializer_list<Matrix> &data) : Matrix()
	{
		std::list<std::list<Matrix> > mat_list_list;

		bool need_new_row = true;
		for (auto &i : data)
		{
			if (i.empty())
			{
				need_new_row = true;
			}
			else
			{
				if (need_new_row)
				{
					mat_list_list.push_back(std::list<Matrix>());
					need_new_row = false;
				}

				mat_list_list.back().push_back(i);
			}
		}


		(*this) = amc::core::combineMatrices(mat_list_list);
	}
	auto Matrix::swap(Matrix &other)->Matrix&
	{
		std::swap(this->m_, other.m_);
		std::swap(this->n_, other.n_);
		std::swap(this->is_row_major_, other.is_row_major_);
		std::swap(this->data_vec_, other.data_vec_);

		return *this;
	}
	auto Matrix::resize(Size m, Size n)->Matrix &
	{
		m_ = m;
		n_ = n;
		data_vec_.resize(m*n);
		return *this;
	}
	auto Matrix::transpose()->Matrix &
	{
		is_row_major_ = !is_row_major_;
		std::swap(m_, n_);
		return *this;
	}
	auto Matrix::copySubMatrixTo(const Matrix &subMat, Size beginRow, Size beginCol, Size rowNum, Size colNum)->void
	{
		if ((beginRow + subMat.m() > m()) || (beginCol + subMat.n() > n()))
		{
			throw std::logic_error("Function CopySubMatrixTo must have subMat smaller than self matrix");
		}

		for (Size i = 0; i < subMat.m(); ++i)
		{
			for (Size j = 0; j < subMat.n(); ++j)
			{
				this->operator()(i + beginRow, j + beginCol) = subMat(i, j);
			}
		}

	}
	auto Matrix::toString() const->std::string
	{
		std::stringstream stream;

		stream.precision(15);
		stream << "{";
		for (Size i = 0; i < m(); ++i)
		{
			for (Size j = 0; j < n(); ++j)
			{
				stream << this->operator()(i, j);
				if (j<n() - 1)stream << ",";
			}
			if (i<m() - 1)
				stream << ";\n";
		}
		stream << "}";

		return stream.str();
	}

	Matrix operator + (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if (m1.size() == 1)
		{
			ret = Matrix(m2);

			for (auto &d : ret)
			{
				d += *m1.data();
			}
		}
		else if (m2.size() == 1)
		{
			ret = Matrix(m1);

			for (auto &d : ret)
			{
				d += *m2.data();
			}
		}
		else if ((m1.m() == m2.m()) && (m1.n() == m2.n()))
		{
			ret.resize(m1.m(), m1.n());

			for (Size i = 0; i < ret.m(); ++i)
			{
				for (Size j = 0; j < ret.n(); ++j)
				{
					ret(i, j) = m1(i, j) + m2(i, j);
				}
			}
		}
		else
		{
			throw std::runtime_error("Can't plus matrices, the dimensions are not equal");
		}
		return ret;
	}
	Matrix operator - (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if ((m1.m() == 1) && (m1.n() == 1))
		{
			ret = Matrix(m2);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] = m1(0, 0) - ret.data()[i];
			}
		}
		else if ((m2.m() == 1) && (m2.n() == 1))
		{
			ret = Matrix(m1);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] -= m2(0, 0);
			}
		}
		else if ((m1.m() == m2.m()) && (m1.n() == m2.n()))
		{
			ret.resize(m1.m(), m1.n());

			for (Size i = 0; i < ret.m(); i++)
			{
				for (Size j = 0; j < ret.n(); j++)
				{
					ret(i, j) = m1(i, j) - m2(i, j);
				}
			}
		}
		else
		{
			throw std::runtime_error("Can't minus matrices, the dimensions are not equal");
		}

		return ret;
	}
	Matrix operator * (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if ((m1.m() == 1) && (m1.n() == 1))
		{
			ret = Matrix(m2);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] *= m1(0, 0);
			}
		}
		else if ((m2.m() == 1) && (m2.n() == 1))
		{
			ret = Matrix(m1);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] *= m2(0, 0);
			}
		}
		else if (m1.n() == m2.m())
		{
			ret.resize(m1.m(), m2.n());

			if (m1.is_row_major_)
			{
				if (m2.is_row_major_)
				{
					amc::core::s_mm(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
				else
				{
					amc::core::s_mmNT(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
			}
			else
			{
				if (m2.is_row_major_)
				{
					amc::core::s_mmTN(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
				else
				{
					amc::core::s_mmTT(m1.m(), m2.n(), m1.n(), m1.data(), 1, m2.data(), 1, ret.data(), 1);
				}
			}


		}
		else
		{
			throw std::runtime_error("Can't multiply matrices, the dimensions are not equal");
		}

		return ret;
	}
	Matrix operator / (const Matrix &m1, const Matrix &m2)
	{
		Matrix ret;

		if ((m1.m() == 1) && (m1.n() == 1))
		{
			ret = Matrix(m2);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] = m1(0, 0) / ret.data()[i];
			}
		}
		else if ((m2.m() == 1) && (m2.n() == 1))
		{
			ret = Matrix(m1);

			for (Size i = 0; i < ret.size(); ++i)
			{
				ret.data()[i] /= m2(0, 0);
			}
		}
		else
		{
			throw std::runtime_error("Right now, divide operator of matrices is not added");
		}

		return ret;
	}
	Matrix operator - (const Matrix &m1)
	{
		Matrix m(m1.m(), m1.n());
		for (Size i = 0; i < m1.size(); ++i)m.data()[i] = -m1.data()[i];

		return m;
	}
	Matrix operator + (const Matrix &m1)
	{
		return m1;
	}

	std::string SeperateString(const std::string &s)
	{
		static const std::string seperateOpr("+-*/^()[]{},;");
		std::string ret;

		for (const char &key : s)
		{
			// 判断是否为科学计数法的数字 //
			if ((&key > s.data() + 1) && (&key<s.data() + s.size() - 2))
			{
				if ((key == '+') || (key == '-'))
				{
					if ((*(&key - 1) == 'e')
						&& (*(&key - 2) <= '9')
						&& (*(&key - 2) >= '0')
						&& (*(&key + 1) <= '9')
						&& (*(&key + 1) >= '0'))
					{
						ret += key;
						continue;
					}
				}
			}


			if (seperateOpr.find(key) != seperateOpr.npos)
			{
				ret = ret + " " + key + " ";
			}
			else
			{
				ret += key;
			}
		}
		return ret;
	}

	Calculator::TokenVec Calculator::Expression2Tokens(const std::string &expression)const
	{
		std::stringstream stream(SeperateString(expression));

		TokenVec tokens;
		Token token;

		while (stream >> token.word)
		{
			token.type = Token::NO;

			switch (*token.word.c_str())
			{
			case ',':token.type = Token::COMMA; break;
			case ';':token.type = Token::SEMICOLON; break;
			case '(':token.type = Token::PARENTHESIS_L; break;
			case ')':token.type = Token::PARENTHESIS_R; break;
			case '[':token.type = Token::BRACKET_L; break;
			case ']':token.type = Token::BRACKET_R; break;
			case '{':token.type = Token::BRACE_L; break;
			case '}':token.type = Token::BRACE_R; break;
			default:
				// 数字
				if (std::stringstream(token.word) >> token.num)
				{
					token.type = Token::NUMBER;
					break;
				}
				// 操作符
				if (operator_map_.find(token.word) != operator_map_.end())
				{
					token.type = Token::OPERATOR;
					token.opr = &operator_map_.find(token.word)->second;
					break;
				}
				// 变量
				if (variable_map_.find(token.word) != variable_map_.end())
				{
					token.type = Token::VARIABLE;
					token.var = &variable_map_.find(token.word)->second;
					break;
				}
				// 函数
				if (function_map_.find(token.word) != function_map_.end())
				{
					token.type = Token::Function;
					token.fun = &function_map_.find(token.word)->second;
					break;
				}

			}
			if (token.type == Token::NO) throw std::runtime_error("unrecognized symbol \"" + token.word + "\"");
			tokens.push_back(token);
			continue;
		}

		return tokens;
	}
	Matrix Calculator::CaculateTokens(TokenVec::iterator beginToken, TokenVec::iterator endToken) const
	{
		if (beginToken >= endToken)
		{
			throw std::runtime_error("invalid expression");
		}

		auto i = beginToken;

		Matrix value;

		bool isBegin = true;

		while (i < endToken)
		{
			// 如果没有当前值,证明刚刚开始计算 //
			if (isBegin)
			{
				isBegin = false;
				switch (i->type)
				{
				case Token::PARENTHESIS_L:
					value = CaculateValueInParentheses(i, endToken);
					break;
				case Token::BRACE_L:
					value = CaculateValueInBraces(i, endToken);
					break;
				case Token::NUMBER:
					value = i->num;
					i++;
					break;
				case Token::OPERATOR:
					value = CaculateValueInOperator(i, endToken);
					break;
				case Token::VARIABLE:
					value = *i->var;
					i++;
					break;
				case Token::Function:
					value = CaculateValueInFunction(i, endToken);
					break;
				default:
					throw std::runtime_error("expression not valid");
				}
			}
			else//如果有当前值,但没有操作符
			{
				if (i->type == Token::OPERATOR)
				{
					if (i->opr->priority_ur)
					{
						value = i->opr->fun_ur(value);
						i++;
					}
					else if (i->opr->priority_b > 0)
					{
						auto e = FindNextEqualLessPrecedenceBinaryOpr(i + 1, endToken, i->opr->priority_b);
						value = i->opr->fun_b(value, CaculateTokens(i + 1, e));
						i = e;
					}
					else
					{
						throw std::runtime_error("expression not valid");
					}
				}
				else
				{
					throw std::runtime_error("expression not valid: lack operator");
				}
			}
		}

		return value;
	}

	Matrix Calculator::CaculateValueInParentheses(TokenVec::iterator &i, TokenVec::iterator maxEndToken)const
	{
		auto beginPar = i + 1;
		auto endPar = FindNextOutsideToken(i + 1, maxEndToken, Token::PARENTHESIS_R);
		i = endPar + 1;

		return CaculateTokens(beginPar, endPar);
	}
	Matrix Calculator::CaculateValueInBraces(TokenVec::iterator &i, TokenVec::iterator maxEndToken)const
	{
		auto beginBce = i + 1;
		auto endBce = FindNextOutsideToken(i + 1, maxEndToken, Token::BRACE_R);
		i = endBce + 1;

		return combineMatrices(GetMatrices(beginBce, endBce));
	}
	Matrix Calculator::CaculateValueInFunction(TokenVec::iterator &i, TokenVec::iterator maxEndToken)const
	{
		auto beginPar = i + 1;
		if (i + 1 >= maxEndToken) throw std::runtime_error("invalid expression");
		if (beginPar->type != Token::PARENTHESIS_L)throw std::runtime_error("function must be followed by \"(\"");

		auto endPar = FindNextOutsideToken(beginPar + 1, maxEndToken, Token::PARENTHESIS_R);
		auto matrices = GetMatrices(beginPar + 1, endPar);

		if (matrices.size() != 1)throw std::runtime_error("function \"" + i->word + "\" + do not has invalid param type");

		auto params = matrices.front();
		auto f = i->fun->funs.find(params.size());
		if (f == i->fun->funs.end())throw std::runtime_error("function \"" + i->word + "\" + do not has invalid param num");

		i = endPar + 1;
		return f->second(params);
	}
	Matrix Calculator::CaculateValueInOperator(TokenVec::iterator &i, TokenVec::iterator maxEndToken)const
	{
		auto opr = i;
		i = FindNextEqualLessPrecedenceBinaryOpr(opr + 1, maxEndToken, opr->opr->priority_ul);
		return opr->opr->fun_ul(CaculateTokens(opr + 1, i));
	}

	auto Calculator::FindNextOutsideToken(TokenVec::iterator beginToken, TokenVec::iterator endToken, Token::Type type)const->Calculator::TokenVec::iterator
	{
		Size parNum = 0, braNum = 0, bceNum = 0;
		auto nextPlace = beginToken;

		while (nextPlace < endToken)
		{
			if ((parNum == 0) && (braNum == 0) && (bceNum == 0) && (nextPlace->type == type))return nextPlace;
			switch (nextPlace->type)
			{
			case Token::PARENTHESIS_L:parNum++; break;
			case Token::PARENTHESIS_R:parNum--; break;
			case Token::BRACKET_L:braNum++; break;
			case Token::BRACKET_R:braNum--; break;
			case Token::BRACE_L:bceNum++; break;
			case Token::BRACE_R:bceNum--; break;
			default:break;
			}

			++nextPlace;
		}

		return nextPlace;
	}
	auto Calculator::FindNextEqualLessPrecedenceBinaryOpr(TokenVec::iterator beginToken, TokenVec::iterator endToken, Size precedence)const->Calculator::TokenVec::iterator
	{
		auto nextOpr = beginToken;

		while (nextOpr < endToken)
		{
			nextOpr = FindNextOutsideToken(nextOpr, endToken, Token::OPERATOR);

			if ((nextOpr == endToken) || (nextOpr->opr->priority_b <= precedence))
			{
				break;
			}
			else
			{
				++nextOpr;
			}
		}

		return nextOpr;
	}
	auto Calculator::GetMatrices(TokenVec::iterator beginToken, TokenVec::iterator endToken)const->std::vector<std::vector<Matrix> >
	{
		std::vector<std::vector<Matrix> > ret;

		auto rowBegin = beginToken;
		while (rowBegin < endToken)
		{
			auto rowEnd = FindNextOutsideToken(rowBegin, endToken, Token::SEMICOLON);
			auto colBegin = rowBegin;

			ret.push_back(std::vector<Matrix>());

			while (colBegin < rowEnd)
			{
				auto colEnd = FindNextOutsideToken(colBegin, rowEnd, Token::COMMA);

				ret.back().push_back(CaculateTokens(colBegin, colEnd));

				if (colEnd == endToken)
					colBegin = colEnd;
				else
					colBegin = colEnd + 1;
			}


			if (rowEnd == endToken)
				rowBegin = rowEnd;
			else
				rowBegin = rowEnd + 1;
		}


		return ret;
	}

	auto Calculator::calculateExpression(const std::string &expression) const->Matrix
	{
		auto tokens = Expression2Tokens(expression);
		return CaculateTokens(tokens.begin(), tokens.end());
	}
	auto Calculator::evaluateExpression(const std::string &expression)const->std::string
	{
		auto ret = expression;

		for (auto &var : string_map_)
		{
			std::string exp{ "\\$\\{" + var.first + "\\}" };

			std::regex var_rex{ exp };
			ret = std::regex_replace(ret, var_rex, var.second);
		}


		return ret;
	}
	auto Calculator::addVariable(const std::string &name, const Matrix &value)->void
	{
		if (function_map_.find(name) != function_map_.end())
		{
			throw std::runtime_error("function \"" + name + "already exists, can't add variable");
		}

		if (variable_map_.find(name) != variable_map_.end())
		{
			throw std::runtime_error("variable \"" + name + "already exists, can't add variable");
		}
		variable_map_.insert(make_pair(name, value));
	}
	auto Calculator::addVariable(const std::string &name, const std::string &value)->void
	{
		if (string_map_.find(name) != string_map_.end())
		{
			throw std::runtime_error("variable \"" + name + "already exists, can't add string variable");
		}
		string_map_.insert(make_pair(name, value));
	}
	auto Calculator::addFunction(const std::string &name, std::function<Matrix(std::vector<Matrix>)> f, Size n)->void
	{
		if (variable_map_.find(name) != variable_map_.end())
		{
			throw std::runtime_error("variable \"" + name + "already exists, can't add function");
		}

		function_map_[name].AddOverloadFun(n, f);
	}

	Calculator::Calculator()
	{
		operator_map_["+"].SetBinaryOpr(1, [](Matrix m1, Matrix m2) {return m1 + m2; });
		operator_map_["+"].SetUnaryLeftOpr(1, [](Matrix m) {return m; });
		operator_map_["-"].SetBinaryOpr(1, [](Matrix m1, Matrix m2) {return m1 - m2; });
		operator_map_["-"].SetUnaryLeftOpr(1, [](Matrix m) {return -m; });
		operator_map_["*"].SetBinaryOpr(2, [](Matrix m1, Matrix m2) {return m1 * m2; });
		operator_map_["/"].SetBinaryOpr(2, [](Matrix m1, Matrix m2) {return m1 / m2; });

		addFunction("sqrt", [](std::vector<Matrix> v)
		{
			Matrix ret = v.front();

			for (auto &d : ret)	d = std::sqrt(d);

			return ret;
		}, 1);
	}
}
