

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"


extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}

static SymbolTable<Symbol,Entry> *symbol_table;
static ClassTable *classtable;

ClassTable::ClassTable(Classes user_classes) : semant_errors(0) , error_stream(cerr) {
    /* Fill this in */
    install_basic_classes();
    for (int i = user_classes->first(); user_classes->more(i); i = user_classes->next(i)) {
        class__class *c = dynamic_cast<class__class *>(user_classes->nth(i));
        if(classes.find(c->get_name()) != classes.end()) {
            semant_error(user_classes->nth(i)) << "class multi define" << endl;
            // exit(0);
        }
        classes[c->get_name()] = c; 
        init_methods_attrs(c);
    }

    for (auto it = classes.begin(); it != classes.end(); it++) {
        Symbol name = it->first;
        Symbol parent = dynamic_cast<class__class *>(it->second)->get_parent();
        if(parent != No_class && classes.find(parent) == classes.end()) {
            semant_error(it->second) << "parent class not found" << endl;
            exit(0);
        }
        inheritance_graph[name].push_back(parent);
    }
}

void ClassTable::init_methods_attrs(class__class *c) {
    Symbol class_name = c->get_name();
    Features features = c->get_features();
    for (int j = features->first(); features->more(j); j = features->next(j)) {
        Feature feature = features->nth(j);
        if (dynamic_cast<method_class *>(feature)) {
            method_class *method = dynamic_cast<method_class *>(feature);
            Symbol method_name = method->get_name();
            if(class_methods[class_name].find(method_name) != class_methods[class_name].end()) {
                semant_error(c) << "method multi define" << endl;
            //    exit(0);
            }
            class_methods[class_name][method_name] = method;
        }
        else {
            attr_class *attr = dynamic_cast<attr_class *>(feature);
            Symbol attr_name = attr->get_name();
            if(class_attrs[class_name].find(attr_name) != class_attrs[class_name].end()) {
                semant_error(c) << "attribute multi define" << endl;
            //    exit(0);
            }
            class_attrs[class_name][attr_name] = attr;
        }
    }
}

void ClassTable::check_inheritance() {
    for (auto it = inheritance_graph.begin(); it != inheritance_graph.end(); it++) {
        Symbol curr_class = it->first;
        std::map<Symbol, int> visited;
        visited[curr_class] = 1;
        dfs_inheritance(curr_class, visited);
    }
}

void ClassTable::dfs_inheritance(Symbol current_class,std::map<Symbol, int> & visited) {
    for (auto it = inheritance_graph[current_class].begin(); it != inheritance_graph[current_class].end(); it++) {
        Symbol parent = *it;
        if (visited.find(parent) == visited.end()) {
            visited[parent] = 1;
            dfs_inheritance(parent,visited);
        }
        else {
            semant_error(classes[current_class]) << "has cycle inheritance error!" << endl;
            exit(0);
        }
    }
}

bool ClassTable::lookup_inheritance(Symbol child,Symbol parent) {
    if(child == parent)
        return true;
    for(auto it = inheritance_graph[child].begin(); it != inheritance_graph[child].end(); it++) {
        if(*it == parent || lookup_inheritance(*it,parent)) {
            return true;
        }
    }
    return false;
}

Symbol ClassTable::lub(Symbol class1, Symbol class2) {
    if(class1 == class2) {
        return class1;
    }
    std::map<Symbol, int> visited;
    visited[class1] = 1;
    while(class1 != No_class) {
        class1 = dynamic_cast<class__class *>(lookup_class(class1))->get_parent();
        visited[class1] = 1;
    }
    while(class2 != No_class) {
        if(visited.find(class2) != visited.end()) {
            return class2;
        }
        class2 = dynamic_cast<class__class *>(lookup_class(class2))->get_parent();
    }
    return Object;
}


Class_ ClassTable::lookup_class(Symbol name) {
    return classes.find(name) != classes.end()?classes[name]:NULL;
}

