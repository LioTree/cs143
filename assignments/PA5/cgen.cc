
//**************************************************************
//
// Code generator SKELETON
//
// Read the comments carefully. Make sure to
//    initialize the base class tags in
//       `CgenClassTable::CgenClassTable'
//
//    Add the label for the dispatch tables to
//       `IntEntry::code_def'
//       `StringEntry::code_def'
//       `BoolConst::code_def'
//
//    Add code to emit everyting else that is needed
//       in `CgenClassTable::code'
//
//
// The files as provided will produce code to begin the code
// segments, declare globals, and emit constants.  You must
// fill in the rest.
//
//**************************************************************

#include "cgen.h"
#include "cgen_gc.h"
#include "cool-tree.h"
#include "cool-tree.handcode.h"
#include <cstring>
#include <memory>
#include <ostream>
#include <string>
#include <vector>

extern void emit_string_constant(ostream& str, char *s);
extern int cgen_debug;

//
// Three symbols from the semantic analyzer (semant.cc) are used.
// If e : No_type, then no code is generated for e.
// Special code is generated for new SELF_TYPE.
// The name "self" also generates code different from other references.
//
//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
Symbol 
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

static char *gc_init_names[] =
  { "_NoGC_Init", "_GenGC_Init", "_ScnGC_Init" };
static char *gc_collect_names[] =
  { "_NoGC_Collect", "_GenGC_Collect", "_ScnGC_Collect" };

static int tag = 0;
static Environment env = Environment();

//  BoolConst is a class that implements code generation for operations
//  on the two booleans, which are given global names here.
BoolConst falsebool(FALSE);
BoolConst truebool(TRUE);

//*********************************************************
//
// Define method for code generation
//
// This is the method called by the compiler driver
// `cgtest.cc'. cgen takes an `ostream' to which the assembly will be
// emmitted, and it passes this and the class list of the
// code generator tree to the constructor for `CgenClassTable'.
// That constructor performs all of the work of the code
// generator.
//
//*********************************************************

void program_class::cgen(ostream &os) 
{
  // spim wants comments to start with '#'
  os << "# start of generated code\n";

  initialize_constants();
  CgenClassTable *codegen_classtable = new CgenClassTable(classes,os);

  os << "\n# end of generated code\n";
}


//////////////////////////////////////////////////////////////////////////////
//
//  emit_* procedures
//
//  emit_X  writes code for operation "X" to the output stream.
//  There is an emit_X for each opcode X, as well as emit_ functions
//  for generating names according to the naming conventions (see emit.h)
//  and calls to support functions defined in the trap handler.
//
//  Register names and addresses are passed as strings.  See `emit.h'
//  for symbolic names you can use to refer to the strings.
//
//////////////////////////////////////////////////////////////////////////////

static void emit_load(char *dest_reg, int offset, char *source_reg, ostream& s)
{
  s << LW << dest_reg << " " << offset * WORD_SIZE << "(" << source_reg << ")" 
    << endl;
}

static void emit_store(char *source_reg, int offset, char *dest_reg, ostream& s)
{
  s << SW << source_reg << " " << offset * WORD_SIZE << "(" << dest_reg << ")"
      << endl;
}

static void emit_load_imm(char *dest_reg, int val, ostream& s)
{ s << LI << dest_reg << " " << val << endl; }

static void emit_load_address(char *dest_reg, char *address, ostream& s)
{ s << LA << dest_reg << " " << address << endl; }

static void emit_partial_load_address(char *dest_reg, ostream& s)
{ s << LA << dest_reg << " "; }

static void emit_load_bool(char *dest, const BoolConst& b, ostream& s)
{
  emit_partial_load_address(dest,s);
  b.code_ref(s);
  s << endl;
}

static void emit_load_string(char *dest, StringEntry *str, ostream& s)
{
  emit_partial_load_address(dest,s);
  str->code_ref(s);
  s << endl;
}

static void emit_load_int(char *dest, IntEntry *i, ostream& s)
{
  emit_partial_load_address(dest,s);
  i->code_ref(s);
  s << endl;
}

static void emit_move(char *dest_reg, char *source_reg, ostream& s)
{ s << MOVE << dest_reg << " " << source_reg << endl; }

static void emit_neg(char *dest, char *src1, ostream& s)
{ s << NEG << dest << " " << src1 << endl; }

static void emit_binop(char *op,char *dest, char *src1, char *src2, ostream& s)
{ s << op << dest << " " << src1 << " " << src2 << endl; }

static void emit_add(char *dest, char *src1, char *src2, ostream& s)
{ s << ADD << dest << " " << src1 << " " << src2 << endl; }

static void emit_addu(char *dest, char *src1, char *src2, ostream& s)
{ s << ADDU << dest << " " << src1 << " " << src2 << endl; }

static void emit_addiu(char *dest, char *src1, int imm, ostream& s)
{ s << ADDIU << dest << " " << src1 << " " << imm << endl; }

static void emit_div(char *dest, char *src1, char *src2, ostream& s)
{ s << DIV << dest << " " << src1 << " " << src2 << endl; }

static void emit_mul(char *dest, char *src1, char *src2, ostream& s)
{ s << MUL << dest << " " << src1 << " " << src2 << endl; }

static void emit_sub(char *dest, char *src1, char *src2, ostream& s)
{ s << SUB << dest << " " << src1 << " " << src2 << endl; }

