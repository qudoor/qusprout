/****************************************************************************
*
* This code is licensed under the Apache License, Version 2.0. You may
* obtain a copy of this license in the LICENSE.txt file in the root directory
* of this source tree or at http://www.apache.org/licenses/LICENSE-2.0.
*
* Any modifications or derivative works of this code must retain this
* copyright notice, and modified files need to carry a notice indicating
* that they have been altered from the originals.
*
****************************************************************************/

#ifndef __H__BASE64__H__
#define __H__BASE64__H__

#include <string>

class CEncodeBase64
{
public:
	CEncodeBase64(std::string::size_type size);
	void operator() (std::string::value_type c);
	std::string str();

private:
	enum
	{
		zero = 0,
		two,
		four
	} m_state;
	unsigned int m_remainder;
	std::string m_encoded;
};

class CDecodeBase64
{
public:
	CDecodeBase64(std::string::size_type size);
	void operator() (std::string::value_type c);
	std::string str() const;

private:
	enum
	{
		zero = 0,
		six,
		four,
		two
	} m_state;
	unsigned int m_remainder;
	std::string m_decoded;
};

extern std::string encodeBase64(const std::string& str);
extern std::string decodeBase64(const std::string& str);

#endif