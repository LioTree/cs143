class CellularAutomaton inherits IO {
    init(map : String) : SELF_TYPE {
        {
            population_map <- map;
            self;
        }
    };
}