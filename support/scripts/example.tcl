set_lef_files "path/to/example.lef"
set_def_files "path/to/example.def"
set_output_file "example.guide"

parse_input_files

set_capacity_adjustment 0.X
set_min_layer Y
set_max_layer Z
set_unidirectional_routing B

run_fastroute

exit

# set_lef_files: string input. set the lef files that will be loaded. can be called multiple times
# set_def_files: string input. set the def files that will be loaded. can be called multiple times
# set_output_file: string input. indicate the name of the generated guides file. do not need ".guide" extension

# parse_input_files: load LEF/DEF input files

# set_capacity_adjustment: float input. indicate the percentage reduction of each edge. optional
# set_min_layer: integer input. indicate the min routing layer available for FastRoute. optional
# set_max_layer: integer input. indicate the max routing layer available for FastRoute. optional
# set_unidirectional_routing: boolean input. indicate if unidirectional routing is activated. optional

# run_fastroute: execute FastRoute4-lefdef flow