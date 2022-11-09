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

#ifndef SINGLETON_H
#define SINGLETON_H

#define SINGLETON(T) Singleton<T>::Instance()

template<typename T>
class Singleton
{
public:
    static T* Instance()
    {
        static Singleton t;
        return t.m_pT;
    }
private:
    Singleton()
    {
        m_pT = new T;
    }

    ~Singleton()
    {
        if (m_pT != NULL)
        {
            delete m_pT;
			m_pT = NULL;
        }
    }

    T* m_pT;
};

#endif