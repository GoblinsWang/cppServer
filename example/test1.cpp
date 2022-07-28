#include <iostream>
#include <memory>
#include <vector>
#include <functional>
using namespace std;

typedef function<void()> EventCallback;

class A : public enable_shared_from_this<A>
{
public:
    using ptr = shared_ptr<A>;
    A(string name) : m_name(name)
    {
        //
    }
    void Print()
    {
        cout << "in A::Print ...." << endl;
        cout << "shared_from_this().use_count: " << shared_from_this().use_count() << endl;
        auto self_ptr = shared_from_this();
        cout << "self_ptr.use_count: " << self_ptr.use_count() << endl;
        cout << self_ptr->m_name << endl;
    }

private:
    string m_name;
};

class B : public enable_shared_from_this<A>
{
public:
    B() = default;

    void newB(string name)
    {
        m_name = name;
        string a_name = "AAAAAA";
        A::ptr a_ptr(new A(a_name));
        vec_A.push_back(a_ptr);
        cout << "a_ptr.use_count: " << a_ptr.use_count() << endl;
    }

    void setEventCallback(EventCallback &cb)
    {
        eventCallback = cb;
    }

    void Print()
    {
        cout << "vec_A[0].use_count: " << vec_A[0].use_count() << endl;
        cout << "in B::Print ...." << endl;
        vec_A[0]->Print();
        cout << "vec_A[0].use_count: " << vec_A[0].use_count() << endl;
    }

private:
    string m_name;
    EventCallback eventCallback;
    vector<A::ptr> vec_A;
};

void message(A::ptr a_ptr)
{
}

int main()
{
    B *b = new B();
    string b_name = "BBBB";
    b->newB(b_name);
    b->Print();
}