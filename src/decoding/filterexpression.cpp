#include "filterexpression.h"

FilterExpression::FilterExpression(QString expression, DbcModel *dbc)
{
	m_operands << "==" << "!=" << "<=" << ">=" << "<" << ">";
	m_bin_operand << "&&" << "||";
	m_dbc = dbc;
	m_signalName = QString();

	/*
	Liste variables possibles :
	- msg.timestamp
	- msg.type (== operator only)
	- msg.id
	- msg.data[0-7] (== <=)
	- msg.len (== < >)
	- msg.number
	- r(regex) (for evaluations)
	- signal(name) == value
	- contains(signal)
	*/

	setExpression(expression);
}

int FilterExpression::splitIndex(QString expression)
{
	// Find the next place to split the string
	QStringList specials;
	specials << m_operands << m_bin_operand << ")" << "(";

	int minIndex = -1;
	for(int i = 0; i < specials.count(); i++)
	{
		int index = expression.indexOf(specials.at(i));

		if(index == -1)
			continue;

		if(index == 0)
		{
			index += specials.at(i).count();
			return index;
		}

		if(minIndex == -1)
			minIndex = index;
		else
		{
			if(index < minIndex)
				minIndex = index;
		}
	}

	return minIndex;
}

QStringList FilterExpression::divide_expression(QString expression)
{
	//
	//	Split the expression in elements
	//  ex : "(A==B)&&C" should give the list "(", "A", "==", "B", , ")", "&&", "C"
	//

	int index;
	QStringList elements;

	while(true)
	{
		index = splitIndex(expression);

		if(index == -1)
		{
			if(!expression.isEmpty())
				elements.append(expression);
			break;
		}
		else
		{
			elements.append(expression.left(index));
			expression = expression.remove(0, index);
		}
	}

	return elements;
}

bool FilterExpression::setExpression(QString expression, DbcModel* dbc)
{
    if(dbc != nullptr)
		m_dbc = dbc;

	m_object = false;
	m_originalExpression = expression;
	m_expression = expression;

	/*
	 An expression is composed of 4 types of elements :
		- objects (ex : "msg.timestamp", "contains(signal)", "msg.data[1]", "0x02")
		- binary operand (||, &&, and, or)
		- operand (==, <=, >, !=)
		- parenthese ()

		Definition : the 'level' goes up each time a '(' is found and down each time a ')' is found
		ex :  "0 ( 1 ( 2 ) 1 ( 2 ) 1 ) 0"

	 Priority rules for splitting the expression in two subexpression :
	 1 - Split to the lower level operand
	 2 - Split to the last binary operand
	 3 - Split to the last operand (this would actually be an invalid expression of form A == B <= C == D)
	 We want to split to the operand that is the last applied in the expression

	 Apply rules in order up to 3 if necessary
	 ex : (A && B) == C should split at == (rule 1)
	 ex : (A == B) == C && D should split at && (rule 1 and 2)
	 ex : (A == B && C) <= E should split at <= (rule 1)
	 ex : A && B || C == D should split at || (rule 2)
	 note : those above are not necessarely valid expressions (validity checked later)
	*/

	//qDebug() << __LINE__ << " Processing original expression : " << expression;

	// Cleanup
	expression = expression.remove(' '); // Remove white spaces from expression

	if(expression.isEmpty())
	{
		m_valid = checkValidity();
		return m_valid;
	}

	expression = reduce_level(expression); // Remove external parentheses if they are paired
	m_expression = expression;	// Simplified expression

	if(expression.isEmpty())
	{
		m_valid = checkValidity();
		return m_valid;
	}

    //qDebug() << __LINE__ << " simplified expression : " << expression;

	// Split expression, ex : "(A==B)&&C" to the list "(", "A", "==", "B", , ")", "&&", "C"
	QStringList elements = divide_expression(expression);
    //qDebug() << __LINE__ << "\tElement list : " << elements;

	//
	// Find at which operand to split the expression to create two subexpressions
	//

	int level = 0;
	int operandMinLevel = 1000;
	int mainSplit = -1;

	for(int i = 0; i < elements.count(); i++)
	{
		if(elements.at(i) == "(")
			level++;
		else if(elements.at(i) == ")")
			level--;
		else if(m_operands.contains(elements.at(i)))
		{
			if(level < operandMinLevel)
			{
				mainSplit = i;
				operandMinLevel = level;
			}
		}
		else if(m_bin_operand.contains(elements.at(i)))
		{
			if(level <= operandMinLevel)
			{
				mainSplit = i;
				operandMinLevel = level;
			}
		}
	}

	// Split the expression in sub-expression or set as variable

	if(mainSplit == -1)
	{
        //qDebug() << __LINE__ << "\tIs an object : " << m_expression;
		m_object = true; // This is a variable (ex : "msg.timestamp")
	}
	else if(mainSplit == 0)
	{
        //qDebug() << __LINE__ << "Starts with an operand";
		// TODO : invalid expression (starts with operand)
        //m_object = true; // Consider the whole expression as an invalid object
    }
	else
	{
		// Recombine elements to get first and second members
		QString first;
		for(int j = 0; j < mainSplit; j++)
		{
			first += elements.at(j);
		}

		QString second;
		for(int k = mainSplit + 1; k < elements.count(); k++)
		{
			second += elements.at(k);
		}

		m_operand = elements.at(mainSplit);
		//qDebug() << __LINE__ << "\t" << "Final FilterExpression : " << first << m_operand << second;

		if(m_firstMember.isNull())
			m_firstMember.reset(new FilterExpression(first, m_dbc));
		else
			m_firstMember.data()->setExpression(first, m_dbc);

		if(m_secondMember.isNull())
			m_secondMember.reset(new FilterExpression(second, m_dbc));
		else
			m_secondMember.data()->setExpression(second, m_dbc);
	}

	m_valid = checkValidity();
	return m_valid;
}

