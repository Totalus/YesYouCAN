#include "string_manip.h"


QString singleSplit(QString seperator, QString& str)
{
	// Example : str = "abcdefghi" and seperator = "e"
	// returns "abcd" and line = "fghi" after the call

	int index = str.indexOf(seperator);
	QString ret = str.left(index);

	str = str.remove(0, index + seperator.length());

	return ret;
}

QString extract(int index, QString& str)
{
	QString ret = str.left(index);

	str = str.remove(0, index + 1);

	return ret;
}


QString singleSplit(QChar seperator, QString& line)
{
	// Example : line = "abcdefghi" and seperator = "e"
	// returns "abcd" and line = "fghi" after the call

	int index = line.indexOf(seperator);
	QString ret = line.left(index);

	line = line.remove(0, index + 1);

	return ret;
}
