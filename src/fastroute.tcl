
sta::define_cmd_args "run_global_routing" {[-output_file out_file]}

proc run_global_routing { args } {
    sta::parse_key_args "run_global_routing" args \
        keys {-output_file} flags {}

    if { [info exists keys(-output_file)] } {
        set out_file $keys(-output_file)
        FastRoute::set_output_file $out_file
    } else {
        puts "WARNING: Default output guide name: out.guide"
        FastRoute::set_output_file "out.guide"
    }

    FastRoute::start_fastroute
    FastRoute::run_fastroute
}