bool FilterExpression::checkValidity()
{
	if(m_expression.isEmpty())
	{
		m_error = "Missing an expression (empty member)";
		return false;
	}

	// Level should go from 0 up and down back to 0, otherwise, missing or additional paranthese
	int level = 0;
	for(int i = 0; i < m_expression.count(); i++)
	{
		if(m_expression.at(i) == "(")
			level++;
		else if(m_expression.at(i) == ")")
			level--;
	}
	if(level != 0)
	{
		m_error = "Missing parenthese in expression \"" + m_originalExpression + "\"";
		return false;
	}

	if(m_object)
	{
		for(int i = 0; i < m_object_patterns.count(); i++)
		{
			if(m_object_patterns.at(i).re.exactMatch(m_expression))
			{
				m_varInfo = m_object_patterns.at(i); // Found matching pattern
				break;
			}
		}

		QStringList pattern_requiring_dbc = {"msg.name", "contains(*)", "signal(*)"};
        if(pattern_requiring_dbc.contains(m_varInfo.re.pattern()) && m_dbc == nullptr)
		{
			m_error = "Use of expression \"" + m_expression + "\" requires association to a CAN database";
			return false;
		}
		else
		{
			// Additional : check if signal is in database
		}

		if(m_varInfo.re.pattern() == "contains(*)")
		{
			m_signalName = m_expression; // Storing it avoids repeating this each evaluation
			m_signalName.remove("contains(", Qt::CaseInsensitive).remove(")");

            if(m_dbc->getSignal(m_signalName) == nullptr)
			{
				m_error = "Signal \"" + m_signalName + "\" from expression \"" + m_originalExpression + "\" not found in database";
				return false;
			}
		}
		else if(m_varInfo.re.pattern() == "signal(*)")
		{
			m_signalName = m_expression; // Storing it avoids repeating this each evaluation
			m_signalName.remove("signal(", Qt::CaseInsensitive).remove(")");

			// Define type according to DBC information
            if(m_dbc->getSignal(m_signalName) == nullptr)
			{
				m_error = "Signal \"" + m_signalName + "\" from expression \"" + m_originalExpression + "\" not found in database";
				return false;
			}
			else if(m_dbc->getSignal(m_signalName)->getSigness() == SIGNED)
				m_varInfo.type = INT;
			else if(m_dbc->getSignal(m_signalName)->getSigness() == UNSIGNED)
				m_varInfo.type = UINT;
			else
				m_varInfo.type = DOUBLE;
		}

        //qDebug() << "\tObject " << m_expression << " matched (" << m_varInfo.re.pattern() << ")" << "[" << m_varInfo.operands << "]" << m_varInfo.type;
	}
	else
	{
        if(m_firstMember.isNull())
        {
            m_error = "Invalid expression";
            return false;
        }

		// If the members are not valid, pass the error up
		if(!m_firstMember.data()->isValid())
		{
			m_error = m_firstMember.data()->getError();
			return false;
		}
		else if(!m_secondMember.data()->isValid())
		{
			m_error = m_secondMember.data()->getError();
			return false;
		}

		// Check if operand between the two members is valid
		if(!m_firstMember.data()->getValidOperands().contains(m_operand))
		{
			m_error = "Can't use operand \"" + m_operand + "\"\nwith member \"" + m_firstMember.data()->getOriginalExpression() + "\"\nin expression \"" + m_originalExpression + "\"";
			return false;
		}
		else if(!m_secondMember.data()->getValidOperands().contains(m_operand))
		{
			m_error = "Can't use operand \"" + m_operand + "\"\nwith member \"" + m_secondMember.data()->getOriginalExpression() + "\"\nin expression \"" + m_originalExpression + "\"";
			return false;
		}

		// If not binary operand, members must be objects
		if(!m_bin_operand.contains(m_operand))
		{
			if(!m_firstMember.data()->isObject())
			{
				m_error = "Can't use expression \"" + m_firstMember.data()->getOriginalExpression() + "\"\nwith operand \"" + m_operand + "\"\nin expression " + m_originalExpression;
				return false;
			}

			if(!m_secondMember.data()->isObject())
			{
				m_error = "Can't use expression \"" + m_secondMember.data()->getOriginalExpression() + "\"\nwith operand \"" + m_operand + "\"\nin expression " + m_originalExpression;
				return false;
			}
		}
	}

	return true;
}

