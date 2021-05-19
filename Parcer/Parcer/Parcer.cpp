#include <string>
#include <map>
#include <stdio.h>
#include "errors.h"
#include "Parcer.h"

namespace __Stack
{
    struct Stack
    {
        std::string value;
        Stack* next;
    };

    Stack* tail = NULL;

    void        push(std::string);
    std::string pop();
    void        clear();
}

void __Stack::push(std::string value)
{
    Stack* stk = new Stack();

    stk->value = value;
    stk->next = tail;

    tail = stk;
}

std::string __Stack::pop()
{
    std::string result = tail->value;
    Stack* stk = tail;

    tail = tail->next;
    delete stk;

    return result;
}

void __Stack::clear()
{
    Stack* stk;

    while (tail)
    {
        stk = tail;
        tail = tail->next;
        delete stk;
    }
}


namespace __Parcer
{
    enum eTokens
    {
        NAME, NUMBER, FUNC, U_MINUS,
        PLUS = '+', MINUS = '-', MUL = '*', DIV = '/',
        POW = '^', SQRT = '\\',
        L_BRUNCH = '(', R_BRUNCH = ')'
    };

    eTokens curr_token;
    uint16_t char_pos = 0;
    bool sign = true;
    std::string x_name;
    std::string functions = { "sin", "cos" };
    std::map<eTokens, int> token_priority = { {L_BRUNCH, 0},
                                             {R_BRUNCH, 0},
                                             {PLUS,     1},
                                             {MINUS,    1},
                                             {MUL,      2},
                                             {DIV,      2},
                                             {POW,      3},
                                             {SQRT,     3},
                                             {U_MINUS,  4},
                                             {FUNC,     5} };
    const int funcs_count = 2;

    inline char getChar(std::string&);
    inline void backChar();
    bool        isFunc(std::string&);
    std::string getNumber(std::string&);
    std::string getName(std::string&);
    std::string getToken(std::string&);
    std::string pop();
    void        deleteSpaces(std::string);
    std::string toParcer(std::string&);
}

char __Parcer::getChar(std::string& s)
{
    return s[char_pos++];
}

void __Parcer::backChar()
{
    char_pos--;
}

bool __Parcer::isFunc(std::string& s)
{
    for (int i = 0; i < funcs_count; i++)
    {
        if (s.find(functions[i]))
        {
            return true;
        }
    }
    return false;
}

std::string __Parcer::getNumber(std::string& s)
{
    std::string result;
    char c = getChar(s);

    while ((c > '0') && (c < '9'))
    {
        result += c;
        if (char_pos == s.length() - 1)
        {
            return result;
        }
        c = getChar(s);
    }
    backChar();

    return result;
}

std::string __Parcer::getName(std::string& s)
{
    std::string result;
    char c = getChar(s);
    bool end_str = false;

    while (isalpha(c))
    {
        result += c;
        if (char_pos == s.length() - 1)
        {
            end_str = true;
            break;
        }
        c = getChar(s);
    }
    if (!end_str)
    {
        backChar();
    }

    if (isFunc(result))
    {
        curr_token = FUNC;
    }
    else
    {
        curr_token = NAME;
        if (x_name.empty())
        {
            x_name = result;
        }
        else
        {
            throw __ERRORS__("ошибка: неверное имя переменной", char_pos - result.length());
        }
    }

    return result;
}

std::string __Parcer::getToken(std::string& s)
{
    char c = getChar(s);

    switch (c)
    {
    case '+': case '-':  case '*': case '/':
    case '^': case '\\': case '(': case ')':
        curr_token = (eTokens)c;
        std::string result;
        result += c;
        return result;
    }

    if ((c > '0') && (c < '9'))
    {
        backChar();
        curr_token = NUMBER;
        return getNumber(s);
    }

    return getName(s);
}

std::string __Parcer::pop()
{
    std::string result = __Stack::pop();
    return getToken(result);
}

void __Parcer::deleteSpaces(std::string s)
{
    for (uint16_t i = 0; i < s.length(); i++)
    {
        if (s[i] == ' ')
        {
            s.erase(i, 1);
        }
    }
}

std::string __Parcer::toParcer(std::string& s)
{
    char_pos = 0;
    sign = true;

    int last = s.length() - 1;
    std::string result;
    std::string token_value;
    __Stack::clear();

    using __Stack::push;
    while (char_pos != last)
    {
        token_value = getToken(s);
        switch (curr_token)
        {
        case NUMBER:
        {
            sign = false;
            result += token_value + " ";
            break;
        }

        case L_BRUNCH:
        {
            sign = true;
            push(token_value);
            break;
        }

        case R_BRUNCH:
        {
            sign = false;
            while (true)
            {
                token_value = pop();
                if (curr_token == L_BRUNCH)
                {
                    break;
                }
                result += token_value + " ";
            }
            break;
        }

        case PLUS: {} case MINUS: {} case MUL: {}
        case DIV: {}  case POW: {}   case SQRT: {}
        case FUNC:
        {
            if (curr_token == MINUS)
            {
                if (sign)
                {
                    curr_token = U_MINUS;
                }
            }
            sign = false;
            eTokens tok = curr_token;
            std::string buff;
            while (true)
            {
                buff = pop();
                if (token_priority[curr_token] >= token_priority[tok])
                {
                    result += buff + " ";
                }
                else
                {
                    break;
                }
            }
            push(buff);
            push(token_value);
            break;
        }

        case NAME:
        {
            sign = false;
            result += token_value + " ";
            break;
        }

        default:
        {
            throw __ERRORS__("ошибка: неизвестный токен", char_pos);
            break;
        }
        }
    }

    return result;
}

std::string __Parcer::toParcer(std::string& s)
{
    __Parcer::deleteSpaces(s);
    return __Parcer::toParcer(s);
}