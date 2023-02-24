(* models one-dimensional cellular automaton on a circle of finite radius
   arrays are faked as Strings,
   X's respresent live cells, dots represent dead cells,
   no error checking is done *)

(* abcefg(*xfwf(*xxxxxxx*)wfwfw*) *)

class CellularAutomaton inherits IO {
    --test--eeee(*feeeee*)
    population_map : String;
};