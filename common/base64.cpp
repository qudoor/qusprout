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

#include <cctype>
#include <algorithm>
#include "base64.h"

const std::string BASE64_CHARS = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

size_t encodedSize(size_t raw)
{
	switch((raw*8)%6)
	{
		case 0:
			return (raw*8)/6;
		case 2:
			return ((raw*8)/6) + 3;
		case 4:
			return ((raw*8)/6) + 2;
	}
	return raw;
}

size_t decodedSize(size_t unpadded)
{
	return (unpadded*6)/8;
}

int base64Index(std::string::value_type c)
{
	if(c >= 'A' && c <= 'Z')
		return c-'A';
	else if(c >= 'a' && c <= 'z')
		return c-'a' + 26;
	else if(c >= '0' && c <= '9')
		return c-'0' + 52;
	else if(c == '+')
		return 62;
	else if(c == '/')
		return 63;
	else
		return -1;
}

CEncodeBase64::CEncodeBase64(std::string::size_type size)
 : m_state(zero), m_remainder(0)
{
	m_encoded.reserve(encodedSize(size));
}

void CEncodeBase64::operator() (std::string::value_type c)
{
	unsigned char value(0);
	switch(m_state)
	{
		case zero:
			value = (c & 0xfc) >> 2;
			m_remainder = c & 0x3;
			m_encoded.push_back(BASE64_CHARS[value]);
			m_state = two;
			break;
		case two:
			value = (m_remainder << 4) | ((c & 0xf0) >> 4);
			m_remainder = c & 0xf;
			m_encoded.push_back(BASE64_CHARS[value]);
			m_state = four;
			break;
		case four:
			value = (m_remainder << 2) | ((c & 0xc0) >> 6);
			m_remainder = c & 0x3f;
			m_encoded.push_back(BASE64_CHARS[value]);
			value = m_remainder;
			m_encoded.push_back(BASE64_CHARS[value]);
			m_state = zero;
			break;
	}
}

std::string CEncodeBase64::str()
{
	unsigned char value(0);
	switch(m_state)
	{
		case zero:
			break;
		case two:
			value = m_remainder << 4;
			m_encoded.push_back(BASE64_CHARS[value]);
			m_encoded.push_back('=');
			m_encoded.push_back('=');
			m_state = zero;
			break;
		case four:
			value = m_remainder << 2;
			m_encoded.push_back(BASE64_CHARS[value]);
			m_encoded.push_back('=');
			m_state = zero;
			break;
	}
	return m_encoded;
}

CDecodeBase64::CDecodeBase64(std::string::size_type size)
 : m_state(zero), m_remainder(0)
{
	m_decoded.reserve(decodedSize(size));
}

void CDecodeBase64::operator() (std::string::value_type c)
{
	unsigned char value(0);
	int index = base64Index(c);
	if(index == -1)
		return;
	switch(m_state)
	{
		case zero:
			m_remainder = index;
			m_state = six;
			break;
		case six:
			value = (m_remainder << 2) | ((index & 0x30) >> 4);
			m_remainder = index & 0xf;
			m_decoded.push_back(value);
			m_state = four;
			break;
		case four:
			value = (m_remainder << 4) | ((index & 0x3c) >> 2);
			m_remainder = index & 0x3;
			m_decoded.push_back(value);
			m_state = two;
			break;
		case two:
			value = (m_remainder << 6) | index;
			m_decoded.push_back(value);
			m_state = zero;
			break;
	}
}

std::string CDecodeBase64::str() const
{
	return m_decoded;
}

std::string encodeBase64(const std::string& str)
{
	return std::for_each(str.begin(), str.end(), CEncodeBase64(str.size())).str();
}

std::string decodeBase64(const std::string& str)
{
	size_t unpadded_size = str.size();
	if(str.size() > 0 && str[str.size()-1] == '=')
		unpadded_size -= 1;
	if(str.size() > 1 && str[str.size()-2] == '=')
		unpadded_size -= 1;

	return std::for_each(str.begin(), str.end(), CDecodeBase64(unpadded_size)).str();
}
