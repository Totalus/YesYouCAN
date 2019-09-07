#ifndef FILTEREXPRESSION_H
#define FILTEREXPRESSION_H

#include <QtCore>
#include "model_cantrace.h"

//#define REGEX_DOUBLE "(-{0,1}\\d+|-{0,1}\\d+\\.\\d+)"
#define REGEX_DOUBLE "-{0,1}\\d+\\.\\d+"
#define REGEX_UINT "(0b[01]+|0x[a-fA-F0-9]+|\\d+)"
#define REGEX_INT "-\\d+"

/* Référence

Liste de trucs à filtrer :
- msg.timestamp (== < >=)
- msg.type (==)
- msg.id
- msg.data[0-7] (==)
- msg.signal == name
- msg.len (== < >)
- msg.number
- r(regex)
- signal(name) == value

Opérandes de comparaison
==, !=, <, <=, >, >=
&&, ||

Nombres : binaires, hexadécimaux, decimal

*/


class FilterExpression
{
	enum Value_Type_t
	{
		TEXT,
		UINT,
		INT,
		DOUBLE,
		BOOLEAN
	};

	struct PatternType_t
	{
		QRegExp re;
		QStringList operands;
		Value_Type_t type;
	};

	const QList< PatternType_t > m_object_patterns = {
		{ QRegExp("msg.timestamp", Qt::CaseInsensitive, QRegExp::FixedString), { "==", "!=", "<", "<=", ">", ">=" } , DOUBLE},
		{ QRegExp("msg.type", Qt::CaseInsensitive, QRegExp::FixedString), { "==" }, TEXT},
		{ QRegExp("msg.id", Qt::CaseInsensitive, QRegExp::FixedString), { "==", "!=", "<", "<=", ">", ">=" }, UINT},
		{ QRegExp("msg.data\\[[0-7]\\]", Qt::CaseInsensitive, QRegExp::RegExp), { "==", "!=", "<", "<=", ">", ">=" }, UINT},
		{ QRegExp("msg.len", Qt::CaseInsensitive, QRegExp::FixedString), { "==", "!=", "<", "<=", ">", ">=" }, UINT},
		{ QRegExp("msg.name", Qt::CaseInsensitive, QRegExp::FixedString), { "==", "!=" }, TEXT},
		{ QRegExp("contains(*)", Qt::CaseInsensitive, QRegExp::Wildcard), { "&&", "||" }, BOOLEAN},
		{ QRegExp("signal(*)", Qt::CaseInsensitive, QRegExp::Wildcard), { "==", "!=", "<", "<=", ">", ">=" }, DOUBLE},
		//{ QRegExp("r(*)", Qt::CaseInsensitive, QRegExp::Wildcard), { "==", "!=", "<", "<=", ">", ">=" }, TEXT},
		{ QRegExp(REGEX_UINT, Qt::CaseInsensitive, QRegExp::RegExp), { "==", "!=", "<", "<=", ">", ">=" }, UINT},
		{ QRegExp(REGEX_INT, Qt::CaseInsensitive, QRegExp::RegExp), { "==", "!=", "<", "<=", ">", ">=" }, INT},
		{ QRegExp(REGEX_DOUBLE, Qt::CaseInsensitive, QRegExp::RegExp), { "==", "!=", "<", "<=", ">", ">=" }, DOUBLE},
		{ QRegExp("*", Qt::CaseInsensitive, QRegExp::Wildcard), { "==", "!=" }, TEXT}
	};

	public:
		FilterExpression(QString expression = QString(), DbcModel* dbc = 0);
		bool setExpression(QString expression, DbcModel* dbc = 0);
		bool setDbc(DbcModel* dbc);
		QString getOriginalExpression();

		// Evaluate expression for the specified message
		// Invalid expression returns true
		bool evaluate(const CanTraceMessage& msg) const;

		// Get value for objects
		QString valueStr(const CanTraceMessage& msg); // Object value
		double valueDouble(const CanTraceMessage& msg, bool *ok = nullptr);
		quint32 valueUInt(const CanTraceMessage& msg, bool *ok = nullptr);
		qint32 valueInt(const CanTraceMessage& msg, bool *ok = nullptr);

		bool checkValidity();
		bool isValid() const;
		QString getError();

		bool isObject();	// No operand, unique basic element

		Value_Type_t valueType();
		const QStringList& getValidOperands();

	private:
		bool m_object;	// The expression is an object (does not contain any operator)
		bool m_valid;	// The expression or object is valid
		DbcModel *m_dbc;

		QString m_originalExpression; // Full original expression
		QString m_expression; // Full expression, simplified for use (no spaces or useless parantheses)

		// Useful when the expression is an object
		PatternType_t m_varInfo; // Object information
		QString m_signalName;	// Holds the signal or message name (useful for "contains(signal name)" and "signal(signal name)" patterns)

		// Useful when the expression is not an object (it is an expression with an operand)
		QScopedPointer<FilterExpression> m_firstMember;		// First member of the expression
		QScopedPointer<FilterExpression> m_secondMember;	// Second member of the expression
		QString m_operand; // Expression operand

		// Reference operators
		QStringList m_operands;
		QStringList m_bin_operand;
		QStringList m_seperator;

		QString m_error; // Expression error if invalid

		// Manipulation functions
		int splitIndex(QString expression);
		QString reduce_level(QString expression);
		QStringList divide_expression(QString expression);

};

#endif // FILTEREXPRESSION_H
