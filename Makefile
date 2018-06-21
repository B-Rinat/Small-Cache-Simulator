all:
	gcc -O3 cache_sim.c -o cache_sim
	
debug:
	gcc -g cache_sim.c -o cache_sim_debug

clean:
	rm -f cache_sim
	rm -f cache_sim_debug
