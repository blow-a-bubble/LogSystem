#include <iostream>
#include <string>
#include <memory>
class Computer
{
public:
    virtual ~Computer() = default;
    void BuildBoard(const std::string& board)
    {
        _board = board;
    }
    void BuildDisplay(const std::string& display)
    {
        _display = display;
    }
    virtual void BuildOs() = 0;  
    void CoutParam()
    {
        std::cout << "电脑型号:\n" \
        << "\t 主板:" <<  _board << "\n"
        << "\t 显示器:" <<  _display << "\n"
        << "\t 操作系统:" <<  _os << "\n";
    }  
protected:
    std::string _board;
    std::string _display;
    std::string _os;
};


class MacComputer : public Computer
{
public:
    void BuildOs() override
    {
        _os = "MAC x12";
    }  
};

class WinComputer : public Computer
{
public:
    void BuildOs() override
    {
        _os = "Win 11";
    }  
};

class Builder
{
public:
    virtual ~Builder() = default;  // 添加虚析构函数
    virtual void BuildBoard(const std::string& board) = 0;
    virtual void BuildDisplay(const std::string& display) = 0;
    virtual void BuildOs() = 0;
    virtual std::shared_ptr<Computer> Build() = 0;
};

class MacBuilder : public Builder
{
public:
    MacBuilder() :_cmp(std::make_shared<MacComputer>()) {}
    void BuildBoard(const std::string& board) override
    {
        _cmp->BuildBoard(board);
    }
    void BuildDisplay(const std::string& display) override
    {
        _cmp->BuildDisplay(display);
    }
    void BuildOs() override
    {
        _cmp->BuildOs();
    }
    std::shared_ptr<Computer> Build() override
    {
        auto ret = _cmp;
        _cmp = std::make_shared<MacComputer>();
        return ret;
    }

private:
    std::shared_ptr<MacComputer> _cmp;
};

class WinBuilder : public Builder
{
public:
    WinBuilder() :_cmp(std::make_shared<WinComputer>()) {}

    void BuildBoard(const std::string& board) override
    {
        _cmp->BuildBoard(board);
    }
    void BuildDisplay(const std::string& display) override
    {
        _cmp->BuildDisplay(display);
    }
    void BuildOs() override
    {
        _cmp->BuildOs();
    }
    std::shared_ptr<Computer> Build() override
    {
        auto ret = _cmp;
        _cmp = std::make_shared<WinComputer>();
        return ret;
    }

private:
    std::shared_ptr<WinComputer> _cmp;
};

class Director
{
public:
    Director(std::shared_ptr<Builder> builder)
    :_builder(std::move(builder))
    {}
    void Construct(const std::string &board, const std::string display)
    {
        _builder->BuildBoard(board);
        _builder->BuildDisplay(display);
        _builder->BuildOs();
    }
private:
    std::shared_ptr<Builder> _builder;
};


int main()
{
    std::shared_ptr<MacBuilder> macBuilder = std::make_shared<MacBuilder>();
    std::shared_ptr<Director> macDirector = std::make_shared<Director>(macBuilder);
    macDirector->Construct("华硕主板", "三星显示器");
    std::shared_ptr<Computer> mac = macBuilder->Build();
    mac->CoutParam();

    macDirector->Construct("技嘉主板", "AOC显示器");
    std::shared_ptr<Computer> macTemp = macBuilder->Build();
    macTemp->CoutParam();

    
    std::shared_ptr<WinBuilder> winBuilder = std::make_shared<WinBuilder>();
    std::shared_ptr<Director> winDirector = std::make_shared<Director>(winBuilder);
    winDirector->Construct("微星主板", "ROG显示器");
    std::shared_ptr<Computer> win = winBuilder->Build();
    win->CoutParam();


    return 0;
}
