#include <assert.h>
#include <ostream>
#include <stdio.h>
#include <vector>
#include <deque>
#include <string>
#include "emit.h"
#include "cool-tree.h"
#include "symtab.h"

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
   void get_methods(std::vector<std::string>&methods);
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
   private:
      char *regname;
   public:
      Reference(char *r) { regname = r; }
      char *get_regname() { return regname; }
      void set_regname(char *r) { regname = r; }
};

class RegisterRef : public Reference
{
};

class OffsetRef : public Reference
{
   private:
      int offset;
   public:
      OffsetRef(char *r, int o) : Reference(r) { offset = o; }
      int get_offset() { return offset; }
      void set_offset(int o) { offset = o; }
};

class Environment : public SymbolTable<Symbol, Reference>
{
   private:
      int temp_num = 0;
      int stack_count = 0;
      int param_count = 0;
   public:
      void set_temp_num(int n) { temp_num = n;stack_count = DEFAULT_OBJFIELDS + temp_num; }
      int get_temp_num() { return temp_num; }
      int get_stack_count() { return stack_count; }
      void set_param_count(int n) { param_count = n; }
      int get_param_count() { return param_count; }
};