read_lef "input.lef"
read_def "input.def"

fastroute -output_file "route.guide" \
	  -max_routing_layer 10 \
	  -unidirectional_routing \
	  -max_length_per_layer {{1 10.0} {2 20.0} {3 15.0} {4 10.0} {5 5.0} {6 10.0} {7 10.0} {8 10.0} {9 10.0} {10 10.0}} \

exit

