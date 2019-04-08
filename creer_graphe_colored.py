import networkx as nx
import matplotlib.pyplot as plt
import random

G=nx.Graph()

G.add_nodes_from(range(400))

color_map=[]

with open("labels.txt","r") as fl :
	for line in fl :
		champs=line.split()
		if champs[1] == "3" :
			color_map.append('blue');
		elif champs[1] == "102" :
			color_map.append('green');
		elif champs[1] == "202" :
			color_map.append('red');		
		elif champs[1] == "305" :
			color_map.append('yellow');

# edges
with open("graphe_ex1.txt","r") as f :
	for line in f :
		champs=line.split()
		G.add_edge(int(champs[0]), int(champs[1]))

nx.draw_networkx(G, node_color = color_map, node_size=20, with_labels=False)

plt.show()
