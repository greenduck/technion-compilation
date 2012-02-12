
#include "utils.h"
#include "symbol.h"
#include "code.h"
#include "linked_list.h"
#include "backpatch.h"
#include "storage.h"

/*
 * Symbol scoping
 */

class Symbol
{
public:
	Symbol(const string name)
		:label(name)
	{
	}

	~Symbol()
	{
	}

	string Label()
	{
		return label;
	}

private:
	string label;
};

static void ScopeInfo_unittest()
{
	typedef CScopeInfo<Symbol> CScope;

	CScope *top;
	CScope *current;

	try {
		current = new CScope(NULL);
		top = current;
	
		Symbol a("a");
		Symbol b("b");
		Symbol c("c");
		current->Add(&a);
		current->Add(&b);
		current->Add(&c);
	
		current = new CScope(current);
		Symbol d("d");
		Symbol e("e");
		Symbol a2("a");
		current->Add(&d);
		current->Add(&e);
		current->Add(&a2);
		//current->Add(new Symbol("a"));

		top->Disp();

		current->Get("a");
		current->Get("b");
		current->Get("x");
	}
	catch (CCompilationException& ex) {
		cout << ex << endl;
	}
}

static void RXCode_unittest()
{
	CCodeBlock *cb = new CCodeBlock();

	CSymbol *a = newSymbol("a", CSymbol::INTEGER);
	CSymbol *b = newSymbol("b", CSymbol::INTEGER);
	CSymbol *c = newConst("18");

	cb->readi(a);
	cb->copyi(b, a);
	cb->prnti(b);

	CSymbol *l1 = newLabel("_____");
	CBPList *bpl1 = new CBPList(l1);
	cb->breqz(b, l1);

	cb->divdi(a, c, b);

	CSymbol *l2 = newLabel("_____");
	bpl1->Merge(l2);
	cb->ujump(l2);

	cb->prnti(c);

	CSymbol *lDest = newLabel("dest");
	bpl1->Backpatch(lDest);
	cb->label(lDest);

	cb->halt();

	for (CCodeBlock::iterator iter = cb->begin(); iter != cb->end(); ++iter) {
		cout << *iter << endl;
	}
}

static void LinkedList_unittest()
{
	class T : public LinkedListNode
	{
		public:
			string m_value;

			T(string value)
				:LinkedListNode(),
				 m_value(value)
			{
			}

			~T()
			{
			}
	};

	T a("A");
	T b("B");
	T c("C");
	T d("D");
	T e("E");
	T f("F");

	b.AddNext(&d);
	b.AddPrev(&a);
	d.AddNext(&c);
	c.AddNext(&f);
	c.AddNext(&e);

	cout << "First: " << static_cast<T*>(a.First())->m_value << endl;
	cout << "Last: " << static_cast<T*>(a.Last())->m_value << endl;

	cout << "All (top-down):" << endl;
	{
		T *x;
		T::TopDown it(&a);
		while ((x = static_cast<T*>(it.FetchAndAdvance())) != NULL) {
			cout << x->m_value << " ";
		}
		cout << endl;
	}

	cout << "All (bottom-up):" << endl;
	{
		T *x;
		T::BottomUp it(a.Last());
		while ((x = static_cast<T*>(it.FetchAndAdvance())) != NULL) {
			cout << x->m_value << " ";
		}
		cout << endl;
	}
}

void Storage_unittest()
{
	CRegAlloc regs(8, (const int[]){0, 1, 3, CRegAlloc::NOREG});

	int a = regs.Acquire();
	int b = regs.Acquire();
	int c = regs.Acquire();
	regs.Release(b);
	int d = regs.Acquire();
	int e = regs.Acquire(d);
	regs.Release(e);
	int f = regs.Acquire();
	int g = regs.Acquire();
	int h = regs.Acquire();

	printf("          %d, %d, %d, %d, %d, %d \n", a, c, d, f, g, h);
	printf("expected: 2, 5, 4, 6, 7, -1 \n");
}

int main()
{
	try {
		ScopeInfo_unittest();
		RXCode_unittest();
		LinkedList_unittest();
		Storage_unittest();
	}
	catch (CException &ex) {
		cerr << ex << endl;
	}
	return 0;
}

