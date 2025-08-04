// #include <iostream>
// #include <memory>
// using namespace std;
// class Base
// {
// public:
//     void fun()
// };
// class Form : public Base
// {
// public:
//     void func()
//     {
//         cout << "hello form" << endl;
//     }
//     int _date = 10;
// };

// int main()
// {
//     unique_ptr<Base> ptr = std::make_unique<Form>();
//     ptr->_date;
//     return 0;
// }


#include <iostream>
#include <string>
#include <vector>
using namespace std;

class Solution {
public:
    // 正对角线 y = x + b -> b = y - x;
    // 负对角线 y = -x + b -> b = y + x;
    // 所以用b和正负来标记对角线
    vector<bool> colFlag; // 标记一列是否有皇后
    vector<bool> addB; // 标记 负对角线 b = y + x 是否存在
    vector<bool> subB; // 标记 正对角线 b = y + x 是否存在
    vector<vector<string>> ret;
    vector<vector<string>> solveNQueens(int n) 
    {
        colFlag.resize(n, false);
        addB.resize(n * 2 - 1);
        subB.resize(n * 2 - 1);
        vector<vector<char>> matrix(n, vector<char>(n, '.'));
        dfs(matrix, 0);
        return ret;
    }
    void dfs(vector<vector<char>>& matrix, int row)
    {
        if(row == matrix.size())
        {
            vector<string> result;
            for(auto & str : matrix)
            {
                result.push_back(string(str.begin(), str.end()));
            }
            ret.push_back(result);
            return;
        }
        for(int i = 0; i < matrix.size(); ++i)
        {
            if(colFlag[i] == false && addB[i + row] == false && subB[i - row] == false)
            {
                matrix[row][i] = 'Q';
                colFlag[i] = true;
                addB[i + row] = true;
                subB[i - row] = true;
                dfs(matrix, row + 1);

                //回溯
                matrix[row][i] = '.';
                colFlag[i] = false;
                addB[i + row] = false;
                subB[i - row] = false;
            }
        }
    }
};

int main()
{
    Solution question;
    auto ret = question.solveNQueens(4);
    for(auto e : ret)
    {
        for(auto m : e)
        {
            cout << m << endl;
        }
        cout << endl;
    }
}