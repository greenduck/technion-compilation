#include "utils.h"
#include "linked_list.h"

LinkedListNode::LinkedListNode()
    :m_next(NULL),
     m_prev(NULL)
{
}

LinkedListNode::~LinkedListNode()
{
    Remove();
}

void LinkedListNode::AddNext(LinkedListNode *newNode)
{
//    BUG_IF((newNode->m_next != NULL || newNode->m_prev != NULL), "newNode already belongs to some linked list");
    newNode->m_next = m_next;
    m_next = newNode;
    newNode->m_prev = this;
    if (newNode->m_next != NULL) {
        newNode->m_next->m_prev = newNode;
    }
}

void LinkedListNode::AddPrev(LinkedListNode *newNode)
{
//    BUG_IF((newNode->m_next != NULL || newNode->m_prev != NULL), "newNode already belongs to some linked list");
    newNode->m_next = this;
    newNode->m_prev = m_prev;
    m_prev = newNode;
    if (newNode->m_prev != NULL) {
        newNode->m_prev->m_next = newNode;
    }
}

void LinkedListNode::Remove()
{
    if (m_prev != NULL) {
        m_prev->m_next = m_next;
    }

    if (m_next != NULL) {
        m_next->m_prev = m_prev;
    }

    m_next = NULL;
    m_prev = NULL;
}

LinkedListNode* LinkedListNode::First()
{
    return ((m_prev == NULL) ? this : m_prev->First());
}

LinkedListNode* LinkedListNode::Last()
{
    return ((m_next == NULL) ? this : m_next->Last());
}


/* iterator classes */
LinkedListNode::TopDown::TopDown(LinkedListNode *start)
    :m_current(start)
{
}

LinkedListNode::TopDown::~TopDown()
{
}

LinkedListNode* LinkedListNode::TopDown::FetchAndAdvance()
{
    LinkedListNode *ret = m_current;
    if (m_current != NULL) {
        m_current = m_current->m_next;
    }
    return ret;
}


LinkedListNode::BottomUp::BottomUp(LinkedListNode *start)
    :m_current(start)
{
}

LinkedListNode::BottomUp::~BottomUp()
{
}

LinkedListNode* LinkedListNode::BottomUp::FetchAndAdvance()
{
    LinkedListNode *ret = m_current;
    if (m_current != NULL) {
        m_current = m_current->m_prev;
    }
    return ret;
}

