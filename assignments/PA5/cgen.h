#include <assert.h>
#include <ostream>
#include <stdio.h>
#include <vector>
#include <deque>
#include <list>
#include <map>
#include <unordered_set>
#include <string>
#include <algorithm>
#include <memory>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

using std::shared_ptr;
using std::dynamic_pointer_cast;
using std::make_shared;

#define REF_PTR shared_ptr<Reference>
#define REG_PTR shared_ptr<RegisterRef>
#define OFFSET_PTR shared_ptr<OffsetRef>
#define MAKE_REF_PTR make_shared<Reference>
#define MAKE_REG_PTR make_shared<RegisterRef>
#define MAKE_OFFSET_PTR make_shared<OffsetRef>
#define TO_REG_PTR dynamic_pointer_cast<RegisterRef>
#define TO_OFFSET_PTR dynamic_pointer_cast<OffsetRef> 
#define REMOVE_CONST const_cast<char *>


enum Basicness     {Basic, NotBasic};
#define TRUE 1
#define FALSE 0

class CgenClassTable;
typedef CgenClassTable *CgenClassTableP;

class CgenNode;
typedef CgenNode *CgenNodeP;

class CgenClassTable : public SymbolTable<Symbol,CgenNode> {
private:
   List<CgenNode> *nds;
   ostream& str;
   int stringclasstag;
   int intclasstag;
   int boolclasstag;


// The following methods emit code for
// constants and global declarations.

   void code_global_data();
   void code_global_text();
   void code_bools(int);
   void code_select_gc();
   void code_constants();
   void code_protObj();
   void code_class_nameTab();
   void code_dispTab();
   void code_object_init();
   void code_methods();

// The following creates an inheritance graph from
// a list of classes.  The graph is implemented as
// a tree of `CgenNode', and class names are placed
// in the base class symbol table.

   void install_basic_classes();
   void install_class(CgenNodeP nd);
   void install_classes(Classes cs);
   void build_inheritance_tree();
   void set_relations(CgenNodeP nd);
public:
   CgenClassTable(Classes, ostream& str);
   void code();
   CgenNodeP root();
};


class CgenNode : public class__class {
private: 
   CgenNodeP parentnd;                        // Parent of class
   List<CgenNode> *children;                  // Children of class
   Basicness basic_status;                    // `Basic' if class is basic
                                              // `NotBasic' otherwise

public:
   CgenNode(Class_ c,
            Basicness bstatus,
            CgenClassTableP class_table);

   void add_child(CgenNodeP child);
   List<CgenNode> *get_children() { return children; }
   void set_parentnd(CgenNodeP p);
   CgenNodeP get_parentnd() { return parentnd; }
   int basic() { return (basic_status == Basic); }
   void code_def(ostream& s);
   void get_attrs(std::vector<attr_class *> &attrs, bool inherit);
   void get_methods(std::list<std::pair<CgenNodeP,method_class *>> &methods);
   void get_methods(std::vector<method_class *>&methods,bool inherit);
   void code_dispTab(ostream& s);
   void code_init(ostream& s);
   void code_methods(ostream& s);
};

class BoolConst 
{
 private: 
  int val;
 public:
  BoolConst(int);
  void code_def(ostream&, int boolclasstag);
  void code_ref(ostream&) const;
};

class Reference
{
   protected:
      char *regname;
   public:
      Reference(char *r) { regname = r; }
      // just need a virtual method to make Reference a polymorphic class
      virtual char *get_regname() = 0; 
      void set_regname(char *r) { regname = r; }
};

class RegisterRef : public Reference
{
   using Reference::Reference;
   public:
      char *get_regname() { return regname; }
};

class OffsetRef : public Reference
{
   private:
      int offset;
   public:
      OffsetRef(char *r, int o) : Reference(r) { offset = o; }
      int get_offset() { return offset; }
      char *get_regname() { return regname; }
};

class Environment : public SymbolTable<Symbol, Reference>
{
   private:
      std::map<Symbol,std::vector<Symbol>> disptable;
      int temp_num = 0;
      int param_num = 0;
      std::vector<shared_ptr<Reference>> temporaries;
      int temporaries_index = 0;
   public:
      void set_temp_num(int n);
      int get_temp_num() { return temp_num; }
      void set_param_num(int n) { param_num = n; }
      int get_param_num() { return param_num; }
      void back_temporaries_index(int n) { temporaries_index -= n; }
      REF_PTR get_new_temporary();
      void clear_temporaries() { std::vector<REF_PTR>().swap(temporaries); }
      void insert_disptable(Symbol classname, Symbol methodname);
      int lookup_disptable(Symbol classname,Symbol methodname);
};