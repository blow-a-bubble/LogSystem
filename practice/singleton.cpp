#include <iostream>
using namespace std;
namespace Hungry
{
    class Singleton
    {   
    private:
        Singleton() { cout << "Singleton" << endl; }
        ~Singleton() { cout << "~Singleton" << endl; }
        Singleton(const Singleton& st) = delete;
        Singleton& operator=(const Singleton& st) = delete;
    public:
        static Singleton& GetInstance()
        {
            return _instance;
        }
        int GetData() { return _data; }
    private:
        static Singleton _instance;
        int _data = 10; 
    };
    Singleton Singleton::_instance;
}

namespace Lazy
{
    class Singleton
    {
    private:
        Singleton() { cout << "Singleton" << endl; }
        ~Singleton() { cout << "~Singleton" << endl; }
        Singleton(const Singleton& st) = delete;
        Singleton& operator=(const Singleton& st) = delete;
    public:
        static Singleton& GetInstance()
        {
            static Singleton _instance;
            return _instance;
        }
        int GetData() { return _data; }
    private:
        int _data;
    };
}
int main()
{
    cout << Hungry::Singleton::GetInstance().GetData() << endl;
    cout << Lazy::Singleton::GetInstance().GetData() << endl;
    return 0;
}