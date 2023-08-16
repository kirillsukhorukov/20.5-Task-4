#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>

using namespace std;

//Глобальный массив с номиналами банкнот
int banknote[]={100, 200, 500, 1000, 2000, 5000};

//Функция ввода и проверки целого числа
int int_input()
{
    string str;
    bool error = false;
    do
    {
        //Сброс ошибки
        error = false;

        getline(cin, str);

        //Проверка длины строки с числом
        if (str.length() > 9)
        {
            cerr << "Error! Repeat input." << endl;
            error = true;
        }
        else 
        {
            //Проверка что все символы являются числами
            for (int i = 0; i < str.length(); i++)
            {
                if (!isdigit(str[i]))
                {
                    cerr << "Error! Repeat input." << endl;
                    error = true;
                    i = str.length() - 1;
                }
            }
        }
    } while (error);
    
    //Возврат целого числа
    int num = stoi(str);
    return num;
}

//Функция записи информации из файла cach.bin в вектор
void reading_cash (vector <int> &vec, const int &n, int &checkSum)
{
    //Поток записи в файл
    ifstream cash;

    //Открытие файла для чтения и проверка что файл открыт
    cash.open("cash.bin", ios::binary);
    if (!cash.is_open()) cerr << "File cash.bin is not open!\n" << endl;

    //Записи информации из файла cach.bin в вектор
    for (int i=0; i<n; i++)
    {
        cash.read((char*)&vec[i], sizeof(int));
        checkSum+=vec[i];
    }

    //Закрытие файла
    cash.close();

}

//Функция записи в файл cash.bin информации из вектора
void writing_cash (vector <int> &vec, const int &n, int &checkSum)
{
    //Поток записи в файл
    ofstream cash;
    
    //Открытие файла для записи и проверка что файл открыт
    cash.open("cash.bin", ios::binary);
    if (!cash.is_open()) cerr << "File cash.bin is not open!\n" << endl;

    //Записи информации из файла cach.bin в вектор
    for (int i=0; i<n; i++)
    {
        cash.write((char*)&vec[i], sizeof(int));
        checkSum+=vec[i];
    }

    //Закрытие файла
    cash.close();
}

//Функция записи контрольнных сумм в файл
void writing_check (char &operation, const int &before, int &after)
{
    //Поток записи в файл
    ofstream check;
    
    //Открытие файла для записи и проверка что файл открыт
    check.open("check_sum.txt", ios::app);
    if (!check.is_open()) cerr << "File check_sum.txt is not open!\n" << endl;

    check << "Operation: " << operation <<  "\tCheck sum before: " << before << "\tCheck sum after: " << after;
    check << "\tThe amount has changed to: " << after-before << endl;

    //Закрытие файла
    check.close();
}

//Функция наполнения массива банкнотами со случайным номиналом
void filling (vector <int> &vec, const int &n)
{
    for (int i=0; i<n; i++)
    {
        if (vec[i]==0)
        {
            char denomination = rand()%6;
            vec[i]=banknote[denomination];
        }
    }
}

//Функция изъятия банкнот из вектора
void withdrawal (vector <int> &vec, const int &n, int &sum)
{
    for (int i=0; i<n; i++)
    {
        if (vec[i] <= sum)
        {
            sum -= vec[i];
            vec[i] = 0;
        }

        //Как только сумма для снятия набралась выходим из цикла
        if (sum == 0) i = n-1;
    }
}

//Функция ввода кода операции
void input_operation (char &operation)
{
    //Флаг ошибки ввода
    bool error=true;

    do
    {
        string str;
        cout << "Enter '+' to fill the ATM, '-' to withdraw cash, 'Q' to complete the operation: ";
        getline(cin, str);
        
        //Проверка вводы
        error= (str[0]=='+' ||  str[0]=='-' || str[0]=='Q') ? false : true;
        if (error) cerr << "Incorrect operation code entered, please repeat the entry!" << endl;
        else operation = str[0];

    } while (error);
}

//Функция запроса суммы снятия
void sum_input (int &sum)
{
    //Флаг ошибки ввода
    bool error=true;

    do 
    {
        //Ввод суммы
        cout << "Enter the amount to be withdrawn in multiples of 100: ";   
        sum = int_input();

        //Проверка кратности 100
        error= (sum%100 == 0) ? false : true;
        if (error) cerr << "Invalid amount entered, please repeat the entry!" << endl;
    }while (error);
}

int main()
{
    cout <<"-----ATM simulation-----\n" << endl;

    //Задание начального зерна случайных чисел
    srand(time(nullptr));

    //Создаем вектор в который содежит информацию о всех купюрах в банкомате
    int capacity=1000;
    vector <int> cashState(capacity);

    //Переменные контрольных сумм до и после операции
    int checkSumBefore = 0;
    int checkSumAfter = 0;

    //Переменная вида операции
    char operation;

    //Ввод кода операции
    input_operation(operation);

    if (operation == '+')
    {
        //Чтение в вектор информации из файла cash.bin
        reading_cash(cashState, capacity, checkSumBefore);
        
        //Наполнение вектора банкнотами со случайным номиналом
        filling (cashState, capacity);

        //Запись в файл cash.bin информации из вектора
        writing_cash(cashState, capacity, checkSumAfter);
        cout << "ATM replenishment!" << endl;

        //Запись контрольных сумм в файл
        writing_check(operation, checkSumBefore, checkSumAfter);
    }
    else if (operation == '-')
    {
        //Чтение в вектор информации из файла cash.bin
        reading_cash(cashState, capacity, checkSumBefore);

        //Запрос суммы снятия наличных
        int withdrawalSum = 0;
        sum_input(withdrawalSum);

        //Проверка достаточно ли денег в банкомате
        if (withdrawalSum <= checkSumBefore)
        {
            //Наполнение вектора банкнотами со случайным номиналом
            withdrawal(cashState, capacity, withdrawalSum);
        }
        else cout << "There are not enough funds in the ATM!" << endl;
        
        //Проверяем, вся ли сумма может быть выдана
        if (withdrawalSum == 0)
        //Если да, то выдаем сумму клиенту, списываем деньги с файла счета внося в него изменения
        {
            cout << "Amount issued, take cash!" << endl;
            
            //Запись в файл cash.bin информации из вектора
            writing_cash(cashState, capacity, checkSumAfter);
        } 

        //Если нет, выдаем сообщение, что сняти наличных невозможно и измениния в файл не вносим
        else 
        {
            cout << "The operation is not possible due to the lack of suitable money in the ATM!" << endl;
            
            //Уравниваем контрольные суммы до и после операции
            checkSumAfter = checkSumBefore;
        }
        

        //Запись контрольных сумм в файл
        writing_check(operation, checkSumBefore, checkSumAfter);
    }
    else if (operation == 'Q')
    {
        cout << "Operation canceled" << endl;
    }

    cout << endl << "Program completed. Press any key...";
    cin.get();
}