#ifndef __ast_h
#define __ast_h
/*
The code in this file was implemented by Professor Paulo Pagliosa 
from Facom - UFMS - Campo Grande - Brazil
*/
class Object
{
public:
  virtual ~Object(){}
  
  // Makes a clone of the object
  virtual Object* clone() const{ return 0; }

  // Gets the number of the object uses
  int getNumberOfUses() const{ return counter; }

  template <typename T> friend T* makeUse(T*);

  // Release this object
  void release()
  {
    if (--counter <= 0)
      delete this;
  }

protected:
  Object():counter(0){}

private:
  int counter;

};

template <typename T> inline T* makeUse(T* object) {
  if(object != 0)
    ++object->counter;
  return object;
}

template <typename T> inline void release(T* object){
  if (object != 0)
    object->release();
}

template <typename T> class ObjectPtr
{
public:
  ObjectPtr():object(0){ }

  ObjectPtr(const ObjectPtr<T>& ptr){ this->object = makeUse(ptr.object); }

  ObjectPtr(T* object){ this->object = makeUse(object); }

  ~ObjectPtr(){ release(this->object); }

  ObjectPtr<T>& operator =(T* object) {
    release(this->object);
    this->object = makeUse(object);
    return *this;
  }

  ObjectPtr<T>& operator =(const ObjectPtr<T>& ptr)
  {
    release(this->object);
    this->object = makeUse(ptr.object);
    return *this;
  }

  bool operator ==(T* object) const
  {
    return this->object == object;
  }

  bool operator ==(const ObjectPtr<T>& ptr) const
  {
    return this->object == ptr.object;
  }

  bool operator !=(T* object) const
  {
    return !operator ==(object);
  }

  bool operator !=(const ObjectPtr<T>& ptr) const
  {
    return !operator ==(ptr);
  }

  operator T*() const
  {
    return this->object;
  }

  T* operator ->() const
  {
    return object;
  }

private:
  T* object;

};

template <typename E> class Iterator;

template <typename E> class Collection: public Object {
public:
  virtual ~Collection(){}

  virtual bool add(const E&) = 0;
  virtual bool addAll(const Collection<E>&) = 0;
  virtual bool remove(const E&) = 0;
  virtual void clear() = 0;

  virtual bool contains(const E&) const = 0;
  virtual int size() const = 0;
  virtual bool isEmpty() const = 0;
  virtual ObjectPtr<Iterator<E> > iterator() const = 0;
};

template <typename E> class Iterator: public Object
{
public:
  virtual ~Iterator(){}

  virtual bool hasNext() const = 0;

  virtual E& next() = 0;
  virtual void restart() = 0;
};

template <typename E> class LinkedListIterator;

template <typename E> struct LinkedListElement
{
  E value;
  LinkedListElement<E>* next;
  LinkedListElement<E>* prev;

  LinkedListElement(const E& aValue):value(aValue){}

};

template <typename E> class LinkedList: public Collection<E>
{
public:
  LinkedList()
  {
    head = tail = 0;
    numberOfElements = 0;
  }
  ~LinkedList();

  bool add(const E&);
  bool addAll(const Collection<E>&);
  bool remove(const E&);
  void clear();

  bool contains(const E&) const;
  int size() const;
  bool isEmpty() const;
  ObjectPtr<Iterator<E> > iterator() const;

  const E& getFirst() const
  {
    return this->head->value;
  }

  Object* clone() const;
  bool operator ==(const LinkedList<E>&) const;

protected:
  LinkedListElement<E>* head;
  LinkedListElement<E>* tail;
  int numberOfElements;

  LinkedListElement<E>* find(const E&) const;

  LinkedList(const LinkedList<E>&);
  LinkedList<E>& operator =(const LinkedList<E>&);

  friend class LinkedListIterator<E>;

};