method_class *ClassTable::lookup_method(Symbol class_name,Symbol method_name) {
    while(class_name != No_class) {
        if(class_methods[class_name].find(method_name) != class_methods[class_name].end()) {
            return class_methods[class_name][method_name];
        }
        class_name = dynamic_cast<class__class *>(classtable->lookup_class(class_name))->get_parent();
    }
    return NULL;
}


void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);
    classes[dynamic_cast<class__class *>(Object_class)->get_name()] = Object_class;
    init_methods_attrs(dynamic_cast<class__class *>(Object_class));

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  
    classes[dynamic_cast<class__class *>(IO_class)->get_name()] = IO_class;
    init_methods_attrs(dynamic_cast<class__class *>(IO_class));

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);
    classes[dynamic_cast<class__class *>(Int_class)->get_name()] = Int_class;
    init_methods_attrs(dynamic_cast<class__class *>(Int_class));

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);
    classes[dynamic_cast<class__class *>(Bool_class)->get_name()] = Bool_class;
    init_methods_attrs(dynamic_cast<class__class *>(Bool_class));

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);
    classes[dynamic_cast<class__class *>(Str_class)->get_name()] = Str_class;
    init_methods_attrs(dynamic_cast<class__class *>(Str_class));
}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 

void class__class::checkClassType() {
    symbol_table->enterscope();
    symbol_table->addid(self,name);
    Features features = get_features();
    // check type of attributes first
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (dynamic_cast<attr_class *>(feature)) {
            attr_class *attr = dynamic_cast<attr_class *>(feature);
            attr->checkFeatureType();
        }
    }
    //check type of methods
    for (int i = features->first(); features->more(i); i = features->next(i)) {
        Feature feature = features->nth(i);
        if (dynamic_cast<method_class *>(feature)) {
            method_class *method= dynamic_cast<method_class *>(feature);
            method->checkFeatureType();
        }
    }
    symbol_table->exitscope();
}

void method_class::checkFeatureType() {
    symbol_table->enterscope();
    for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
        formal_class *formal = dynamic_cast<formal_class *>(formals->nth(i));
        Symbol formal_type = formal->get_type_decl();
        Symbol formal_name = formal->get_name();
        symbol_table->addid(formal_name,formal_type);
    }

    Symbol expr_type = expr->checkExprType();
    symbol_table->exitscope();
    if(!classtable->lookup_inheritance(expr_type,return_type)) {
        cout << "return type error" << endl;
    }
}

void attr_class::checkFeatureType() {
    if(dynamic_cast<no_expr_class *>(init) == NULL) {
        Symbol init_type = init->checkExprType();
        if(!classtable->lookup_inheritance(init_type,type_decl)) {
            cout << "attribute init type error" << endl;
        }
    }
    symbol_table->addid(name,type_decl);
}

Symbol int_const_class::checkExprType() {
    return Int; 
}

Symbol plus_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "plus error" << endl;
    }
    return Int;
}

Symbol sub_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "sub error" << endl;
    }
    return Int;
}

Symbol mul_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "mul error" << endl;
    }
    return Int;
}

Symbol divide_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "divide error" << endl;
    }
    return Int;
}

Symbol bool_const_class::checkExprType() {
    return Bool;
}

Symbol string_const_class::checkExprType() {
    return Str;
}

Symbol new__class::checkExprType() {
    // ignore SELF_TYPE first
    if(classtable->lookup_class(type_name) == NULL) {
        cout << "new error" << endl;
    }
    return type_name;
}

Symbol comp_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    if(e1_type != Bool) {
        cout << "comp error" << endl;
    }
    return Bool;
}

Symbol loop_class::checkExprType() {
    Symbol pred_type = pred->checkExprType(); 
    if(pred_type != Bool) {
        cout << "loop error" << endl;
    }
    return Object;
}

Symbol object_class::checkExprType() {
    Symbol name_type = symbol_table->lookup(name);
    if(name_type == NULL) {
        cout << "unknown variable" << endl;
        name_type = Object;
    }
    return name_type;
}

Symbol no_expr_class::checkExprType() {
    return No_type;
}

Symbol assign_class::checkExprType() {
    Symbol expr_type = expr->checkExprType();
    Symbol var_type = symbol_table->lookup(name);
    if(var_type == NULL) {
        cout << "unknown variable" << endl;
        var_type = Object;
    }
    if(!classtable->lookup_inheritance(expr_type,var_type)) {
        cout << "assign error" << endl;
    }
    return expr_type;
}