QString FilterExpression::reduce_level(QString expression)
{
	expression = expression.remove(' '); // In case

	if(expression.isEmpty())
		return expression;

	while(expression.at(0) == '(' && expression.at(expression.count() - 1) == ')')
	{
		int minLevel = 0; // Starts with a '('
		for(int i = 0; i < expression.count() - 1; i++)
		{
			if(expression.at(i) == '(')
				minLevel++;
			else if(expression.at(i) == ')')
				minLevel--;

			if(minLevel == 0)
				return expression;
		}

		expression.remove(0, 1); // Remove leading '('
		expression.remove(expression.count() - 1, 1); // Remove trailing ')'

		if(expression.isEmpty())
			break;
	}

	return expression;
}

QString FilterExpression::valueStr(const CanTraceMessage &msg)
{
	if(m_varInfo.re.pattern() == "msg.type")
		return msg.type;

	else if(m_varInfo.re.pattern() == "msg.name")
	{
        if(m_dbc != nullptr)
		{
			CanMessage *m = m_dbc->getMessage(msg.id);
            if(m != nullptr)
				return m->getName();
			else
				return QString();
		}
		else
			return QString();
	}

	else if(m_varInfo.re.pattern() == "r(*)")
		return QString(m_expression).remove("r(").remove(")");

	else if(m_varInfo.re.pattern() == "*")
		return m_expression;

	else
		return QString();
}

double FilterExpression::valueDouble(const CanTraceMessage &msg, bool* ok)
{
	if(ok != nullptr)
		*ok = true;

	if(m_varInfo.re.pattern() == "msg.timestamp")
		return msg.timestamp;

	else if(m_varInfo.re.pattern() == "msg.id")
		return msg.id;

	else if(m_varInfo.re.pattern() == "signal(*)") // Return signal value
	{
		CanMessage* m = m_dbc->getMessage(msg.id);

		if(m != NULL)
		{
			QList<SignalValue_t> sigval_list = m->getSignalsValue(msg.data);

			for(int i = 0; i < sigval_list.count(); i++)
			{
				if(sigval_list.at(i).name == m_signalName)
					return sigval_list.at(i).value;
			}
		}

		if(ok != nullptr)
			*ok = false;

		return 0;
	}

	else if(m_varInfo.re.pattern() == "msg.data\\[[0-7]\\]")
	{
		int index = QString(m_expression).remove("msg.data[").remove("]").toInt(); // 0 to 7
		if(msg.data.count() > index)
			return msg.data.at(index);
		else
		{
			if(ok != nullptr)
				*ok = false;
			return 0;
		}

	}

	else if(m_varInfo.re.pattern() == REGEX_UINT)
	{
		if(m_expression.startsWith("0b")) // Binary
			return QString(m_expression).remove("0b").toUInt(ok, 2);
		else if(m_expression.startsWith("0x")) // Hex
			return QString(m_expression).remove("0x").toUInt(ok, 16);
		else
			return m_expression.toUInt();
	}

	else if(m_varInfo.re.pattern() == REGEX_DOUBLE)
	{
		return m_expression.toDouble(ok);
	}

	else if(m_varInfo.re.pattern() == "msg.len")
		return msg.data.count();

	else
	{
		if(ok != nullptr)
			*ok = false;

		return 0;
	}
}

