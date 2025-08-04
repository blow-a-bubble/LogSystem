#include <stdio.h>
#include <stdarg.h>
#include <iostream>

//宏函数不定参
#define Log(format, ...) printf("[%s:%d]" format "\n", __FILE__, __LINE__, ##__VA_ARGS__);

//C语言函数不定参
void printNum(int n, ...)
{
    va_list ap;
    va_start(ap, n);
    for (int i = 0; i < n; i++)
    {
        int num = va_arg(ap, int);
        printf("param[%d]:%d\n", i, num);;
    }
    va_end(ap);    
}
void myprintf(const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    char* buffer = nullptr;
    int ret = vasprintf(&buffer, fmt, ap);
    if(ret != -1)
    {
        printf("%s", buffer);
        free(buffer);
    }
    va_end(ap);
}
//C++函数不定参
void xprinrf()
{
    std::cout << std::endl;
}
template<typename T, typename ...Args>
void xprinrf(T&& t, Args ...args)
{
    std::cout << t;
    if((sizeof ...(args)) > 0)
    {
        xprinrf(std::forward<Args>(args)...);
    }
    else
    {
        xprinrf();
    }
}
int main()
{
    Log("%s %d", "hello", 100);
    printNum(3, 1, 2, 3);
    myprintf("hello wangxing\n");
    xprinrf("hello", "wangxingxing");
    return 0;
}