Symbol cond_class::checkExprType() {
    Symbol pred_type = pred->checkExprType();
    Symbol then_type = then_exp->checkExprType();
    Symbol else_type = else_exp->checkExprType();
    if(pred_type != Bool) {
        cout << "cond error" << endl;
    }
    return classtable->lub(then_type,else_type);
}

Symbol block_class::checkExprType() {
    //foreach body
    Symbol expr_type;
    for (int i = body->first(); body->more(i); i = body->next(i)) {
        Expression e = dynamic_cast<Expression>(body->nth(i));
        expr_type = e->checkExprType();
    }
    return expr_type;
}

Symbol neg_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    if(e1_type != Int) {
        cout << "lt error" << endl;
    }
    return Int;
}

Symbol lt_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "lt error" << endl;
    }
    return Bool;
}

Symbol eq_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "eq error" << endl;
    }
    return Bool;
}

Symbol leq_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    Symbol e2_type = e2->checkExprType();
    if(e1_type != Int || e2_type != Int) {
        cout << "leq error" << endl;
    }
    return Bool;
}

Symbol isvoid_class::checkExprType() {
    Symbol e1_type = e1->checkExprType();
    return Bool;
}

// ignore SELF_TYPE first
Symbol static_dispatch_class::checkExprType() {
    Symbol expr_type = expr->checkExprType();
    if(!classtable->lookup_inheritance(expr_type,type_name)) {
        cout << "static dispatch error: expr_type and type_name is not equal" << endl;
    }
    Symbol return_type = Object;
    if(classtable->lookup_class(type_name) != NULL) {
        method_class *method = classtable->lookup_method(type_name,name);
        if(method != NULL) {
            Formals formals = method->get_formals();
            if(formals->len() != actual->len()) {
                cout << "static dispatch error: argument length not equal" << endl;
            }
            for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
                Symbol formal_type = dynamic_cast<formal_class *>(formals->nth(i))->get_type_decl();
                Symbol actual_type = actual->nth(i)->checkExprType();
                if(!classtable->lookup_inheritance(actual_type,formal_type)) {
                    cout << "static dispatch error:actual_type and formal_type not equal" << endl;
                }
            }
            return_type = method->get_return_type();
        }
        else {
            cout << "method not found" << endl;
        }
    }
    else {
        cout << "type not found" << endl;
    }
    return return_type;
}

// ignore SELF_TYPE first
Symbol dispatch_class::checkExprType() {
    Symbol expr_type = expr->checkExprType();
    Symbol return_type = Object;
    if(classtable->lookup_class(expr_type) != NULL) {
        method_class *method = classtable->lookup_method(expr_type,name);
        if(method != NULL) {
            Formals formals = method->get_formals();
            if(formals->len() != actual->len()) {
                cout << "dispatch error: argument length not equal" << endl;
            }
            for (int i = formals->first(); formals->more(i); i = formals->next(i)) {
                Symbol formal_type = dynamic_cast<formal_class *>(formals->nth(i))->get_type_decl();
                Symbol actual_type = actual->nth(i)->checkExprType();
                if(!classtable->lookup_inheritance(actual_type,formal_type)) {
                    cout << "dispatch error:actual_type and formal_type not equal" << endl;
                }
            }
            return_type = method->get_return_type();
        }
        else {
            cout << "method not found" << endl;
        }
    }
    else {
        cout << "type not found" << endl;
    }
    return return_type;
}

Symbol typcase_class::checkExprType() {
   return Object; 
}

Symbol let_class::checkExprType() {
    return Object;
}    

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    classtable = new ClassTable(classes);

    /* some semantic analysis code may go here */
    classtable->check_inheritance(); 
    symbol_table = new SymbolTable<Symbol, Entry>();
    for (int i = classes->first(); classes->more(i); i = classes->next(i)) {
        class__class *c = dynamic_cast<class__class *>(classes->nth(i));
        c->checkClassType();
    }

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}


