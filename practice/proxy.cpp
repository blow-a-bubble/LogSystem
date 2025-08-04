#include <iostream>
#include <string>
#include <memory>
class AbstractRentHouse
{
public:
    ~AbstractRentHouse() = default;

    virtual void RentHouse() = 0;
};

class Landlord : public AbstractRentHouse
{
public:
    void RentHouse() override
    {
        std::cout << "把房子租出去\n";
    }
};

class Intermediary : AbstractRentHouse
{
public:
    Intermediary()
    :_landlord(new Landlord)
    {}
    void RentHouse() override
    {
        std::cout << "发布租房\n";
        _landlord->RentHouse();
        std::cout << "租房售后\n";
    }
private:
    std::unique_ptr<Landlord> _landlord;
};

int main()
{
    Intermediary intermediary;
    intermediary.RentHouse();
    return 0;;
}