quint32 FilterExpression::valueUInt(const CanTraceMessage &msg, bool *ok)
{
	if(ok != nullptr)
		*ok = true;

	if(m_varInfo.re.pattern() == "msg.id")
		return msg.id;

	else if(m_varInfo.re.pattern() == "signal(*)") // Return signal value
	{
		CanMessage *m = m_dbc->getMessage(msg.id);

        if(m != nullptr)
		{
			QList<SignalValue_t> sigval_list = m->getSignalsValue(msg.data);

			for(int i = 0; i < sigval_list.count(); i++)
			{
				if(sigval_list.at(i).name == m_signalName)
					return sigval_list.at(i).value;
			}
		}

		if(ok != nullptr)
			*ok = false;

		return 0;
	}

	else if(m_varInfo.re.pattern() == "msg.data\\[[0-7]\\]")
	{
		int index = QString(m_expression).remove("msg.data[").remove("]").toInt(); // 0 to 7
		if(msg.data.count() > index)
			return msg.data.at(index);
		else
		{
			if(ok != nullptr)
				*ok = false;
			return 0;
		}
	}

	else if(m_varInfo.re.pattern() == REGEX_UINT)
	{
		if(m_expression.startsWith("0b")) // Binary
			return QString(m_expression).remove("0b").toUInt(ok, 2);
		else if(m_expression.startsWith("0x")) // Hex
			return QString(m_expression).remove("0x").toUInt(ok, 16);
		else
			return m_expression.toUInt();
	}

	else if(m_varInfo.re.pattern() == REGEX_DOUBLE)
	{
		return m_expression.toDouble(ok);
	}

	else if(m_varInfo.re.pattern() == "msg.len")
		return msg.data.count();


	if(ok != nullptr)
		*ok = false;

	return 0;
}

qint32 FilterExpression::valueInt(const CanTraceMessage &msg, bool *ok)
{
	if(ok != nullptr)
		*ok = true;

	if(m_varInfo.re.pattern() == "msg.id")
		return msg.id;

	else if(m_varInfo.re.pattern() == "signal(*)") // Return signal value
	{
		CanMessage* m = m_dbc->getMessage(msg.id);

		if(m != NULL)
		{
			QList<SignalValue_t> sigval_list = m->getSignalsValue(msg.data);

			for(int i = 0; i < sigval_list.count(); i++)
			{
				if(sigval_list.at(i).name == m_signalName)
					return sigval_list.at(i).value;
			}
		}

		if(ok != nullptr)
			*ok = false;

		return 0;
	}

	else if(m_varInfo.re.pattern() == "msg.data\\[[0-7]\\]")
	{
		int index = QString(m_expression).remove("msg.data[").remove("]").toInt(); // 0 to 7
		if(msg.data.count() > index)
			return msg.data.at(index);
		else
		{
			if(ok != nullptr)
				*ok = false;
			return 0;
		}
	}

	else if(m_varInfo.re.pattern() == REGEX_UINT)
	{
		if(m_expression.startsWith("0b")) // Binary
			return QString(m_expression).remove("0b").toInt(ok, 2);
		else if(m_expression.startsWith("0x")) // Hex
			return QString(m_expression).remove("0x").toInt(ok, 16);
		else
			return m_expression.toInt();
	}

	else if(m_varInfo.re.pattern() == REGEX_DOUBLE)
	{
		return m_expression.toInt(ok);
	}

	else if(m_varInfo.re.pattern() == "msg.len")
		return msg.data.count();


	if(ok != nullptr)
		*ok = false;

	return 0;
}