static void emit_sll(char *dest, char *src1, int num, ostream& s)
{ s << SLL << dest << " " << src1 << " " << num << endl; }

static void emit_jalr(char *dest, ostream& s)
{ s << JALR << "\t" << dest << endl; }

static void emit_jal(char *address,ostream &s)
{ s << JAL << address << endl; }

static void emit_return(ostream& s)
{ s << RET << endl; }

static void emit_gc_assign(ostream& s)
{ s << JAL << "_GenGC_Assign" << endl; }

static void emit_disptable_ref(Symbol sym, ostream& s)
{  s << sym << DISPTAB_SUFFIX; }

static void emit_init_ref(Symbol sym, ostream& s)
{ s << sym << CLASSINIT_SUFFIX; }

static void emit_label_ref(int l, ostream &s)
{ s << "label" << l; }

static void emit_protobj_ref(Symbol sym, ostream& s)
{ s << sym << PROTOBJ_SUFFIX; }

static void emit_method_ref(Symbol classname, Symbol methodname, ostream& s)
{ s << classname << METHOD_SEP << methodname; }

static void emit_label_def(int l, ostream &s)
{
  emit_label_ref(l,s);
  s << ":" << endl;
}

static void emit_beqz(char *source, int label, ostream &s)
{
  s << BEQZ << source << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_beq(char *src1, char *src2, int label, ostream &s)
{
  s << BEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bne(char *src1, char *src2, int label, ostream &s)
{
  s << BNE << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bleq(char *src1, char *src2, int label, ostream &s)
{
  s << BLEQ << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blt(char *src1, char *src2, int label, ostream &s)
{
  s << BLT << src1 << " " << src2 << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_blti(char *src1, int imm, int label, ostream &s)
{
  s << BLT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_bgti(char *src1, int imm, int label, ostream &s)
{
  s << BGT << src1 << " " << imm << " ";
  emit_label_ref(label,s);
  s << endl;
}

static void emit_branch(int l, ostream& s)
{
  s << BRANCH;
  emit_label_ref(l,s);
  s << endl;
}

//
// Push a register on the stack. The stack grows towards smaller addresses.
//
static void emit_push(char *reg, ostream& str)
{
  emit_store(reg,0,SP,str);
  emit_addiu(SP,SP,-4,str);
}

//
// Fetch the integer value in an Int object.
// Emits code to fetch the integer value of the Integer object pointed
// to by register source into the register dest
//
static void emit_fetch_int(char *dest, char *source, ostream& s)
{ emit_load(dest, DEFAULT_OBJFIELDS, source, s); }

//
// Emits code to store the integer value contained in register source
// into the Integer object pointed to by dest.
//
static void emit_store_int(char *source, char *dest, ostream& s)
{ emit_store(source, DEFAULT_OBJFIELDS, dest, s); }


static void emit_test_collector(ostream &s)
{
  emit_push(ACC, s);
  emit_move(ACC, SP, s); // stack end
  emit_move(A1, ZERO, s); // allocate nothing
  s << JAL << gc_collect_names[cgen_Memmgr] << endl;
  emit_addiu(SP,SP,4,s);
  emit_load(ACC,0,SP,s);
}

static void emit_gc_check(char *source, ostream &s)
{
  if (source != (char*)A1) emit_move(A1, source, s);
  s << JAL << "_gc_check" << endl;
}


///////////////////////////////////////////////////////////////////////////////
//
// coding strings, ints, and booleans
//
// Cool has three kinds of constants: strings, ints, and booleans.
// This section defines code generation for each type.
//
// All string constants are listed in the global "stringtable" and have
// type StringEntry.  StringEntry methods are defined both for String
// constant definitions and references.
//
// All integer constants are listed in the global "inttable" and have
// type IntEntry.  IntEntry methods are defined for Int
// constant definitions and references.
//
// Since there are only two Bool values, there is no need for a table.
// The two booleans are represented by instances of the class BoolConst,
// which defines the definition and reference methods for Bools.
//
///////////////////////////////////////////////////////////////////////////////

//
// Strings
//
void StringEntry::code_ref(ostream& s)
{
  s << STRCONST_PREFIX << index;
}

//
// Emit code for a constant String.
// You should fill in the code naming the dispatch table.
//

void StringEntry::code_def(ostream& s, int stringclasstag)
{
  IntEntryP lensym = inttable.add_int(len);

  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s  << LABEL                                             // label
      << WORD << stringclasstag << endl                                 // tag
      << WORD << (DEFAULT_OBJFIELDS + STRING_SLOTS + (len+4)/4) << endl // size
      << WORD;


 /***** Add dispatch information for class String ******/

      emit_disptable_ref(Str,s);  s << endl;                  // dispatch table 
      s << WORD;  lensym->code_ref(s);  s << endl;            // string length
  emit_string_constant(s,str);                                // ascii string
  s << ALIGN;                                                 // align to word
}

//
// StrTable::code_string
// Generate a string object definition for every string constant in the 
// stringtable.
//
void StrTable::code_string_table(ostream& s, int stringclasstag)
{  
  for (List<StringEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,stringclasstag);
}

//
// Ints
//
void IntEntry::code_ref(ostream &s)
{
  s << INTCONST_PREFIX << index;
}

//
// Emit code for a constant Integer.
// You should fill in the code naming the dispatch table.
//

void IntEntry::code_def(ostream &s, int intclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                // label
      << WORD << intclasstag << endl                      // class tag
      << WORD << (DEFAULT_OBJFIELDS + INT_SLOTS) << endl  // object size
      << WORD; 

 /***** Add dispatch information for class Int ******/

      emit_disptable_ref(Int,s);  s << endl;              // dispatch table
      s << WORD << str << endl;                           // integer value
}


//
// IntTable::code_string_table
// Generate an Int object definition for every Int constant in the
// inttable.
//
void IntTable::code_string_table(ostream &s, int intclasstag)
{
  for (List<IntEntry> *l = tbl; l; l = l->tl())
    l->hd()->code_def(s,intclasstag);
}


//
// Bools
//
BoolConst::BoolConst(int i) : val(i) { assert(i == 0 || i == 1); }

void BoolConst::code_ref(ostream& s) const
{
  s << BOOLCONST_PREFIX << val;
}
  
//
// Emit code for a constant Bool.
// You should fill in the code naming the dispatch table.
//

void BoolConst::code_def(ostream& s, int boolclasstag)
{
  // Add -1 eye catcher
  s << WORD << "-1" << endl;

  code_ref(s);  s << LABEL                                  // label
      << WORD << boolclasstag << endl                       // class tag
      << WORD << (DEFAULT_OBJFIELDS + BOOL_SLOTS) << endl   // object size
      << WORD;

 /***** Add dispatch information for class Bool ******/

      emit_disptable_ref(Bool,s);  s << endl;               // dispatch table
      s << WORD << val << endl;                             // value (0 or 1)
}


//////////////////////////////////////////////////////////////////////////////
//
//  CgenClassTable methods
//
//////////////////////////////////////////////////////////////////////////////

//***************************************************
//
//  Emit code to start the .data segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_data()
{
  Symbol main    = idtable.lookup_string(MAINNAME);
  Symbol string  = idtable.lookup_string(STRINGNAME);
  Symbol integer = idtable.lookup_string(INTNAME);
  Symbol boolc   = idtable.lookup_string(BOOLNAME);

  str << "\t.data\n" << ALIGN;
  //
  // The following global names must be defined first.
  //
  str << GLOBAL << CLASSNAMETAB << endl;
  str << GLOBAL; emit_protobj_ref(main,str);    str << endl;
  str << GLOBAL; emit_protobj_ref(integer,str); str << endl;
  str << GLOBAL; emit_protobj_ref(string,str);  str << endl;
  str << GLOBAL; falsebool.code_ref(str);  str << endl;
  str << GLOBAL; truebool.code_ref(str);   str << endl;
  str << GLOBAL << INTTAG << endl;
  str << GLOBAL << BOOLTAG << endl;
  str << GLOBAL << STRINGTAG << endl;

  //
  // We also need to know the tag of the Int, String, and Bool classes
  // during code generation.
  //
  str << INTTAG << LABEL
      << WORD << intclasstag << endl;
  str << BOOLTAG << LABEL 
      << WORD << boolclasstag << endl;
  str << STRINGTAG << LABEL 
      << WORD << stringclasstag << endl;    
}


//***************************************************
//
//  Emit code to start the .text segment and to
//  declare the global names.
//
//***************************************************

void CgenClassTable::code_global_text()
{
  str << GLOBAL << HEAP_START << endl
      << HEAP_START << LABEL 
      << WORD << 0 << endl
      << "\t.text" << endl
      << GLOBAL;
  emit_init_ref(idtable.add_string("Main"), str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Int"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("String"),str);
  str << endl << GLOBAL;
  emit_init_ref(idtable.add_string("Bool"),str);
  str << endl << GLOBAL;
  emit_method_ref(idtable.add_string("Main"), idtable.add_string("main"), str);
  str << endl;
}

void CgenClassTable::code_object_init()
{
  for(List<CgenNode> *l = nds; l; l = l->tl()) {
    CgenNodeP node = l->hd();
    node->code_init(str);
  }
}

void CgenClassTable::code_methods()
{
  for(List<CgenNode> *l = nds; l; l = l->tl()) {
    CgenNodeP node = l->hd();
    node->code_methods(str);
  }
}

void CgenClassTable::code_bools(int boolclasstag)
{
  falsebool.code_def(str,boolclasstag);
  truebool.code_def(str,boolclasstag);
}

void CgenClassTable::code_select_gc()
{
  //
  // Generate GC choice constants (pointers to GC functions)
  //
  str << GLOBAL << "_MemMgr_INITIALIZER" << endl;
  str << "_MemMgr_INITIALIZER:" << endl;
  str << WORD << gc_init_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_COLLECTOR" << endl;
  str << "_MemMgr_COLLECTOR:" << endl;
  str << WORD << gc_collect_names[cgen_Memmgr] << endl;
  str << GLOBAL << "_MemMgr_TEST" << endl;
  str << "_MemMgr_TEST:" << endl;
  str << WORD << (cgen_Memmgr_Test == GC_TEST) << endl;
}


//********************************************************
//
// Emit code to reserve space for and initialize all of
// the constants.  Class names should have been added to
// the string table (in the supplied code, is is done
// during the construction of the inheritance graph), and
// code for emitting string constants as a side effect adds
// the string's length to the integer table.  The constants
// are emmitted by running through the stringtable and inttable
// and producing code for each entry.
//
//********************************************************

void CgenClassTable::code_constants()
{
  //
  // Add constants that are required by the code generator.
  //
  stringtable.add_string("");
  inttable.add_string("0");

  stringtable.code_string_table(str,stringclasstag);
  inttable.code_string_table(str,intclasstag);
  code_bools(boolclasstag);
}

void CgenClassTable::code_protObj()
{
  for(List<CgenNode> *l = nds; l; l = l->tl()) {
    CgenNodeP node = l->hd();
    node->code_def(str);
  }
}

void CgenClassTable::code_class_nameTab()
{
  str << CLASSNAMETAB << LABEL;
  std::deque<CgenNodeP> classes;
  for(List<CgenNode> *l = nds; l; l = l->tl())
    classes.push_front(l->hd());
  for(auto it = classes.begin(); it != classes.end(); ++it) {
    str << WORD;  stringtable.lookup_string((*it)->name->get_string())->code_ref(str); str << endl;
  }
}

void CgenClassTable::code_dispTab()
{
  for(List<CgenNode> *l = nds; l; l = l->tl()) {
    CgenNodeP node = l->hd();
    node->code_dispTab(str);
  }
}

CgenClassTable::CgenClassTable(Classes classes, ostream& s) : nds(NULL) , str(s)
{
   tag = classes->len() + 4; /* tags:  Object:0,IO:1,Int:2,Bool:3,String:4,user classes and so on... */
   stringclasstag = 4     /* Change to your String class tag here */;
   intclasstag =    2     /* Change to your Int class tag here */;
   boolclasstag =   3     /* Change to your Bool class tag here */;

   enterscope();
   if (cgen_debug) cout << "Building CgenClassTable" << endl;
   install_basic_classes();
   install_classes(classes);
   build_inheritance_tree();

   code();
   exitscope();
}

void CgenClassTable::install_basic_classes()
{

// The tree package uses these globals to annotate the classes built below.
  //curr_lineno  = 0;
  Symbol filename = stringtable.add_string("<basic class>");

//
// A few special class names are installed in the lookup table but not
// the class list.  Thus, these classes exist, but are not part of the
// inheritance hierarchy.
// No_class serves as the parent of Object and the other special classes.
// SELF_TYPE is the self class; it cannot be redefined or inherited.
// prim_slot is a class known to the code generator.
//
  addid(No_class,
	new CgenNode(class_(No_class,No_class,nil_Features(),filename),
			    Basic,this));
  addid(SELF_TYPE,
	new CgenNode(class_(SELF_TYPE,No_class,nil_Features(),filename),
			    Basic,this));
  addid(prim_slot,
	new CgenNode(class_(prim_slot,No_class,nil_Features(),filename),
			    Basic,this));

// 
// The Object class has no parent class. Its methods are
//        cool_abort() : Object    aborts the program
//        type_name() : Str        returns a string representation of class name
//        copy() : SELF_TYPE       returns a copy of the object
//
// There is no need for method bodies in the basic classes---these
// are already built in to the runtime system.
//
  install_class(
   new CgenNode(
    class_(Object, 
	   No_class,
	   append_Features(
           append_Features(
           single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
           single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
           single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	   filename),
    Basic,this));

// 
// The IO class inherits from Object. Its methods are
//        out_string(Str) : SELF_TYPE          writes a string to the output
//        out_int(Int) : SELF_TYPE               "    an int    "  "     "
//        in_string() : Str                    reads a string from the input
//        in_int() : Int                         "   an int     "  "     "
//
   install_class(
    new CgenNode(
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
	   filename),	    
    Basic,this));

//
// The Int class has no methods and only a single attribute, the
// "val" for the integer. 
//
   install_class(
    new CgenNode(
     class_(Int, 
	    Object,
            single_Features(attr(val, prim_slot, no_expr())),
	    filename),
     Basic,this));

//
// Bool also has only the "val" slot.
//
    install_class(
     new CgenNode(
      class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename),
      Basic,this));

//
// The class Str has a number of slots and operations:
//       val                                  ???
//       str_field                            the string itself
//       length() : Int                       length of the string
//       concat(arg: Str) : Str               string concatenation
//       substr(arg: Int, arg2: Int): Str     substring
//       
   install_class(
    new CgenNode(
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
	     filename),
        Basic,this));

}

// CgenClassTable::install_class
// CgenClassTable::install_classes
//
// install_classes enters a list of classes in the symbol table.
//
void CgenClassTable::install_class(CgenNodeP nd)
{
  Symbol name = nd->get_name();

  if (probe(name))
    {
      return;
    }

  // The class name is legal, so add it to the list of classes
  // and the symbol table.
  nds = new List<CgenNode>(nd,nds);
  addid(name,nd);
}

void CgenClassTable::install_classes(Classes cs)
{
  for(int i = cs->first(); cs->more(i); i = cs->next(i))
    install_class(new CgenNode(cs->nth(i),NotBasic,this));
}

//
// CgenClassTable::build_inheritance_tree
//
void CgenClassTable::build_inheritance_tree()
{
  for(List<CgenNode> *l = nds; l; l = l->tl())
      set_relations(l->hd());
}

//
// CgenClassTable::set_relations
//
// Takes a CgenNode and locates its, and its parent's, inheritance nodes
// via the class table.  Parent and child pointers are added as appropriate.
//
void CgenClassTable::set_relations(CgenNodeP nd)
{
  CgenNode *parent_node = probe(nd->get_parent());
  nd->set_parentnd(parent_node);
  parent_node->add_child(nd);
}

void CgenNode::add_child(CgenNodeP n)
{
  children = new List<CgenNode>(n,children);
}

void CgenNode::set_parentnd(CgenNodeP p)
{
  assert(parentnd == NULL);
  assert(p != NULL);
  parentnd = p;
}



void CgenClassTable::code()
{
  if (cgen_debug) cout << "coding global data" << endl;
  code_global_data();

  if (cgen_debug) cout << "choosing gc" << endl;
  code_select_gc();

  if (cgen_debug) cout << "coding constants" << endl;
  code_constants();

//                 Add your code to emit
//                   - prototype objects
  if (cgen_debug) cout << "coding protObj" << endl;
  code_protObj();
//                   - class_nameTab
  if (cgen_debug) cout << "coding class_nameTab" << endl;
  code_class_nameTab();
//                   - dispatch tables
  if (cgen_debug) cout << "coding dispatch tables" << endl;
  code_dispTab();
//

  if (cgen_debug) cout << "coding global text" << endl;
  code_global_text();

//                 Add your code to emit
//                   - object initializer
  if (cgen_debug) cout << "coding object intializer" << endl;
  code_object_init();
//                   - the class methods
  if (cgen_debug) cout << "coding class methods" << endl;
  code_methods();
//                   - etc...

}


CgenNodeP CgenClassTable::root()
{
   return probe(Object);
}


///////////////////////////////////////////////////////////////////////
//
// CgenNode methods
//
///////////////////////////////////////////////////////////////////////

CgenNode::CgenNode(Class_ nd, Basicness bstatus, CgenClassTableP ct) :
   class__class((const class__class &) *nd),
   parentnd(NULL),
   children(NULL),
   basic_status(bstatus)
{ 
   stringtable.add_string(name->get_string());          // Add class name to string table
}

void CgenNode::get_attrs(std::vector<attr_class *> &attrs, bool inherit)
{
  if(inherit && parentnd)
    parentnd->get_attrs(attrs,inherit);
  for(int i = features->first(); features->more(i); i = features->next(i)) {
    if(dynamic_cast<attr_class *>(features->nth(i)) != NULL)
      attrs.push_back(dynamic_cast<attr_class *>(features->nth(i)));
  }
}

void CgenNode::get_methods(std::vector<std::string> &methods)
{
  if(parentnd)
    parentnd->get_methods(methods);
  for(int i = features->first(); features->more(i); i = features->next(i)) {
    method_class* m = dynamic_cast<method_class *>(features->nth(i));
    if(m != NULL)
      methods.push_back(std::string(name->get_string()) + "." + std::string(m->name->get_string()));
  }
}

// another version of get_methods, parameter is std::vector<method_class *> &methods and bool inherit
void CgenNode::get_methods(std::vector<method_class *> &methods, bool inherit) {
  if(inherit && parentnd)
    parentnd->get_methods(methods,inherit);
  for(int i = features->first(); features->more(i); i = features->next(i)) {
    method_class* m = dynamic_cast<method_class *>(features->nth(i));
    if(m != NULL)
      methods.push_back(m);
  }
}

void CgenNode::code_def(ostream& s)
{
  s << WORD << "-1" << endl;
  emit_protobj_ref(name,s);  s  << LABEL;
  s << WORD << tag-- << endl;
  std::vector<attr_class *> attrs;
  get_attrs(attrs,true);
  s << WORD << attrs.size() + 3 << endl;
  s << WORD << name << DISPTAB_SUFFIX << endl;
  for(auto it = attrs.begin(); it != attrs.end(); it++) {
    s << WORD;
    if((*it)->type_decl == Int) {
      inttable.lookup_string("0")->code_ref(s);
    }
    else if((*it)->type_decl == Str) {
      stringtable.lookup_string("")->code_ref(s);
    }
    else if((*it)->type_decl == Bool) {
      truebool.code_ref(s);
    }
    else
      s << "0";
    s << endl;
  }
}

void CgenNode::code_dispTab(ostream& s)
{
  emit_disptable_ref(name,s);  s << LABEL;
  std::vector<std::string> methods;
  get_methods(methods);
  for(auto it = methods.begin();it != methods.end();it++) {
    s << WORD << *it << endl;
  }
}

//FIXME
void CgenNode::code_init(ostream &s)
{
  emit_init_ref(name,s);  s << LABEL;
  
  emit_addiu(SP, SP, -DEFAULT_OBJFIELDS * WORD_SIZE, s);
  emit_store(FP, 3, SP, s);
  emit_store(SELF, 2, SP, s);
  emit_store(RA, 1, SP, s);
  emit_addiu(FP, SP, 4, s);

  emit_move(SELF, ACC, s);
  if(parentnd->name != No_class) {
    BUILD_ADDRESS(parentnd->name, CLASSINIT_SUFFIX);
    emit_jal(address.get(), s);
  }

  std::vector<attr_class *> own_attrs;
  std::vector<attr_class *> all_attrs;
  get_attrs(own_attrs,false);
  get_attrs(all_attrs,true);
  int index = 0;
  for(auto it = own_attrs.begin(); it != own_attrs.end(); it++) {
    Symbol init_type;
    if((init_type = (*it)->init->type) != NULL) {
      if((init_type = (*it)->init->type) == Str) {
        string_const_class * strp = dynamic_cast<string_const_class *>((*it)->init);
        emit_load_string(ACC, stringtable.lookup_string(strp->token->get_string()), s); 
      }
      else if((init_type = (*it)->init->type) == Int) {
        int_const_class * intp = dynamic_cast<int_const_class *>((*it)->init);
        emit_load_int(ACC, inttable.lookup_string(intp->token->get_string()), s); 
      }
      else if((init_type = (*it)->init->type) == Bool) {
        bool_const_class * boolp = dynamic_cast<bool_const_class *>((*it)->init);
        emit_load_bool(ACC,BoolConst(boolp->val),s);
      }
      else {
        BUILD_ADDRESS(init_type, PROTOBJ_SUFFIX);
        emit_load_address(ACC, address.get(), s);
        emit_jal("Object.copy", s);
        RESET_ADDRESS(init_type, CLASSINIT_SUFFIX);
        emit_jal(address.get(), s);
      }
      emit_store(ACC, 3 + (all_attrs.size() - own_attrs.size()) + index,SELF, s);
    }
    index++;
  }

  emit_move(ACC, SELF, s);
  emit_load(FP, 3, SP, s);
  emit_load(SELF, 2, SP, s);
  emit_load(RA, 1, SP, s);
  emit_addiu(SP, SP, DEFAULT_OBJFIELDS * WORD_SIZE, s);
  emit_return(s);
}

void CgenNode::code_methods(ostream& s)
{
  env.enterscope();
  // record offset of attributes in this class
  std::vector<attr_class *> attrs;
  get_attrs(attrs,true);
  int index = 0;
  for(auto it = attrs.begin(); it != attrs.end(); it++) {
    Reference * ref = new OffsetRef(SELF,DEFAULT_OBJFIELDS + index);
    env.addid((*it)->name, ref);
    index++;
  }
  std::vector<method_class *> methods;
  get_methods(methods,false);
  // deal with each method
  for(auto it = methods.begin(); it != methods.end(); it++) {
    env.enterscope();
    // generate method name
    s << name << "." << (*it)->name << LABEL;
    // get temporaries number
    int temp_num = (*it)->get_temp_num();
    env.set_temp_num(temp_num);
    // record offset of paramters in this methods
    int i;
    for(i = (*it)->formals->first(); (*it)->formals->more(i);i = (*it)->formals->next(i)) {
      Reference * ref = new OffsetRef(FP,DEFAULT_OBJFIELDS + temp_num + (*it)->formals->len() - 1 - i);
      env.addid(dynamic_cast<formal_class *>((*it)->formals->nth(i))->name, ref);
    }
    env.set_param_num(i); // save count of parameters
    // generate codes which setup stack frame
    (*it)->set_stack_frame(s);
    // generate codes of expr
    (*it)->expr->code(s);
    // generate codes which restore stack frame
    (*it)->restore_stack_frame(s);
    env.exitscope();
    env.clear_temporaries();
  }
  env.exitscope();
}

void method_class::set_stack_frame(ostream &stream) {
  int stack_count = DEFAULT_OBJFIELDS + env.get_temp_num();
  emit_addiu(SP, SP, -(WORD_SIZE * stack_count), stream); 
  emit_store(FP, stack_count, SP, stream);
  emit_store(SELF, stack_count - 1, SP, stream);
  emit_store(RA, stack_count - 2, SP, stream);
  emit_addiu(FP, SP, WORD_SIZE, stream);
  emit_move(SELF, ACC, stream);
  //save $s1-$s6
  for(int i = 0; i < SAVE_REG_COUNT && i < env.get_temp_num(); i++) 
    emit_store(save_regs[i], env.get_temp_num() - 1 - i, FP, stream);
}

void method_class::restore_stack_frame(ostream &stream) {
  int stack_count = DEFAULT_OBJFIELDS + env.get_temp_num();
  // restore $s1-$s6
  for(int i = 0; i < SAVE_REG_COUNT && i < env.get_temp_num(); i++) 
    emit_load(save_regs[i], env.get_temp_num() - 1 - i, FP, stream);
  emit_load(FP, stack_count, SP, stream);
  emit_load(SELF, stack_count - 1, SP, stream);
  emit_load(RA, stack_count - 2, SP, stream);
  emit_addiu(SP, SP, WORD_SIZE * (stack_count + env.get_param_num()), stream);
  emit_return(stream);
}

//******************************************************************
//
//   Fill in the following methods to produce code for the
//   appropriate expression.  You may add or remove parameters
//   as you wish, but if you do, remember to change the parameters
//   of the declarations in `cool-tree.h'  Sample code for
//   constant integers, strings, and booleans are provided.
//
//*****************************************************************

REF_PTR assign_class::code(ostream &s) {
  Reference *var_ref = env.lookup(name);
  REG_PTR expr_ref = TO_REG_PTR(expr->code(s)); //should always be reference of ACC

  if(dynamic_cast<RegisterRef *>(var_ref) != NULL) {
    // reg1 <- reg2
    if(strcmp(expr_ref->get_regname(),var_ref->get_regname())) {
      emit_move(var_ref->get_regname(), expr_ref->get_regname(), s);
    }
  }
  else if(dynamic_cast<OffsetRef *>(var_ref) != NULL) {
    // offset <- reg
    OffsetRef * var_offset_ref = dynamic_cast<OffsetRef *>(var_ref);
    emit_store(expr_ref->get_regname(), var_offset_ref->get_offset(), var_offset_ref->get_regname(), s);
  }
  return expr_ref; // we cannot return var_ref or the smart pointer wrapper of it since it's a pointer
}

REF_PTR static_dispatch_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

// TODO
REF_PTR dispatch_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR cond_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR loop_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR typcase_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR block_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR let_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR arith(Expression e1,Expression e2, char *op,ostream &s) {
  REF_PTR e1_ref = e1->code(s);
  e2->code(s); // reference of e2 should always be ACC(I think)
  emit_jal("Object.copy", s);
  // if e1_ref is not a RegisterRef, we need to load it into T1
  if(TO_OFFSET_PTR(e1_ref) != NULL) {
    OFFSET_PTR e1_offset_ref = TO_OFFSET_PTR(e1_ref);
    emit_load(T1, e1_offset_ref->get_offset(), e1_offset_ref->get_regname(), s);
    e1_ref.reset(new RegisterRef(T1));
  }
  emit_load(T2, 3, ACC, s);
  emit_load(T1, 3, e1_ref->get_regname(), s);
  // emit_add(T1, T1, T2, s);
  emit_binop(op, T1, T1, T2, s);
  emit_store(T1, 3, ACC, s);
  env.back_temporaries_index(1); 
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR plus_class::code(ostream &s) {
  return arith(e1,e2,ADD,s); 
}

REF_PTR sub_class::code(ostream &s) {
  return arith(e1,e2,SUB,s); 
}

REF_PTR mul_class::code(ostream &s) {
  return arith(e1,e2,MUL,s); 
}

REF_PTR divide_class::code(ostream &s) {
  return arith(e1,e2,DIV,s); 
}

REF_PTR neg_class::code(ostream &s) {
  REG_PTR e1_ref = TO_REG_PTR(e1->code(s));
  emit_jal("Object.copy", s);
  emit_load(T1, 3, ACC, s);
  emit_neg(T1, T1, s);
  emit_store(T1, 3, ACC, s);
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR lt_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR eq_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR leq_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR comp_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR int_const_class::code(ostream& s)  
{
  //
  // Need to be sure we have an IntEntry *, not an arbitrary Symbol
  //
  REF_PTR ref = env.get_new_temporary();
  if(TO_REG_PTR(ref) != NULL) {
    REG_PTR reg_ref = TO_REG_PTR(ref);
    emit_load_int(reg_ref->get_regname(),inttable.lookup_string(token->get_string()),s);
    return reg_ref;
  }
  else if(TO_OFFSET_PTR(ref) != NULL) {
    OFFSET_PTR offset_ref = TO_OFFSET_PTR(ref);
    emit_load_int(ACC,inttable.lookup_string(token->get_string()),s);
    emit_store(ACC, offset_ref->get_offset(), offset_ref->get_regname(), s);
    return offset_ref;
  }
}

REF_PTR string_const_class::code(ostream& s)
{
  REF_PTR ref = env.get_new_temporary();
  if(TO_REG_PTR(ref) != NULL) {
    REG_PTR reg_ref = TO_REG_PTR(ref);
    emit_load_string(reg_ref->get_regname(),stringtable.lookup_string(token->get_string()),s);
    return reg_ref;
  }
  else if(TO_OFFSET_PTR(ref) != NULL) {
    OFFSET_PTR offset_ref = TO_OFFSET_PTR(ref);
    emit_load_string(ACC,stringtable.lookup_string(token->get_string()),s);
    emit_store(ACC, offset_ref->get_offset(), offset_ref->get_regname(), s);
    return offset_ref;
  }
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR bool_const_class::code(ostream& s)
{
  emit_load_bool(ACC, BoolConst(val), s);

  REF_PTR ref = env.get_new_temporary();
  if(TO_REG_PTR(ref) != NULL) {
    REG_PTR reg_ref = TO_REG_PTR(ref);
    emit_load_bool(reg_ref->get_regname(),BoolConst(val),s);
    return reg_ref;
  }
  else if(TO_OFFSET_PTR(ref) != NULL) {
    OFFSET_PTR offset_ref = TO_OFFSET_PTR(ref);
    emit_load_bool(ACC,BoolConst(val),s);
    emit_store(ACC, offset_ref->get_offset(), offset_ref->get_regname(), s);
    return offset_ref;
  }
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR new__class::code(ostream &s) {
  BUILD_ADDRESS(type_name, PROTOBJ_SUFFIX);
  emit_load_address(ACC, address.get(), s);
  emit_jal("Object.copy", s);
  RESET_ADDRESS(type_name, CLASSINIT_SUFFIX);
  emit_jal(address.get(), s);

  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR isvoid_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR no_expr_class::code(ostream &s) {
  return REF_PTR(new RegisterRef(ACC));
}

REF_PTR object_class::code(ostream &s) {
  // it's a little bit strange for using both smart pointer and pointer, but I don't want to modify framework code of this assignment.
  Reference *object_ref = env.lookup(name);
  REF_PTR ref = env.get_new_temporary();

  if(dynamic_cast<OffsetRef *>(object_ref) != NULL) {
    OffsetRef* object_offset_ref = dynamic_cast<OffsetRef *>(object_ref);
    if(TO_REG_PTR(ref) != NULL) {
      REG_PTR reg_ref = TO_REG_PTR(ref);
      emit_load(reg_ref->get_regname(), object_offset_ref->get_offset(), object_offset_ref->get_regname(), s);
      return reg_ref;
    }
    else if(TO_OFFSET_PTR(ref) != NULL) {
      OFFSET_PTR offset_ref = TO_OFFSET_PTR(ref);
      emit_load(ACC, object_offset_ref->get_offset(), object_offset_ref->get_regname(), s);
      emit_store(ACC, offset_ref->get_offset(), offset_ref->get_regname(), s);
      return offset_ref;
    }
  }
  // TODO: only consider object which is an attribute or parameter now
}

///////////////////////////////////////////////////////////////////////
//
// get temporaries number of each expr
//
///////////////////////////////////////////////////////////////////////

int binary_op_temp(Expression e1,Expression e2)
{
  int e1_temp_num = e1->get_temp_num();
  int e2_temp_num = e2->get_temp_num();
  return ((e1_temp_num) > 1 + e2_temp_num) ? e1_temp_num : 1 + e2_temp_num;
}

int method_class::get_temp_num()
{
  return expr->get_temp_num();
}

int assign_class::get_temp_num() {
  return expr->get_temp_num();
}

int static_dispatch_class::get_temp_num() {
    int max_temp_num = expr->get_temp_num();
    for (int i = 0; i < actual->len(); i++) {
        int temp_num = actual->nth(i)->get_temp_num();
        if (temp_num > max_temp_num) {
            max_temp_num = temp_num;
        }
    }
    return max_temp_num;
}

int dispatch_class::get_temp_num() {
    int max_temp_num = expr->get_temp_num();
    for (int i = 0; i < actual->len(); i++) {
        int temp_num = actual->nth(i)->get_temp_num();
        if (temp_num > max_temp_num) {
            max_temp_num = temp_num;
        }
    }
    return max_temp_num;
}

//TODO
int cond_class::get_temp_num() {
    int max_temp_num = pred->get_temp_num();
    int then_temp_num = then_exp->get_temp_num();
    int else_temp_num = else_exp->get_temp_num();
    if (then_temp_num > max_temp_num) {
        max_temp_num = then_temp_num;
    }
    if (else_temp_num > max_temp_num) {
        max_temp_num = else_temp_num;
    }
    return max_temp_num;
}

//TODO
int loop_class::get_temp_num() {
    int max_temp_num = pred->get_temp_num();
    int body_temp_num = body->get_temp_num();
    if (body_temp_num > max_temp_num) {
        max_temp_num = body_temp_num;
    }
    return max_temp_num;
}

//TODO
int typcase_class::get_temp_num() {
  /*
    int max_temp_num = expr->get_temp_num();
    for (int i = 0; i < cases->len(); i++) {
        int case_temp_num = cases->nth(i)->get_temp_num();
        if (case_temp_num > max_temp_num) {
            max_temp_num = case_temp_num;
        }
    }
    return max_temp_num;
    */
    return 0;
}

int block_class::get_temp_num() {
    int max_temp_num = 0;
    for (int i = 0; i < body->len(); i++) {
        int temp_num = body->nth(i)->get_temp_num();
        if (temp_num > max_temp_num) {
            max_temp_num = temp_num;
        }
    }
    return max_temp_num;
}

int let_class::get_temp_num() {
  int init_temp_num = init->get_temp_num();
  int body_temp_num = body->get_temp_num();
  return 1 + init_temp_num + body_temp_num;
}

int plus_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int sub_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int mul_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int divide_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int neg_class::get_temp_num() {
  return e1->get_temp_num();
}

int lt_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int eq_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int leq_class::get_temp_num() {
  return binary_op_temp(e1,e2); 
}

int comp_class::get_temp_num() {
  return e1->get_temp_num();
}

int int_const_class::get_temp_num() {
   return 0;
}

int bool_const_class::get_temp_num() {
   return 0;
}

int string_const_class::get_temp_num() {
   return 0;
}

int new__class::get_temp_num() {
   return 0;
}

int isvoid_class::get_temp_num() {
   return e1->get_temp_num();
}

int no_expr_class::get_temp_num() {
   return 0;
}

int object_class::get_temp_num() {
   return 0;
}

///////////////////////////////////////////////////////////////////////
//
// Environment
//
///////////////////////////////////////////////////////////////////////

void Environment::set_temp_num(int n) {
  temp_num = n;
  int i;
  for(i = 0;i < SAVE_REG_COUNT && i < n;i++)
  {
    if(n <= SAVE_REG_COUNT)
      temporaries.push_back(REG_PTR(new RegisterRef(save_regs[n - 1 - i])));
    else
      temporaries.push_back(REG_PTR(new RegisterRef(save_regs[SAVE_REG_COUNT - 1 - i])));
  }
  for(;i < n;i++)
  {
    temporaries.push_back(OFFSET_PTR(new OffsetRef(FP,i - SAVE_REG_COUNT)));
  }
  temporaries.push_back(REG_PTR(new RegisterRef(ACC)));
  temporaries_index = 0;
}

void Environment::clear_temporaries() {
  std::vector<REF_PTR>().swap(temporaries);
}

REF_PTR Environment::get_new_temporary() {
  if(next_varibales.empty())
    return temporaries[temporaries_index++];
  else 
    return next_varibales.top();
}