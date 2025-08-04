#include <iostream>
#include <memory>
#include <string>

class Fruit
{
public: 
    virtual void name() = 0;
};
class Apple : public Fruit
{
public:
    void name() override
    {
        std::cout << "Apple" << std::endl;
    }
};
class Banana : public Fruit
{
public:
    void name() override
    {
        std::cout << "Banana" << std::endl;
    }
};

class Animal
{
public:
    virtual void name() = 0;
};

class Sheep : public Animal
{
public:
    void name() override
    {
        std::cout << "i am a sheep" << std::endl;
    }
};
class Dog : public Animal
{
public:
    void name() override
    {
        std::cout << "i am a dog" << std::endl;
    }
};

class Cat : public Animal
{
public:
    void name() override
    {
        std::cout << "i am a cat" << std::endl;
    }
};


// 简单工厂
// class Factory
// {
// public:
//     std::shared_ptr<Fruit> create(const std::string& op)
//     {
//         if(op == "Apple")
//         {
//             return std::make_shared<Apple>();
//         }
//         else
//         {
//             return std::make_shared<Banana>();
//         }
//     }
// };

// void Test1()
// {
//     Factory fc;
//     std::shared_ptr<Fruit> fruit = fc.create("Apple");
//     fruit->name();
//     fruit = fc.create("Banana");
//     fruit->name();
// }


// 工厂方法模式
// class Factory
// {
// public:
//     virtual std::shared_ptr<Fruit> create() = 0;
// };

// class AppleFactory : public Factory
// {
// public:
//     std::shared_ptr<Fruit> create() override
//     {
//         return std::make_shared<Apple>();
//     }
// };

// class BananaFactory : public Factory
// {
// public:
//     std::shared_ptr<Fruit> create() override
//     {
//         return std::make_shared<Banana>();
//     }
// };

// void Test2()
// {
//     std::shared_ptr<Factory> fc(new AppleFactory);
//     std::shared_ptr<Fruit> fruit = fc->create();
//     fruit->name();
//     fc.reset(new BananaFactory);
//     fruit = fc->create();
//     fruit->name();
// }


// 抽象工厂模式
class Factory
{
public:
    virtual std::shared_ptr<Fruit> GetFruit(const std::string &op) = 0;
    virtual std::shared_ptr<Animal> GetAnimal(const std::string &op) = 0;
};

class FruitFactory : public Factory
{
public:
    std::shared_ptr<Fruit> GetFruit(const std::string &op) override
    {
        if(op == "苹果")
        {
            return std::make_shared<Apple>();
        }
        else
        {
            return std::make_shared<Banana>();
        }
    }
    std::shared_ptr<Animal> GetAnimal(const std::string &op) override
    {
        return std::shared_ptr<Animal>();
    }
};

class AnimalFactory : public Factory
{
public:
    std::shared_ptr<Fruit> GetFruit(const std::string &op) override
    {
        return std::shared_ptr<Fruit>();
    }
    std::shared_ptr<Animal> GetAnimal(const std::string &op) override
    {
        if(op == "羊")
        {
            return std::make_shared<Sheep>();
        }
        else if(op == "小狗")
        {
            return std::make_shared<Dog>();
        }
        else
        {
            return std::make_shared<Cat>();
        }
    }
};

class FactoryProducer
{
public:
    static std::shared_ptr<Factory> Produce(const std::string &op)
    {
        if(op == "水果")
        {
            return std::make_shared<FruitFactory>();
        }
        else 
        {
            return std::make_shared<AnimalFactory>();
        }
    }
};

void Test3()
{
    std::shared_ptr<Factory> ff = FactoryProducer::Produce("水果");
    std::shared_ptr<Fruit> fruit = ff->GetFruit("苹果");
    if(fruit) fruit->name();
    fruit = ff->GetFruit("香蕉");
    if(fruit) fruit->name();

    std::shared_ptr<Factory> af = FactoryProducer::Produce("动物");
    std::shared_ptr<Animal> animal = af->GetAnimal("羊");
    if(animal) animal->name();
    animal = af->GetAnimal("小狗");
    if(animal) animal->name();
    animal = af->GetAnimal("猫");
    if(animal) animal->name();

}
int main()
{
    // Test1();
    // Test2();
    Test3();
    return 0;
}