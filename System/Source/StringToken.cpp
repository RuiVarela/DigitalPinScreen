#include "../Headers/StringToken.h"

std::istream &StringToken::cleanLine(std::istream &input)
{
	char c;
	do
	input.get(c);
	while(input.good() && c != '\n');

	return input;
}

char * StringToken::strcpyAndNewLine(char * destination, char * origin)
{
	while (*origin != '\0')
	{
		*destination = *origin;
		++destination;
		++origin;
	}
	*destination = '\n';
	return destination;
}


char * StringToken::trimLeft(char * ch)
{
	while (*ch == ' ')
		++ch;

	return ch;
}
int  StringToken::numberOfTokens(char * ch)
{
	int count = 0;

	if (*ch != '\0')
	{
		++count;

		while (*ch != '\0')
		{
			if(*ch == ' ')
				++count;

			++ch;
		}
	}

	return count;
}
char *  StringToken::nextToken(char * ch)
{
	while ((*ch != ' ') && (*ch != '\0'))
		++ch;

	return ++ch;
}
char * StringToken::getToken(char * ch, int token)
{
	for (;token;--token)
	{
		while (*ch != ' ')
			++ch;
		++ch;
	}

	return ch;
}
char *  StringToken::copyToken(char *dest, char *source)
{
	while((*source != ' ') && (*source != '\0'))
	{
		*dest = *source;
		++dest;
		++source;
	}
	*dest = '\0';

	if (*source == ' ')
		++source;

	return source;
}
