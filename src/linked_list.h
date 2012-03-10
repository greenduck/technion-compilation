/* 
 * General purpose linked list implementation 
 * A class that needs linked list functionality, 
 * should inherit this class 
 */
#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

class LinkedListNode
{
private:
    LinkedListNode *m_next;
    LinkedListNode *m_prev;

public:
    LinkedListNode();
    ~LinkedListNode();

    inline LinkedListNode *Next()
    {
        return m_next;
    }

    inline LinkedListNode *Prev()
    {
        return m_prev;
    }

    void AddNext(LinkedListNode *new_node);
    void AddPrev(LinkedListNode *new_node);
    void Remove();
    LinkedListNode *First();
    LinkedListNode *Last();

    /* iterator classes */
    friend class TopDown;
    friend class BottomUp;

    class TopDown
    {
    private:
        LinkedListNode *m_current;

    public:
        TopDown(LinkedListNode *start);
        ~TopDown();
        LinkedListNode *FetchAndAdvance();
    };

    class BottomUp
    {
    private:
        LinkedListNode *m_current;

    public:
        BottomUp(LinkedListNode *start);
        ~BottomUp();
        LinkedListNode *FetchAndAdvance();
    };
};

#endif  // _LINKED_LIST_H