template <typename E> class LinkedListIterator: public Iterator<E>
{
public:
  LinkedListIterator(const LinkedList<E>& list)
  {
    current = head = list.head;
  }

  bool hasNext() const;

  E& next();
  void restart();

private:
  LinkedListElement<E>* head;
  LinkedListElement<E>* current;

};

template <typename E> LinkedList<E>::~LinkedList()
{
  clear();
}

template <typename E> bool LinkedList<E>::add(const E& value)
{
  LinkedListElement<E>* e = new LinkedListElement<E>(value);

  e->next = 0;
  e->prev = tail;
  if (tail == 0)
    head = e;
  else
    tail->next = e;
  tail = e;
  numberOfElements++;
  return true;
}

template <typename E> bool LinkedList<E>::addAll(const Collection<E>& c)
{
  bool changed = false;

  for (ObjectPtr<Iterator<E> > i = c.iterator(); i->hasNext();)
    changed |= add(i->next());
  return changed;
}

template <typename E> bool LinkedList<E>::remove(const E& value)
{
  LinkedListElement<E>* e = find(value);

  if (e == 0)
    return false;
  if (e == head)
    head = e->next;
  else
    e->prev->next = e->next;
  if (e == tail)
    tail = e->prev;
  else
    e->next->prev = e->prev;
  delete e;
  numberOfElements--;
  return true;
}

template <typename E> void LinkedList<E>::clear()
{
  while (head != 0)
  {
    LinkedListElement<E>* temp = head;

    head = head->next;
    delete temp;
  }
  tail = 0;
  numberOfElements = 0;
}

template <typename E> bool LinkedList<E>::contains(const E& value) const
{
  return find(value) != 0;
}

template <typename E> int LinkedList<E>::size() const
{
  return numberOfElements;
}

template <typename E> bool LinkedList<E>::isEmpty() const
{
  return numberOfElements == 0;
}

template <typename E> ObjectPtr<Iterator<E> > LinkedList<E>::iterator() const
{
  return new LinkedListIterator<E>(*this);
}

template <typename E> LinkedListElement<E>* LinkedList<E>::find(const E& value) const
{
  for (LinkedListElement<E>* e = head; e != 0; e = e->next)
    if (value == e->value)
      return e;
  return 0;
}

template <typename E> Object* LinkedList<E>::clone() const
{
  LinkedList<E>* c = new LinkedList<E>();

  for (LinkedListElement<E>* e = head; e != 0; e = e->next)
    c->add(e->value);
  return c;
}

template <typename E> bool LinkedList<E>::operator ==(const LinkedList<E>& list) const
{
  if (numberOfElements != list.numberOfElements)
    return false;

  LinkedListElement<E>* e1 = head;
  LinkedListElement<E>* e2 = list.head;

  for (; e1 != 0; e1 = e1->next, e2 = e2->next)
    if (e1->value != e2->value)
      return false;
  return true;
}

template <typename E> bool LinkedListIterator<E>::hasNext() const
{
  return current != 0;
}

template <typename E> E& LinkedListIterator<E>::next()
{
  E& value = current->value;

  current = current->next;
  return value;
}

template <typename E>
void
LinkedListIterator<E>::restart()
{
  current = head;
}

class Node: public Object{
public:
};

template <typename Node> class List{
public:
  typedef ObjectPtr<Iterator<ObjectPtr<Node> > > IteratorPtr;

  void add(Node* node)
  {
    this->nodes.add(node);
  }

  bool isEmpty() const
  {
    return this->nodes.isEmpty();
  }

  int size() const
  {
    return this->nodes.size();
  }

  IteratorPtr iterator() const
  {
    return this->nodes.iterator();
  }

  Node* getFirst() const
  {
    return nodes.getFirst();
  }



private:
  LinkedList<ObjectPtr<Node> > nodes;

};

class ExpressionNode {
public:
  virtual std::string print(){ return "NOTHING";};
};

class LogicExpressionNode: public ExpressionNode {
public:
  virtual std::string print(){ return "NOTHING";};
  virtual int getRHSType(){ return -1; };
};


#endif // __ast_h