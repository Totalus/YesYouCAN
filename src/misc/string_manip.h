/*
	String manipulation functions to make it easier to decode text files
*/

#ifndef STRING_MANIP
#define STRING_MANIP

#include <QString>

QString singleSplit(QString seperator, QString& str);

QString extract(int index, QString& str);

QString singleSplit(QChar seperator, QString& line);

#endif // STRING_MANIP

