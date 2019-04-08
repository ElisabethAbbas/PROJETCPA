import networkx as nx
import matplotlib.pyplot as plt
import random

G=nx.Graph()

G.add_nodes_from(range(400))

f= open("graphe_ex1.txt","w")

p=0.5
q=0.25
for i in range(400):
	for j in range(400):
		if(i//100==j//100):
			if random.random()<p:
				f.write("%d %d\n" %(i,j))
				G.add_edge(i,j)
		elif random.random()<q:
			f.write("%d %d\n" %(i,j))
			G.add_edge(i,j)

f.close()

nx.draw_networkx(G, node_size=15, node_color='y', node_shape='h', with_labels=False)
plt.show()