bool FilterExpression::evaluate(const CanTraceMessage &msg) const
{
	if(!isValid())
		return true; // Invalid expression always returns true

	if(m_object)
	{
		if(m_varInfo.type == BOOLEAN) // "contains(signal name)"
		{
			// Check if signal in message
			CanMessage* m = m_dbc->getMessage(msg.id);
			if(m != NULL)
				return m->getSignalList().contains(m_signalName);
		}

		return false; // Invalid
	}


	if(m_operand == "&&")
	{
		return m_firstMember.data()->evaluate(msg) && m_secondMember.data()->evaluate(msg);
	}
	else if(m_operand == "||")
	{
		return m_firstMember.data()->evaluate(msg) || m_secondMember.data()->evaluate(msg);
	}
	else if(m_operand == "<" || m_operand == ">" || m_operand == "<=" || m_operand == ">=")
	{
		// Find what is the best type for evaluation (UINT if possible, Double otherwise)
		Value_Type_t type = UINT; // Type to use for evaluation
		if(m_firstMember.data()->valueType() == DOUBLE || m_secondMember.data()->valueType() == DOUBLE)
			type = DOUBLE;
		else if(m_firstMember.data()->valueType() == INT || m_secondMember.data()->valueType() == INT)
			type = INT;

		if(m_operand == "<")
		{
			if(type == UINT)
			{
				quint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueUInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueUInt(msg, &ok);
				if(!ok) return false;

				return val1 < val2;
			}
			else if(type == INT)
			{
				qint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueInt(msg, &ok);
				if(!ok) return false;

				return val1 < val2;
			}
			else
			{
				double val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueDouble(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueDouble(msg, &ok);
				if(!ok) return false;

				return val1 < val2;
			}
		}
		else if(m_operand == ">")
		{
			if(type == UINT)
			{
				quint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueUInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueUInt(msg, &ok);
				if(!ok) return false;

				return val1 > val2;
			}
			else if(type == INT)
			{
				qint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueInt(msg, &ok);
				if(!ok) return false;

				return val1 > val2;
			}
			else
			{
				double val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueDouble(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueDouble(msg, &ok);
				if(!ok) return false;

				return val1 > val2;
			}
		}
		else if(m_operand == "<=")
		{
			if(type == UINT)
			{
				quint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueUInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueUInt(msg, &ok);
				if(!ok) return false;

				return val1 <= val2;
			}
			else if(type == INT)
			{
				qint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueInt(msg, &ok);
				if(!ok) return false;

				return val1 <= val2;
			}
			else
			{
				double val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueDouble(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueDouble(msg, &ok);
				if(!ok) return false;

				return val1 <= val2;
			}
		}
		else if(m_operand == ">=")
		{
			if(type == UINT)
			{
				quint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueUInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueUInt(msg, &ok);
				if(!ok) return false;

				return val1 >= val2;
			}
			else if(type == INT)
			{
				qint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueInt(msg, &ok);
				if(!ok) return false;

				return val1 >= val2;
			}
			else
			{
				double val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueDouble(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueDouble(msg, &ok);
				if(!ok) return false;

				return val1 >= val2;
			}
		}
	}
	else if(m_operand == "==" || m_operand == "!=")
	{
		Value_Type_t type = UINT; // Type to use for evaluation
		if(m_firstMember.data()->valueType() == TEXT || m_secondMember.data()->valueType() == TEXT)
			type = TEXT;
		else if(m_firstMember.data()->valueType() == DOUBLE || m_secondMember.data()->valueType() == DOUBLE)
			type = DOUBLE;
		else if(m_firstMember.data()->valueType() == INT || m_secondMember.data()->valueType() == INT)
			type = INT;

		if(m_operand == "==")
		{
			if(type == TEXT)
			{
				return m_firstMember.data()->valueStr(msg) == m_secondMember.data()->valueStr(msg);
			}
			else if(type == DOUBLE)
			{
				double val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueDouble(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueDouble(msg, &ok);
				if(!ok) return false;

				return val1 == val2;
			}
			else if(type == INT)
			{
				qint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueInt(msg, &ok);
				if(!ok) return false;

				return val1 == val2;
			}
			else if(type == UINT)
			{
				quint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueUInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueUInt(msg, &ok);
				if(!ok) return false;

				return val1 == val2;
			}
		}
		else if(m_operand == "!=")
		{
			if(type == TEXT)
			{
				return m_firstMember.data()->valueStr(msg) == m_secondMember.data()->valueStr(msg);
			}
			else if(type == DOUBLE)
			{
				double val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueDouble(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueDouble(msg, &ok);
				if(!ok) return false;

				return val1 != val2;
			}
			else if(type == INT)
			{
				qint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueInt(msg, &ok);
				if(!ok) return false;

				return val1 != val2;
			}
			else if(type == UINT)
			{
				quint32 val1, val2;
				bool ok;

				val1 = m_firstMember.data()->valueUInt(msg, &ok);
				if(!ok)	return false;
				val2 = m_secondMember.data()->valueUInt(msg, &ok);
				if(!ok) return false;

				return val1 != val2;
			}
		}
	}

	return false;
}


bool FilterExpression::isObject()
{
	return m_object;
}

FilterExpression::Value_Type_t FilterExpression::valueType()
{	
	return m_varInfo.type;
}

const QStringList& FilterExpression::getValidOperands()
{
	if(m_object)
		return m_varInfo.operands; // Object operand
	else
		return m_bin_operand; // Expression
}

QString FilterExpression::getError()
{
	return m_error;
}

QString FilterExpression::getOriginalExpression()
{
	return m_originalExpression;
}

bool FilterExpression::setDbc(DbcModel *dbc)
{
	m_dbc = dbc;
	m_valid = checkValidity();
	return m_valid;
}


bool FilterExpression::isValid() const
{
	return m_valid;
}
