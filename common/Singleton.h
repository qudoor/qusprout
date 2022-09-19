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