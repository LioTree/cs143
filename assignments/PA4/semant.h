#ifndef SEMANT_H_
#define SEMANT_H_

#include <assert.h>
#include <iostream>  
#include <map>
#include <vector>
#include "cool-tree.h"
#include "stringtab.h"
#include "symtab.h"
#include "list.h"

#define TRUE 1
#define FALSE 0

class ClassTable;
typedef ClassTable *ClassTableP;

// This is a structure that may be used to contain the semantic
// information such as the inheritance graph.  You may use it or not as
// you like: it is only here to provide a container for the supplied
// methods.

class ClassTable {
private:
  int semant_errors;
  void install_basic_classes();
  ostream& error_stream;
  std::map<Symbol,Class_> classes;
  std::map<Symbol,std::vector<Symbol>> inheritance_graph;
  std::map<Symbol,std::map<Symbol,method_class *>> class_methods;
  std::map<Symbol,std::map<Symbol,attr_class *>> class_attrs;
  bool main_exists;
  Symbol self_type;

public:
  ClassTable(Classes);
  void init_methods_attrs(class__class *c);
  void check_inheritance();
  void dfs_inheritance(Symbol current_class,std::map<Symbol, int> & visited);
  bool lookup_inheritance(Symbol child,Symbol parent);
  Symbol lub(Symbol class1, Symbol class2);
  Class_ lookup_class(Symbol name);
  method_class *lookup_method(Symbol class_name,Symbol method_name);
  void get_parent_attrs(Symbol class_name,std::vector<attr_class *> &attrs);
  int errors() { return semant_errors; }
  void set_self_type(Symbol type) { self_type = type; }
  Symbol get_self_type() { return self_type; }
  ostream& semant_error();
  ostream& semant_error(Class_ c);
  ostream& semant_error(Symbol filename, tree_node *t);
};


#endif

