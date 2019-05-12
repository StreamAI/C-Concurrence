#include <atomic>
#include <iostream>

template<typename T>
class lock_free_stack
{
private:
    struct node
    {
        T data;
        node* next;
        node(const T& data) : data(data), next(nullptr) {}
    };
    std::atomic<node*> head;

 public:
    lock_free_stack(): head(nullptr) {}
    void push(const T& data)
    {
        node* new_node = new node(data);
        do{
            new_node->next = head.load();   //将 head 的当前值放入new_node->next
        }while(!head.compare_exchange_strong(new_node->next, new_node));
        // 如果新元素new_node的next和栈顶head一样，证明在你之前没人操作它，使用新元素替换栈顶退出即可；
        // 如果不一样，证明在你之前已经有人操作它，栈顶已发生改变，该函数会自动更新新元素的next值为改变后的栈顶；
        // 然后继续循环检测直到状态1成立退出；
    }
    T pop()
    {
        node* node;
        do{
            node = head.load();
        }while (node && !head.compare_exchange_strong(node, node->next));

        if(node) 
            return node->data;
    }
};
 
int main()
{
    lock_free_stack<int> s;
    s.push(1);
    s.push(2);
    s.push(3);
    std::cout << s.pop() << std::endl;
    std::cout << s.pop() << std::endl;
    
    getchar();
    return 0;
}