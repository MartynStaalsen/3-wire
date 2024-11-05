import pynode
import time

master = pynode.duplex_pynode_master()

N = 10
network = [master]
for i in range(N):
    new_node = pynode.duplex_pynode(upstream=network[-1])
    master.add_node(new_node)
    network.append(new_node)
    master.known_nodes = 0

master.q.clear()
master.send('hello')
time.sleep(1)
print(f'master contents @ {time.time()}')
print(f'{master.q}')
master.known_nodes = len(master.q)
print(f'known nodes: {master.known_nodes}')
print()

R = 10
for t in range(R):
    master.q.clear()
    master.send(chr(ord('a')+t))
    now = time.time()
    # wait for all known nodes to return
    while len(master.q) < master.known_nodes:
        time.sleep(0.05)
        if time.time() - now > 5:
            print('timeout')
            break

    # wait for data from all nodes
    print(f'all returned after {time.time() - now} seconds')
    print(f'{master.q}')
    for n in network[1:]:
        print(f'\t{n.index}: {n.data}, {n.q0}, {n.q1}')
    time.sleep(0.2)

for n in network[1:]:
    n.closeout()
