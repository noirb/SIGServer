#include "JspStringUtil.h"
#include "JspLog.h"
#include <stdio.h>
#include <string>
#include <vector>
#include <unistd.h>
#include <stdarg.h>

namespace Jsp
{
	/////////////////////////////////////////////////////////////
	//
	//	StringArray

	StringArray::StringArray()
	{
		m_strings.clear();
		m_ppArray = NULL;
		m_capacity = 0;
		m_bStringsUpdated = true;
	}

	StringArray::~StringArray()
	{
		freeArray();
	}

	void StringArray::clear()
	{
		m_strings.clear();
		m_bStringsUpdated = true;
	}

	void StringArray::add(const std::string &str)
	{
		m_strings.push_back(str);
		m_bStringsUpdated = true;
	}

	void StringArray::add(const char *format, ...)
	{
		char buf[256];
		va_list arg;

		va_start(arg, format);
		vsprintf(buf, format, arg);

		m_strings.push_back(buf);
		m_bStringsUpdated = true;

		va_end(arg);
	}

	void StringArray::decomp(const char *str)
	{
		char *buf = strdup(str);

		if (buf)
		{
			char *token = strtok(buf, " \t");

			while (token)
			{
				add((const char *)token);

				token = strtok(NULL, " \t");
			}

			free(buf);
		}
	}

	int StringArray::count()
	{
		return (int)m_strings.size();
	}

	char **StringArray::getArray(bool bNeedLastNULL)
	{
		if (m_bStringsUpdated)
		{
			freeArray();

			int n = count();

			if (bNeedLastNULL)
			{
				allocArray(n+1);
			}
			else
			{
				allocArray(n);
			}

			for (int i=0; i<n; i++)
			{
				m_ppArray[i] = (char *)m_strings[i].c_str();
			}

			if (bNeedLastNULL)
			{
				m_ppArray[n] = NULL;
			}

			m_bStringsUpdated = false;
		}

		return m_ppArray;
	}

	void StringArray::allocArray(int nStrings)
	{
		freeArray();

		if (nStrings > 0)
		{
			m_ppArray = (char **)malloc(sizeof(char *) * nStrings);
			if (!m_ppArray)
			{
				Jsp::printLog(LOG_FATAL, "*** out of memory *** [%s:%d]\n", __FILE__, __LINE__);
				exit(1);
			}

			for (int i=0; i<nStrings; i++)
			{
				m_ppArray[i] = NULL;
			}

			m_capacity = nStrings;
		}
	}

	void StringArray::freeArray()
	{
		if (m_ppArray)
		{
			// no need to do "free(ppArray[i])"
			free(m_ppArray);
			m_ppArray = NULL;
		}
	}
};
